#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Clock.hpp"


const STATIC Rgba DevConsole::CONSOLE_INFO(			0.12156862745098039215686274509804f, 
													0.8078431372549019607843137254902f,
													0.7960784313725490196078431372549f, 
													1.0f);
const STATIC Rgba DevConsole::CONSOLE_ECHO(			1.0f, 
													0.85490196078431372549019607843137f, 
													0.4f, 
													1.0f);
const STATIC Rgba DevConsole::CONSOLE_ERROR(		0.92549019607843137254901960784314f, 
													0.28235294117647058823529411764706f,
													0.38823529411764705882352941176471f, 
													1.0f);
const STATIC Rgba DevConsole::CONSOLE_ERROR_DESC(	0.65490196078431372549019607843137f,
													0.24313725490196078431372549019608f,
													0.36078431372549019607843137254902f, 
													1.0f);

STATIC bool DevConsole::CommandTest(EventArgs& args)
{
	g_theDevConsole->PrintString(CONSOLE_INFO, Stringf("Test command received"));
	args.DebugPrintContents();
	return false; // Does not consume event; continue to call other subscribers’ callback functions
}

bool DevConsole::ReadInChar(const unsigned char key_code)
{
	if(key_code == 96) return true;	// 96 tilde, 8 back-space
	std::string string_after_carrot = m_currentLine.substr(m_carrotIndex, m_currentLine.size());
	m_currentLine.erase(m_carrotIndex, m_currentLine.size());
	m_currentLine.push_back(key_code);
	m_currentLine.append(string_after_carrot);
	m_carrotIndex++;
	return true;
}

void DevConsole::RemoveCharacterBeforeCarrot()
{
	if(m_currentLine.empty()) return;
	if(m_carrotIndex == 0) return;

	std::string string_after_carrot = m_currentLine.substr(m_carrotIndex, m_currentLine.size());
	m_currentLine.erase(m_carrotIndex-1, m_currentLine.size());
	m_currentLine.append(string_after_carrot);
	m_carrotIndex--;

}

void DevConsole::RemoveCharacterAfterCarrot()
{
	if(m_currentLine.empty()) return;
	if(m_carrotIndex == static_cast<int>(m_currentLine.size())) return;
	
	std::string string_after_carrot = m_currentLine.substr(m_carrotIndex + 1, m_currentLine.size());
	m_currentLine.erase(m_carrotIndex, m_currentLine.size());
	m_currentLine.append(string_after_carrot);
}

void DevConsole::ExecuteLine()
{
	if(m_currentLine.empty()) return;
	PrintString(CONSOLE_ECHO, m_currentLine);
	ExecuteCommandString(m_currentLine);
	m_currentLine.clear();
	m_carrotIndex = 0;
}

bool DevConsole::HandleChar(const unsigned char key_code)
{
switch (key_code)
	{
		case VK_BACK:
		case VK_RETURN:
		case VK_ESCAPE:
		{
			return true;
		}
		default:
		{
			return ReadInChar(key_code);
		}
	}
}

bool DevConsole::HandleKey(const unsigned char key_code)
{
	switch (key_code)
	{
		case VK_BACK:
		{
			RemoveCharacterBeforeCarrot();
			return true;
		}
		case VK_DELETE:
		{
			RemoveCharacterAfterCarrot();
			return true;
		}
		case VK_RETURN:
		{
			ExecuteLine();
			return true;
		}
		case VK_ESCAPE:
		{
			if(m_currentLine.empty()) DEV_CONSOLE_IN_USE = false;
			m_currentLine.clear();
			m_carrotIndex = 0;
			return true;
		}
		case VK_LEFT:
		{
			m_carrotIndex--;
			if(m_carrotIndex < 0) m_carrotIndex = 0;
			return true;
		}
		case VK_RIGHT:
		{
			m_carrotIndex++;
			if(m_carrotIndex > static_cast<int>(m_currentLine.size())) m_carrotIndex = static_cast<int>(m_currentLine.size());
			return true;
		}
		case VK_OEM_3:
		{
			DEV_CONSOLE_IN_USE = !DEV_CONSOLE_IN_USE;
			return true;
		}
		default:
		{
			return false;
		}
	}
}


void DevConsole::AddLineAtEnd(const Rgba& line_color, const std::string& line_string)
{
	std::vector<std::string> lines = SplitStringOnDelimiter(Stringf("	%s", line_string.c_str()), '\n');

	for (int lineID = 0; lineID < static_cast<int>(lines.size()); lineID++)
	{
		m_devLog.emplace_back(lines[lineID], line_color, static_cast<float>(m_devClock->m_totalTime));
	}
}

void DevConsole::ExecuteCommandLine(const std::string& command_line)
{
	EventArgs args;
	std::vector<std::string> event_and_args = SplitStringOnDelimiter(command_line, ' ');

	const std::string event_name = event_and_args[0];

	for (int arg_id = 1; arg_id < static_cast<int>(event_and_args.size()); ++arg_id)
	{
		std::vector<std::string> key_and_val = SplitStringOnDelimiter(event_and_args[arg_id], '=');

		if (key_and_val.size() != 2)
		{
			PrintString(CONSOLE_ERROR, Stringf("The command, %s, was not read", event_and_args[arg_id].c_str()));
			AddLineAtEnd(CONSOLE_ERROR_DESC,
			             Stringf(
				             "Make sure that the arguments are in a key=value order, \n No spaces are allowed in between key and = nor = and value."));
			return;
		}
		args.SetValue(key_and_val[0], key_and_val[1]);
	}

	g_theEventSystem->FireEvent(event_name, args);
}


DevConsole::DevConsole()
{
	m_fontSize = g_gameConfigBlackboard.GetValue("devConsoleFontSize", m_fontSize);
	m_fontFile = g_gameConfigBlackboard.GetValue("devConsoleFontFile", m_fontFile);
}

DevConsole::~DevConsole() = default;

void DevConsole::PrintString(const std::string& line, const Rgba& color, float current_time)
{
	std::vector<std::string> lines = SplitStringOnDelimiter(
		Stringf("[T: %05F, F: %i]: %s", current_time, m_frameNumber, line.c_str()), '\n');

	for (int lineID = 0; lineID < static_cast<int>(lines.size()); lineID++)
	{
		m_devLog.emplace_back(lines[lineID], color, current_time);
	}
}

void DevConsole::PrintString(const Rgba& color, const std::string& line)
{
	PrintString(line, color, static_cast<float>(m_devClock->m_totalTime));
}

void DevConsole::Render(RenderContext* const& renderer, const Camera& camera) const
{
	if(!DEV_CONSOLE_IN_USE) return;

	// create the dev console boundries
	renderer->BindShader(m_defaultShader);
	renderer->BindSampler(SAMPLE_MODE_POINT);
	renderer->BindTextureView(0U, nullptr);
	const AABB2 dev_bounds(camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight());
	std::vector<Vertex_PCU> dev_verts;

	CPUMesh background_mesh;
	CpuMeshAddQuad(&background_mesh, dev_bounds);
	background_mesh.SetColor(Rgba(0.0f, 0.0f, 0.0f, 0.75f));
	m_background->CreateFromCPUMesh<Vertex_PCU>(background_mesh);
	g_theRenderer->DrawMesh(*m_background);

	float y_dif = dev_bounds.maxs.y - dev_bounds.mins.y;
	const int number_of_lines_allowed = RoundToNearestInt(y_dif / m_fontSize);
	
	const int minimum_index = static_cast<int>(m_devLog.size()) - (1 + number_of_lines_allowed
		+ m_numLinesBetweenInputNHistory);

	// ...each frame; draw two text strings on screen
	// write current input
	BitmapFont* console_font = renderer->CreateOrGetBitmapFont(m_fontFile.c_str());
	renderer->BindTextureView(0U, (console_font->GetTexture()));
	CPUMesh font_mesh;

	if(m_currentLine.size() != 0 )
	{
		console_font->AddMeshForText2D(&font_mesh, Vec2(dev_bounds.mins.x + 1.0f, dev_bounds.mins.y), m_fontSize, m_currentLine, Rgba::WHITE);
		m_lineMesh->CreateFromCPUMesh<Vertex_PCU>(font_mesh);
		g_theRenderer->DrawMesh(*m_lineMesh);
	}

	//std::vector<Vertex_PCU> text_verts;
	//console_font->AddVertsForText2D(text_verts, Vec2(1.0f, 0.0f), m_fontSize, m_currentLine, Rgba::WHITE);

	CPUMesh log_mesh;

	// write console history
	for (int lineID = static_cast<int>(m_devLog.size()) - 1; lineID > minimum_index; lineID--)
	{
		if (lineID < 0) break;

		float text_pos_y = static_cast<float>(static_cast<int>(m_devLog.size()) - lineID - 1 + m_numLinesBetweenInputNHistory) * m_fontSize;

		console_font->AddMeshForText2D(&log_mesh, Vec2(dev_bounds.mins.x + 1.0f, dev_bounds.mins.y + text_pos_y), m_fontSize, m_devLog[lineID].m_line,
		                                m_devLog[lineID].m_color);
	}
	
	if(m_devLog.size() > 0)
	{
		m_logMesh->CreateFromCPUMesh<Vertex_PCU>(log_mesh);
		g_theRenderer->DrawMesh(*m_logMesh);
	}

	// creating carrot key
	if(!m_drawCarrot) return;

	renderer->BindTextureView(0U, nullptr);
	CPUMesh carrot_mesh;
	//std::vector<Vertex_PCU> carrot_verts;
	float carrot_x_pos = static_cast<float>(m_carrotIndex) * m_fontSize + 1.0f + dev_bounds.mins.x;
	CpuMeshAddLine(&carrot_mesh, Vec2(carrot_x_pos, dev_bounds.mins.y),  Vec2(carrot_x_pos, dev_bounds.mins.y + m_fontSize), 0.25f, Rgba::WHITE);
	m_carrotMesh->CreateFromCPUMesh<Vertex_PCU>(carrot_mesh);
	g_theRenderer->DrawMesh(*m_carrotMesh);

// 	AddVertsForLine2D(carrot_verts, Vec2(carrot_x_pos, 0.0f), 
// 		Vec2(carrot_x_pos, m_fontSize), 0.25f, Rgba::WHITE);
//	renderer->DrawVertexArray(carrot_verts);
}

void DevConsole::ExecuteCommandString(const std::string& command_string)
{
	std::vector<std::string> commands = SplitStringOnDelimiter(command_string, '\n');

	for (int command_id = 0; command_id < static_cast<int>(commands.size()); ++command_id)
	{
		ExecuteCommandLine(commands[command_id]);
	}
}

void DevConsole::SetFontSize(const float font_size)
{
	m_fontSize = font_size;
}

void DevConsole::Startup(Clock* dev_clock)
{
	g_theEventSystem->SubscribeEventCallbackFunction("Test", CommandTest);
	m_carrotIndex = static_cast<int>(m_currentLine.size());
	m_defaultShader = g_theRenderer->CreateOrGetShader("default_unlit.hlsl");

	m_background = new GPUMesh(g_theRenderer);
	m_lineMesh = new GPUMesh(g_theRenderer);
	m_logMesh = new GPUMesh(g_theRenderer);
	m_carrotMesh = new GPUMesh(g_theRenderer);

	m_devClock = dev_clock;
}

void DevConsole::BeginFrame()
{
	m_frameNumber++;
}

void DevConsole::Update(double delta_seconds)
{
	m_devClock->Step(delta_seconds);

	if(m_currentTime >= m_carrotBlinkRate)
	{
		m_currentTime = 0.0f;
		m_drawCarrot = !m_drawCarrot;
	}

	m_currentTime += static_cast<float>(m_devClock->m_frameTime);
}

void DevConsole::EndFrame()
{
}

void DevConsole::ShutDown()
{
	delete m_background;
	m_background = nullptr;

	delete m_lineMesh;
	m_lineMesh = nullptr;

	delete m_logMesh;
	m_logMesh = nullptr;

	delete m_carrotMesh;
	m_carrotMesh = nullptr;
}
