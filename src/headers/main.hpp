#pragma once
#if SDL2_VULKAN
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "../windows/headers/windows.hpp"

#if SDL2_VULKAN
#include "vulkan.h"

void update_main_window(ImGui_ImplVulkanH_Window * wd) {
	// Rendering
	ImGui::Render();
	ImDrawData *main_draw_data = ImGui::GetDrawData();
	const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
	auto clear_color = windows::hello_world::state->clear_color;
	wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
	wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
	wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
	wd->ClearValue.color.float32[3] = clear_color.w;
	if (!main_is_minimized)
		gimi::vulkan::FrameRender(wd, main_draw_data);

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	// Present Main Platform Window
	if (!main_is_minimized)
		gimi::vulkan::FramePresent(wd);
}

void setup_render(ImGui_ImplVulkanH_Window *wd, SDL_Window *window) {
	ImGui_ImplSDL2_InitForVulkan(window);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = gimi::vulkan::g_Instance;
	init_info.PhysicalDevice = gimi::vulkan::g_PhysicalDevice;
	init_info.Device = gimi::vulkan::g_Device;
	init_info.QueueFamily = gimi::vulkan::g_QueueFamily;
	init_info.Queue = gimi::vulkan::g_Queue;
	init_info.PipelineCache = gimi::vulkan::g_PipelineCache;
	init_info.DescriptorPool = gimi::vulkan::g_DescriptorPool;
	init_info.Subpass = 0;
	init_info.MinImageCount = gimi::vulkan::g_MinImageCount;
	init_info.ImageCount = wd->ImageCount;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = gimi::vulkan::g_Allocator;
	init_info.CheckVkResultFn = gimi::vulkan::_check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
}

void load_fonts(ImGui_ImplVulkanH_Window *wd, VkResult &err) {
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// Use any command queue
	VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
	VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

	err = vkResetCommandPool(gimi::vulkan::g_Device, command_pool, 0);
	gimi::vulkan::check_vk_result(err);
	VkCommandBufferBeginInfo begin_info = {};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	err = vkBeginCommandBuffer(command_buffer, &begin_info);
	gimi::vulkan::check_vk_result(err);

	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

	VkSubmitInfo end_info = {};
	end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	end_info.commandBufferCount = 1;
	end_info.pCommandBuffers = &command_buffer;
	err = vkEndCommandBuffer(command_buffer);
	gimi::vulkan::check_vk_result(err);
	err = vkQueueSubmit(gimi::vulkan::g_Queue, 1, &end_info, VK_NULL_HANDLE);
	gimi::vulkan::check_vk_result(err);

	err = vkDeviceWaitIdle(gimi::vulkan::g_Device);
	gimi::vulkan::check_vk_result(err);
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

std::tuple<int, ImGui_ImplVulkanH_Window *, SDL_Window *, ImGuiIO &, VkResult &> cfg() {
	VkResult err;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |=
			ImGuiConfigFlags_NavEnableKeyboard
			| ImGuiConfigFlags_NavEnableGamepad
			| ImGuiConfigFlags_DockingEnable
			| ImGuiConfigFlags_ViewportsEnable;

	io.ConfigDockingTransparentPayload = true;
	io.ConfigViewportsNoDecoration = true;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigDockingAlwaysTabBar = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("Error: %s\n", SDL_GetError());
		return {-1, nullptr, nullptr, io, err};
	}

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Create window with Vulkan graphics context

	auto* window = gimi::vulkan::SetupVulkan();

	// Create Window Surface
	VkSurfaceKHR surface;
	if (SDL_Vulkan_CreateSurface(window, gimi::vulkan::g_Instance, &surface) == 0) {
		printf("Failed to create Vulkan surface.\n");
		return {1, nullptr, window, io, err};
	}

	// Create Framebuffers
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	ImGui_ImplVulkanH_Window *wd = &gimi::vulkan::g_MainWindowData;
	gimi::vulkan::SetupVulkanWindow(wd, surface, w, h);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	setup_render(wd, window);

	// Upload Fonts
	load_fonts(wd, err);

	return {0, wd, window, io, err};
}

void shutdown(VkResult &err, SDL_Window *window) {
	err = vkDeviceWaitIdle(gimi::vulkan::g_Device);
	gimi::vulkan::check_vk_result(err);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	gimi::vulkan::CleanupVulkanWindow();
	gimi::vulkan::CleanupVulkan();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

#endif

#if WINDOWS_NATIVE

#include "dx11.hpp"

void update_main_window() {
	// Rendering
	ImGui::Render();
	auto clear_color = windows::hello_world::state->clear_color;
	const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w,
											 clear_color.z * clear_color.w, clear_color.w};
	gimi::dx11::g_pd3dDeviceContext->OMSetRenderTargets(1, &gimi::dx11::g_mainRenderTargetView, nullptr);
	gimi::dx11::g_pd3dDeviceContext->ClearRenderTargetView(gimi::dx11::g_mainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	gimi::dx11::g_pSwapChain->Present(1, 0);
}

void setup_render(HWND &hwnd) {
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(gimi::dx11::g_pd3dDevice, gimi::dx11::g_pd3dDeviceContext);
}

void load_fonts() {
	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);
}

std::tuple<int, ImGuiIO &, WNDCLASSEXW &, HWND &> cfg() {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |=
			ImGuiConfigFlags_NavEnableKeyboard
			| ImGuiConfigFlags_NavEnableGamepad
			| ImGuiConfigFlags_DockingEnable
			| ImGuiConfigFlags_ViewportsEnable;

	io.ConfigDockingTransparentPayload = true;
	io.ConfigViewportsNoDecoration = true;
	io.ConfigViewportsNoAutoMerge = false;
	io.ConfigDockingAlwaysTabBar = true;

	WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, gimi::dx11::WndProc, 0L, 0L,
					  GetModuleHandle(nullptr), nullptr, nullptr,
					  nullptr, nullptr, L"GIMI Manager", nullptr};
	RegisterClassExW(&wc);
	HWND hwnd = CreateWindowW(wc.lpszClassName, L"GIMI Manager", WS_OVERLAPPEDWINDOW, 100, 100,
							  480, 270,
							  nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!gimi::dx11::CreateDeviceD3D(hwnd)) {
		gimi::dx11::CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return {1, io, wc, hwnd};
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	setup_render(hwnd);

	// Upload Fonts
	load_fonts();

	return {0, io, wc, hwnd};
}

void shutdown(HWND &hwnd, WNDCLASSEXW &wc) {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	gimi::dx11::CleanupDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

#endif