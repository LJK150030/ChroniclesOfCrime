#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Core/WindowContext.hpp"

ImGUISystem::ImGUISystem(RenderContext* context)
{
	m_renderContext = context;
}

ImGUISystem::~ImGUISystem()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGUISystem::Startup()
{
	//Setup imGUI here
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	
	ImGui_ImplWin32_Init(g_theWindow->m_hwnd);
	ImGui_ImplDX11_Init(g_theRenderer->GetDx11Device(), g_theRenderer->GetDx11Context());
	ImGui::StyleColorsDark();

	io.Fonts->AddFontDefault();		
}


void ImGUISystem::BeginFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
}

void ImGUISystem::EndFrame()
{
	//out of principle
}

void ImGUISystem::Render()
{
	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();

	//Set RTV for imGUI
	ColorTargetView* frameCTV = m_renderContext->GetFrameColorTarget();
	ID3D11RenderTargetView* dx_rtx = frameCTV->m_rtv;
	ID3D11DeviceContext* context = m_renderContext->GetDx11Context();
	context->OMSetRenderTargets(1, &dx_rtx, NULL);

	ImGui_ImplDX11_RenderDrawData(data);
}