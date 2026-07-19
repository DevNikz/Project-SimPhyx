#include "RenderParticle.h"

using namespace Physics;

void RenderParticle::Draw()
{
	if (PhysicsParticle->IsDestroyed()) return; 
	RenderModel->Color(Color);
	RenderModel->Position(PhysicsParticle->Position);
	RenderModel->Scale(Scale);
	RenderModel->Rotation(glm::vec3(1.f));
	RenderModel->DrawModel();
}

void RenderParticle::Draw(glm::vec3 rot)
{
	if (PhysicsParticle->IsDestroyed()) return;
	RenderModel->Color(Color);
	RenderModel->Position(PhysicsParticle->Position);
	RenderModel->Scale(Scale);
	RenderModel->Rotation(rot);
	RenderModel->DrawModel();
}

//Sprite Animation
void RenderParticle::addClip(const std::string& name, const std::string& texturePath,
	int columns, int rows, int frameCount,
	float frameDuration, int row)
{
	Clip clip;
	clip.texture = Quad::LoadTextureCached(texturePath);
	if (clip.texture.id == 0)
	{
		std::cerr << "[AnimatedSprite] Skipping clip '" << name
			<< "' — texture failed to load: " << texturePath << std::endl;
		return;
	}
	clip.columns = columns;
	clip.rows = rows;
	clip.frameCount = frameCount;
	clip.row = row;
	clip.frameDuration = frameDuration;

	m_clips[name] = clip;

	// First clip added becomes the default so you don't have to
	// explicitly call play() before the first draw().
	if (m_currentClipName.empty())
		play(name);
}

void RenderParticle::play(const std::string& name)
{
	if (name == m_currentClipName) return; // already playing — don't reset

	auto it = m_clips.find(name);
	if (it == m_clips.end())
	{
		std::cerr << "[AnimatedSprite] No such clip: " << name << std::endl;
		return;
	}

	m_currentClipName = name;
	m_timer = 0.0f;
	m_frame = 0;

	const Clip& clip = it->second;
	RenderSprite->setTexture(clip.texture, clip.columns, clip.rows);
	RenderSprite->setFrame(0, clip.row);
}

void RenderParticle::update(float deltaTime)
{
	if (m_currentClipName.empty()) return;

	const Clip& clip = m_clips[m_currentClipName];
	if (clip.frameCount <= 1) return; // static pose, nothing to animate

	m_timer += deltaTime;
	if (m_timer >= clip.frameDuration)
	{
		m_timer -= clip.frameDuration;
		m_frame = (m_frame + 1) % clip.frameCount;
		RenderSprite->setFrame(m_frame, clip.row);
	}
}


void Physics::RenderParticle::DrawSprite()
{
	if (PhysicsParticle->IsDestroyed()) return;
	//RenderModel->Color(Color);
	//RenderModel->Position(PhysicsParticle->Position);
	//RenderModel->Scale(Scale);
	//RenderModel->Rotation(glm::vec3(1.f));
	//RenderModel->DrawModel();

	RenderSprite->setPosition(PhysicsParticle->Position);
	RenderSprite->setRotationDegrees(0.f);
	RenderSprite->setFacingScale(facingScale);
	RenderSprite->draw();
}
