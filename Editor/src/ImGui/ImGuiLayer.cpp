#include "depch.h"
#include "ImGuiLayer.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include "Engine/Core/Application.h"

#include "ImGuizmo.h"

namespace Engine
{
	ImGuiLayer::ImGuiLayer() : Layer(TEXT("ImGuiLayer"))
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		
		io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Bold.ttf", 16.f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans/OpenSans-Regular.ttf", 16.f);

		ImGuiStyle& style = ImGui::GetStyle();

		ImGui::StyleColorsDark();

		SetUIDarkBlueTheme();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.f;
			style.Colors[ImGuiCol_WindowBg].w = 1.f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();

		io.DisplaySize = ImVec2((float)(app.GetWindow().GetWidth()), (float)(app.GetWindow().GetHeight()));

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetUITheme(bool Dark, float Alpha)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6000000238418579f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 8.0f;
		style.WindowBorderSize = 0.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ChildRounding = 5.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 5.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(5.0f, 3.0f);
		style.FrameRounding = 5.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(6.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.CellPadding = ImVec2(6.099999904632568f, 6.0f);
		style.IndentSpacing = 20.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 5.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.2156862765550613f, 0.2156862765550613f, 0.2156862765550613f, 0.3921568691730499f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.2745098173618317f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1176470592617989f, 0.1176470592617989f, 0.6078431606292725f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.5411764979362488f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 0.5411764979362488f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2745098173618317f, 0.2745098173618317f, 0.2745098173618317f, 0.3921568691730499f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5764706134796143f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.1176470592617989f, 0.1176470592617989f, 0.1176470592617989f, 0.2901960909366608f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 0.2901960909366608f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8627451062202454f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9725490212440491f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7019608020782471f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3490196168422699f);
	}

	void ImGuiLayer::SetUIDarkBlueTheme()
	{
		ImVec4 color = ImVec4(0.2f, 0.2f, 0.2f, 1.f);

		ImGuiStyle& style = ImGui::GetStyle();

		const float kC = 0.2f;
		const float lum = color.x * .299f + color.y * .587f + color.z * .114f;
		const float lumK = 1 + lum * 0.25f;

		ImVec4 wBG(color.x * kC, color.y * kC, color.z * kC, 1.0f);

		//ImVec4 clA(0.00f, 0.31f, 0.40f, 1.00f);
		//ImVec4 clB(0.00f, 0.21f, 0.27f, 1.00f);

		ImVec4 clA(wBG.x * 3.0f * lumK, wBG.y * 3.0f * lumK, wBG.z * 3.0f * lumK, 1.00f); //0.00f, 0.55f, 0.87f
		ImVec4 clB(wBG.x * 1.5f * lumK, wBG.y * 1.5f * lumK, wBG.z * 1.5f * lumK, 1.00f); //0.20f, 0.22f, 0.27f
		ImVec4 clC(wBG.x * 4.0f * lumK, wBG.y * 4.0f * lumK, wBG.z * 4.0f * lumK, 1.00f); //0.20f, 0.22f, 0.27f


		ImVec4 txt(0.90f, 0.90f, 0.90f, 1.00f); //0.86f, 0.93f, 0.89f    

		//ImVec4 act(0.00f, 0.25f, 0.57f, 1.00f); //0.86f, 0.93f, 0.89f
		ImVec4 act(wBG.x * 3.75f, wBG.y * 3.75f, wBG.z * 3.75f, 1.00f);
		const float diff = 1.f / (lumK * lumK);
		act.x *= diff;
		act.y *= diff;
		act.z *= diff;

		ImVec4 lin(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 chk(0.60f * lumK, 0.60f * lumK, 0.60f * lumK, 1.0f);

		const ImVec4 ch1(chk.x * 0.60f, chk.y * 0.60f, chk.z * 0.60f, 1.00f);
		const ImVec4 ch2(chk.x * 0.80f, chk.y * 0.80f, chk.z * 0.80f, 1.00f);

		//ImVec4 chk(0.00f, 0.25f, 0.57f, 1.00f);

		style.Colors[ImGuiCol_Text] = ImVec4(txt.x, txt.y, txt.z, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(txt.x, txt.y, txt.z, 0.45f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(clB.x, clB.y, clB.z, 0.30f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(clA.x, clA.y, clA.z, 0.66f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(clA.x, clA.y, clA.z, 0.60f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(act.x, act.y, act.z, 0.75f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(act.x, act.y, act.z, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(clB.x, clB.y, clB.z, 0.85f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(wBG.x, wBG.y, wBG.z, 0.66f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(act.x, act.y, act.z, 0.85f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(clB.x, clB.y, clB.z, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(clB.x, clB.y, clB.z, 0.25f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(clA.x, clA.y, clA.z, 0.50f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ch1;
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ch2;
		style.Colors[ImGuiCol_CheckMark] = ImVec4(chk.x, chk.y, chk.z, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ch1;
		style.Colors[ImGuiCol_SliderGrabActive] = ch2;
		style.Colors[ImGuiCol_Button] = ImVec4(clC.x, clC.y, clC.z, 0.40f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(act.x, act.y, act.z, 0.50f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(act.x, act.y, act.z, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(act.x, act.y, act.z, 0.50f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(act.x, act.y, act.z, 0.75f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(act.x, act.y, act.z, 1.00f);
		style.Colors[ImGuiCol_Separator] = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(clA.x, clA.y, clA.z, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(clA.x, clA.y, clA.z, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(clA.x, clA.y, clA.z, 0.75f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ch1;
		style.Colors[ImGuiCol_ResizeGripActive] = ch2;
		style.Colors[ImGuiCol_PlotLines] = ImVec4(lin.x, lin.y, lin.z, 0.75f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(lin.x, lin.y, lin.z, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ch1;
		style.Colors[ImGuiCol_PlotHistogramHovered] = ch2;
		style.Colors[ImGuiCol_PopupBg] = ImVec4(clB.x, clB.y, clB.z, 0.9f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(clB.x, clB.y, clB.z, 0.73f);

		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}
}