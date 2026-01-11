#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Options
{
public:
	// References
	Camera& cam;

	bool options_win_open = true;
	bool other_win_open = true;

	// Camera settings
	float camera_sensitivity = 1.0;
	float camera_speed = 10.0;
	float camera_fov = 70.0;
	bool camera_fov_changed = false;

	// Ray training settings
	int rt_rays_per_pixel = 1;
	int rt_max_bounces = 16;

	Options(Camera& camera) : cam(camera) {}

	void render_options_window(float delta_time)
	{
		ImGui::Begin("Options", &options_win_open, ImGuiWindowFlags_MenuBar);
		ImGui::PushItemWidth(160);

		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Camera settings
		ImGui::SeparatorText("Camera");
		ImGui::Text("Position: (%.2f, %.2f, %.2f)", cam.get_position().x, cam.get_position().y, cam.get_position().z);
		ImGui::Text("Direction: (%.2f, %.2f, %.2f)", cam.get_front().x, cam.get_front().y, cam.get_front().z);
		ImGui::Text("Pitch: %.2f\tYaw: %.2f", cam.get_pitch(), cam.get_yaw());
		if (ImGui::SliderFloat("FOV", &camera_fov, 20.0, 170.0, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
			camera_fov_changed = true;
			cam.set_fov(camera_fov);
		}
		ImGui::SliderFloat("Sensitivity", &camera_sensitivity, 0.1, 2.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Camera speed", &camera_speed, 1.0, 50.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		// Ray tracer settings
		ImGui::SeparatorText("Ray tracing");
		if (ImGui::SliderInt("Ray bounce limit", &rt_max_bounces, 0, 32, "%d", ImGuiSliderFlags_AlwaysClamp)) {
			cam.need_refresh();
		}
		if (ImGui::SliderInt("Rays per pixel", &rt_rays_per_pixel, 1, 16, "%d", ImGuiSliderFlags_AlwaysClamp)) {
			cam.need_refresh();
		}
		

		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGui::Begin("Metrics window", &other_win_open, ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("Frame time: %.4fms", delta_time * 1000);
		ImGui::Text("FPS: %.1f", 1.0 / delta_time);
		ImGui::End();
	}
};