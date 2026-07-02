#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>
enum class TaskPriority : int {
	Low = 0,
	Normal = 1,
	High = 2
};

class ThreadScheduler {
public:
	explicit ThreadScheduler(int numThreads = 0) {
		int n = (numThreads == 0)
			? std::max<int>(1, std::thread::hardware_concurrency())
			: numThreads;

		workers.reserve(n);
		for (int i = 0; i < n; i++) workers.emplace_back(&ThreadScheduler::WorkerLoop, this);
	}

    ~ThreadScheduler() { Shutdown(); }
    ThreadScheduler(const ThreadScheduler&) = delete;
    ThreadScheduler& operator=(const ThreadScheduler&) = delete;

    std::future<void> Submit(std::function<void()> fn) {
        auto promise = std::make_shared<std::promise<void>>();
        std::future<void> fut = promise->get_future();

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (stopping) throw std::runtime_error("ThreadScheduler: submit after shutdown");
            queue.emplace([fn = std::move(fn), promise]() mutable {
                try {
                    fn();
                    promise->set_value();
                }
                catch (...) {
                    promise->set_exception(std::current_exception());
                }
            });
        }

        cv.notify_one();
        return fut;
    }

    void SubmitBatch(size_t count,
        std::function<void(size_t start, size_t end)> fn)
    {
        if (count == 0) return;

        const int _workers = workers.size();
        const int chunkSz = (count + _workers - 1) / _workers;

        std::vector<std::future<void>> futures;
        futures.reserve(_workers);

        for (size_t w = 0; w < _workers; ++w) {
            size_t start = w * chunkSz;
            if (start >= count) break;
            size_t end = std::min(start + chunkSz, count);

            futures.push_back(Submit([fn, start, end] {
                fn(start, end);
                }));
        }

        // Wait for all chunks to complete before returning
        for (auto& f : futures) f.get();
    }

    void WaitAll() {
        std::unique_lock<std::mutex> lock(queueMutex);
        cvDone.wait(lock, [this] {
            return queue.empty() && (activeTasks == 0);
            });
    }

    void Shutdown() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (stopping) return;
            stopping = true;
        }
        cv.notify_all();
        for (auto& t : workers) {
            if (t.joinable()) t.join();
        }
    }

    int WorkerCount() const { return workers.size(); }

private:
    void WorkerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [this] {
                    return !queue.empty() || stopping;
                    });
                if (stopping && queue.empty()) return;
                task = std::move(queue.front());
                queue.pop();
                ++activeTasks;
            }
            task();
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                --activeTasks;
            }
            cvDone.notify_all();
        }
    }

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> queue;
	std::mutex queueMutex;
	std::condition_variable cv, cvDone;
	std::atomic<int> activeTasks = 0;
	bool stopping = false;
};