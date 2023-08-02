#pragma once

#include <tuple>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl2.h>
#include <SDL_vulkan.h>
#include <SDL.h>
#include <vulkan/vulkan.h>

namespace gimi::vulkan {
	extern VkAllocationCallbacks *g_Allocator;
	extern VkInstance g_Instance;
	extern VkPhysicalDevice g_PhysicalDevice;
	extern VkDevice g_Device;
	extern uint32_t g_QueueFamily;
	extern VkQueue g_Queue;
	extern VkPipelineCache g_PipelineCache;
	extern VkDescriptorPool g_DescriptorPool;

	extern ImGui_ImplVulkanH_Window g_MainWindowData;
	extern uint32_t g_MinImageCount;
	extern bool g_SwapChainRebuild;

	void NewFrame(bool& done, SDL_Window *window);

	void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data);

	void FramePresent(ImGui_ImplVulkanH_Window *wd);

	void _check_vk_result(VkResult err);

	void check_vk_result(VkResult &err);

	bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension);

	VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();

	SDL_Window *SetupVulkan();

	void
	SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, const VkSurfaceKHR &surface, const int &width, const int &height);

	void CleanupVulkan();

	void CleanupVulkanWindow();
}