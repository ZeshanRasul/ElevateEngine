#include <imgui.h>

static void SetupElevateImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Core UI panels (steel grey - brighter than before)
	colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.17f, 0.20f, 1.00f); // #2A2C33
	colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.15f, 0.18f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);

	// Text
	colors[ImGuiCol_Text] = ImVec4(0.93f, 0.95f, 1.00f, 1.00f); // bright
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.52f, 0.58f, 1.00f);

	// Borders
	colors[ImGuiCol_Border] = ImVec4(0.26f, 0.28f, 0.32f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	ImVec4 ACCENT = ImVec4(0.20f, 0.55f, 1.00f, 1.00f);
	ImVec4 ACCENT_HOVER = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
	ImVec4 ACCENT_ACTIVE = ImVec4(0.50f, 0.75f, 1.00f, 1.00f);


	// Frames
	colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.19f, 0.23f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ACCENT_HOVER;
	colors[ImGuiCol_FrameBgActive] = ACCENT_ACTIVE;

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.29f, 0.32f, 0.36f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ACCENT_HOVER;
	colors[ImGuiCol_ScrollbarGrabActive] = ACCENT_ACTIVE;

	// Checkmarks / grabs / sliders
	colors[ImGuiCol_CheckMark] = ACCENT_ACTIVE;
	colors[ImGuiCol_SliderGrab] = ACCENT;
	colors[ImGuiCol_SliderGrabActive] = ACCENT_ACTIVE;

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.22f, 0.23f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ACCENT_HOVER;
	colors[ImGuiCol_ButtonActive] = ACCENT_ACTIVE;

	// Softer header / title / tab background, tuned for industrial blue
	ImVec4 HEADER_BG = ImVec4(0.14f, 0.19f, 0.26f, 1.00f); // dark blue-grey
	ImVec4 HEADER_BG_HOVER = ImVec4(0.18f, 0.24f, 0.32f, 1.00f);
	ImVec4 HEADER_BG_ACTIVE = ImVec4(0.20f, 0.28f, 0.38f, 1.00f);

	// Title bar
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = HEADER_BG_ACTIVE;
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.11f, 1.00f);

	// Headers (tree nodes, collapsing headers, etc.)
	colors[ImGuiCol_Header] = HEADER_BG;
	colors[ImGuiCol_HeaderHovered] = HEADER_BG_HOVER;
	colors[ImGuiCol_HeaderActive] = HEADER_BG_ACTIVE;

	// Tabs
	colors[ImGuiCol_Tab] = HEADER_BG;
	colors[ImGuiCol_TabHovered] = HEADER_BG_HOVER;
	colors[ImGuiCol_TabActive] = HEADER_BG_ACTIVE;
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = HEADER_BG;


	// Separators
	colors[ImGuiCol_Separator] = ImVec4(0.26f, 0.28f, 0.32f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ACCENT_HOVER;
	colors[ImGuiCol_SeparatorActive] = ACCENT_ACTIVE;

	// Resize grips
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ACCENT_HOVER;
	colors[ImGuiCol_ResizeGripActive] = ACCENT_ACTIVE;

	// Docking
	colors[ImGuiCol_DockingPreview] = ImVec4(ACCENT.x, ACCENT.y, ACCENT.z, 0.45f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);

	// Tables
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30f, 0.33f, 0.38f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.16f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.18f, 0.19f, 0.22f, 1.00f);

	// Plots
	colors[ImGuiCol_PlotLines] = ACCENT;
	colors[ImGuiCol_PlotLinesHovered] = ACCENT_ACTIVE;
	colors[ImGuiCol_PlotHistogram] = ACCENT;
	colors[ImGuiCol_PlotHistogramHovered] = ACCENT_ACTIVE;

	// Text select
	colors[ImGuiCol_TextSelectedBg] = ImVec4(ACCENT.x, ACCENT.y, ACCENT.z, 0.45f);

	// Nav
	colors[ImGuiCol_NavHighlight] = ACCENT_ACTIVE;
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1, 1, 1, 0.7f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0, 0, 0, 0.3f);

	// Modal
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.4f);

	// Layout / metrics tuned for "AAA editor" feel
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.FramePadding = ImVec2(5.0f, 3.0f);
	style.CellPadding = ImVec2(6.0f, 3.0f);
	style.ItemSpacing = ImVec2(6.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 3.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 13.0f;
	style.GrabMinSize = 10.0f;

	style.WindowBorderSize = 1.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.FrameBorderSize = 1.0f;
	style.TabBorderSize = 0.0f;

	// Subtle rounding, not bubbly
	style.WindowRounding = 4.0f;
	style.ChildRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.PopupRounding = 3.0f;
	style.ScrollbarRounding = 3.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 3.0f;

	// Editor-style titles: left aligned
	style.WindowTitleAlign = ImVec2(0.02f, 0.5f);
	style.ColorButtonPosition = ImGuiDir_Right;
	style.SeparatorTextBorderSize = 1.0f;
	style.SeparatorTextPadding = ImVec2(12.0f, 3.0f);
}
