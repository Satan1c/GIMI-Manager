#pragma once
#define SDL_MAIN_HANDLED

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "vulkan.h"
#include "../windows/headers/windows.h"


static bool g_SwapChainRebuild = false;

static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);

static void FramePresent(ImGui_ImplVulkanH_Window *wd);

void shutdown(VkResult &err, SDL_Window *window);

void setup_render(ImGui_ImplVulkanH_Window *wd, SDL_Window *window);

void load_fonts(ImGui_ImplVulkanH_Window *wd, VkResult &err);

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