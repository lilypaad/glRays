#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Options
{
public:
	bool win_open = true;

	// Camera settings
	float fov = 70.0;
	float sensitivity = 1.0;
	float camera_speed = 10.0;

	Options() {}

	void render_options_window()
	{
		ImGui::Begin("Options", &win_open, ImGuiWindowFlags_MenuBar);

		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::SeparatorText("Camera Options");
		ImGui::SliderFloat("FOV", &fov, 45.0, 120.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Sensitivity", &sensitivity, 0.1, 2.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Camera speed", &camera_speed, 1.0, 50.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::End();
	}
};