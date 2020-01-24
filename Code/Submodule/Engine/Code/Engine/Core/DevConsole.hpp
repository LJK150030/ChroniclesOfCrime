#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <vector>


class Camera;
class Shader;
class GPUMesh;
class Clock;

class DevConsole
{
public:
	const static Rgba CONSOLE_INFO;
	const static Rgba CONSOLE_ECHO;
	const static Rgba CONSOLE_ERROR;
	const static Rgba CONSOLE_ERROR_DESC;

public:
	explicit DevConsole();
	~DevConsole();
	void Startup(Clock* dev_clock);
	void BeginFrame();
	void Update(double delta_seconds);
	void EndFrame();
	void ShutDown();

	void PrintString(const std::string& line, const Rgba& color, float current_time);
	void PrintString(const Rgba& color, const std::string& line);
	void Render(RenderContext* const& renderer, const Camera& camera) const;
	void ExecuteCommandString(const std::string& command_string);
	void SetFontSize(const float font_size);

	static bool CommandTest(EventArgs& args);
	bool ReadInChar(const unsigned char key_code);
	void RemoveCharacterBeforeCarrot();
	void RemoveCharacterAfterCarrot();
	void ExecuteLine();
	bool HandleChar(const unsigned char key_code);
	bool HandleKey(const unsigned char key_code);

private:
	void AddLineAtEnd(const Rgba& line_color, const std::string& line_string);
	void ExecuteCommandLine(const std::string& command_line);

private:
	struct DevLine
	{
		DevLine(const std::string& line, const Rgba& color, const float current_time):
			m_line(line), m_color(color), m_timeOfCreation(current_time)
		{
		};

		std::string m_line;
		Rgba m_color;
		float m_timeOfCreation;
	};

	std::vector<DevLine> m_devLog;
	float m_fontSize = 1.0f;
	float m_currentTime = 0.0f;
	int m_frameNumber = 0;
	std::string m_fontFile = "SquirrelFixedFont.png";
	const int m_numLinesBetweenInputNHistory = 3;

	Shader* m_defaultShader = nullptr;

	std::string m_currentLine = "";
	int m_carrotIndex = 0;
	bool m_drawCarrot = true;
	float m_carrotBlinkRate = 0.5f;

	GPUMesh* m_background;
	GPUMesh* m_lineMesh;
	GPUMesh* m_logMesh;
	GPUMesh* m_carrotMesh;

	Clock* m_devClock;
};
