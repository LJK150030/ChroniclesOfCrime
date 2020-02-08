#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"

class Camera;
class DialogueSystem;
class Scenario;
class Material;
class Shader;
class GPUMesh;

class Game
{

public:
	Game();
	~Game();

	void Startup();
	void Shutdown();

	void BeginFrame() const;
	void Update(double delta_seconds);
	
	void Render() const;

	void EndFrame() const;
	
	bool HandleKeyPressed(unsigned char key_code);
	bool HandleKeyReleased(unsigned char key_code);
	void SetDeveloperMode(bool on_or_off);

	//TODO: hacky solution to get the current scenario
	Scenario* GetCurrentScenario() const;

private:
	void GarbageCollection() const;
	void InitCamera();
	void InitGameObjs();

private:

	bool m_inDevMode = false;
	float m_time = 0.0f;
	int m_currentFrame = 0;
	
	Camera* m_gameCamera = nullptr;
	Vec3 m_camPosition = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 m_camEuler = Vec3(0.0f, 0.0f, 0.0f);

	// Game objects
	DialogueSystem* m_dialogueSystem = nullptr;
	Scenario* m_currentScenario = nullptr;

	// TODO: put in a Background obj
	Material* m_woodMaterial = nullptr;
	Shader* m_defaultShader = nullptr;
	GPUMesh* m_quad = nullptr;
	Matrix44 m_quadTransform = Matrix44::IDENTITY; // quad's model matrix
};
