#pragma once

#include <tuple>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_sdl2.h>
#include <SDL_vulkan.h>
#include <SDL.h>
#include <vulkan/vulkan.h>

namespace gimi::vulkan {
	VkAllocationCallbacks *g_Allocator = nullptr;
	VkInstance g_Instance = VK_NULL_HANDLE;
	VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice g_Device = VK_NULL_HANDLE;
	uint32_t g_QueueFamily = (uint32_t) -1;
	VkQueue g_Queue = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

	ImGui_ImplVulkanH_Window g_MainWindowData;
	uint32_t g_MinImageCount = 2;

	void NewFrame();

	void _check_vk_result(VkResult err);

	void check_vk_result(VkResult &err);

	bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension);

	VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();

	void SetupVulkan(ImVector<const char *> &instance_extensions);

	void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, const VkSurfaceKHR &surface, const int &width, const int &height);

	void CleanupVulkan();

	void CleanupVulkanWindow();

	void shutdown(VkResult &err, SDL_Window *window);

	void setup_render(ImGui_ImplVulkanH_Window *wd, SDL_Window *window);
}