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
	float camera_pos_x = 0.0f;
	float camera_pos_y = 0.0f;
	float camera_pos_z = 0.0f;
	float camera_pitch = 0.0f;
	float camera_yaw = 0.0f;
	bool camera_fov_changed = false;
	bool camera_moved = false;

	// Ray training settings
	int rt_rays_per_pixel = 1;
	int rt_max_bounces = 4;

	Options(Camera& camera) : cam(camera) {}

	void render_options_window(float delta_time)
	{
		// Fetch current camera settings
		camera_pos_x = cam.get_position().x;
		camera_pos_y = cam.get_position().y;
		camera_pos_z = cam.get_position().z;
		camera_pitch = cam.get_pitch();
		camera_yaw = cam.get_yaw();

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
		ImGui::SeparatorText("Camera settings");

		ImGui::PushItemWidth(70);
		if (ImGui::DragFloat("X", &camera_pos_x, 0.1f, -50.0f, 50.0f, "%.2f"))
			camera_moved = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("Y", &camera_pos_y, 0.1f, -50.0f, 50.0f, "%.2f"))
			camera_moved = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("Z", &camera_pos_z, 0.1f, -50.0f, 50.0f, "%.2f"))
			camera_moved = true;
		ImGui::PopItemWidth();

		ImGui::PushItemWidth(70);
		if (ImGui::DragFloat("Pitch", &camera_pitch, 0.2f, -50.0f, 50.0f, "%.1f"))
			camera_moved = true;
		ImGui::SameLine();
		if (ImGui::DragFloat("Yaw", &camera_yaw, 0.2f, -50.0f, 50.0f, "%.1f"))
			camera_moved = true;
		ImGui::PopItemWidth();

		if (ImGui::SliderFloat("FOV", &camera_fov, 20.0, 170.0, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
			camera_fov_changed = true;
			cam.set_fov(camera_fov);
		}
		ImGui::SliderFloat("Sensitivity", &camera_sensitivity, 0.1, 2.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Camera speed", &camera_speed, 1.0, 50.0, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		// Ray tracer settings
		ImGui::SeparatorText("Ray tracer settings");
		if (ImGui::SliderInt("Bounce limit", &rt_max_bounces, 0, 16, "%d", ImGuiSliderFlags_AlwaysClamp)) {
			cam.need_refresh();
		}
		if (ImGui::SliderInt("Samples/pixel", &rt_rays_per_pixel, 1, 8, "%d", ImGuiSliderFlags_AlwaysClamp)) {
			cam.need_refresh();
		}

		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGui::Begin("Metrics window", &other_win_open, ImGuiWindowFlags_NoTitleBar);
		ImGui::Text("Frame time: %.4fms", delta_time * 1000);
		ImGui::Text("FPS: %.1f", 1.0 / delta_time);
		ImGui::End();

		if (camera_moved) {
			cam.set_position(glm::vec3(camera_pos_x, camera_pos_y, camera_pos_z));
			cam.set_pitch(camera_pitch);
			cam.set_yaw(camera_yaw);
			cam.need_refresh();
			cam.update_vectors();
			camera_moved = false;
		}
	}
};