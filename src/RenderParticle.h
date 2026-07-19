#pragma once
#include "Physics/Particle.h"
#include "../Model.h"
#include "../Quad.h"
#include <unordered_map>

namespace Physics {
	class RenderParticle {
	public:
		Particle* PhysicsParticle;
		Model* RenderModel;
		Quad* RenderSprite;
		glm::vec3 Color;
		glm::vec3 Scale;
		float facingScale = 1.f;

		RenderParticle(Particle* p, Model* obj) : PhysicsParticle(p), RenderModel(obj)
		{
			Color = glm::vec3(1.0f, 1.0f, 1.0f);
			Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		};
		RenderParticle(Particle* p, Model* obj, glm::vec3 c, glm::vec3 s) : PhysicsParticle(p), RenderModel(obj), Color(c), Scale(s) 
		{
		};

		RenderParticle(Particle* p, Quad* sprite) : PhysicsParticle(p), RenderSprite(sprite)
		{
		};

		void Draw();
		void Draw(glm::vec3 rot);

		//Sprite Animation
		void addClip(const std::string& name, const std::string& texturePath,
			int columns, int rows, int frameCount,
			float frameDuration, int row = 0);

		void play(const std::string& name);

		void update(float deltaTime);

		void DrawSprite();

		Quad* quad() { return RenderSprite; }
		const Quad* quad() const { return RenderSprite; }
		const std::string& currentClip() const { return m_currentClipName; }

	private:

		//Animation Stuffs
		struct Clip
		{
			Quad::TextureHandle texture;
			int columns = 1;
			int rows = 1;
			int frameCount = 1;
			int row = 0;
			float frameDuration = 0.1f;
		};

		std::unordered_map<std::string, Clip> m_clips;
		std::string m_currentClipName;
		float m_timer = 0.0f;
		int m_frame = 0;
	};
}