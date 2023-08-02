#include "headers/hello_world.h"
#include "headers/demo.h"
#include "headers/another.h"

namespace windows::hello_world {
	State *state = new State{
			&windows::demo::state->show_demo_window,
			&windows::another::state->show_another_window,
			ImVec4(0.45f, 0.55f, 0.60f, 1.00f)};

	void draw(const float &frmerate) {
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin(
				"Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", state->show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", state->show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *) &state->clear_color); // Edit 3 floats representing a color

		if (ImGui::Button(
				"Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frmerate, frmerate);
		ImGui::End();
	}
}