#include "headers/vulkan.h"

namespace gimi::vulkan {
	VkAllocationCallbacks *g_Allocator = nullptr;
	VkInstance g_Instance = VK_NULL_HANDLE;
	VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice g_Device = VK_NULL_HANDLE;
	uint32_t g_QueueFamily = (uint32_t) -1;
	VkQueue g_Queue = VK_NULL_HANDLE;
	VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

	ImGui_ImplVulkanH_Window g_MainWindowData;
	uint32_t g_MinImageCount = 2;
	bool g_SwapChainRebuild = false;

	void NewFrame(bool &done, SDL_Window *window) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
				event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Resize swap chain?
		if (g_SwapChainRebuild) {
			int width, height;
			SDL_GetWindowSize(window, &width, &height);
			if (width > 0 && height > 0) {
				ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
				ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData,
													   g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
				g_MainWindowData.FrameIndex = 0;
				g_SwapChainRebuild = false;
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data) {
		VkResult err;

		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE,
									&wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
			g_SwapChainRebuild = true;
			return;
		}
		check_vk_result(err);

		ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE,
								  UINT64_MAX);    // wait indefinitely instead of periodically checking
			check_vk_result(err);

			err = vkResetFences(g_Device, 1, &fd->Fence);
			check_vk_result(err);
		}
		{
			err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			check_vk_result(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
			check_vk_result(err);
		}
	}

	void FramePresent(ImGui_ImplVulkanH_Window *wd) {
		if (g_SwapChainRebuild)
			return;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(g_Queue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
			g_SwapChainRebuild = true;
			return;
		}
		check_vk_result(err);
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
	}

	void _check_vk_result(VkResult err) {
		check_vk_result(err);
	}

	void check_vk_result(VkResult &err) {
		if (err == 0)
			return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}

	bool IsExtensionAvailable(const ImVector<VkExtensionProperties> &properties, const char *extension) {
		for (const VkExtensionProperties &p: properties)
			if (strcmp(p.extensionName, extension) == 0)
				return true;
		return false;
	}

	VkPhysicalDevice SetupVulkan_SelectPhysicalDevice() {
		uint32_t gpu_count;
		VkResult err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
		check_vk_result(err);
		IM_ASSERT(gpu_count > 0);

		ImVector<VkPhysicalDevice> gpus;
		gpus.resize(gpu_count);
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.Data);
		check_vk_result(err);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
		// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
		// dedicated GPUs) is out of scope of this sample.
		for (VkPhysicalDevice &device: gpus) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				return device;
		}

		// Use first GPU (Integrated) is a Discrete one is not available.
		if (gpu_count > 0)
			return gpus[0];
		return VK_NULL_HANDLE;
	}

	void CreateVulkanInstance(ImVector<const char *> &instance_extensions, VkResult &err) {
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		// Enumerate available extensions
		uint32_t properties_count;
		ImVector<VkExtensionProperties> properties;
		vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
		check_vk_result(err);

		// Enable required extensions
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
			instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
			create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		}
#endif

		// Create Vulkan Instance
		create_info.enabledExtensionCount = (uint32_t) instance_extensions.Size;
		create_info.ppEnabledExtensionNames = instance_extensions.Data;
		err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
		check_vk_result(err);
	}

	void CreateDescriptorPool(VkResult &err) {
		VkDescriptorPoolSize pool_sizes[] =
				{
						{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
				};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1;
		pool_info.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
		check_vk_result(err);
	}

	VkResult CreateLogicalDevice(VkResult &err) {
		ImVector<const char *> device_extensions;
		device_extensions.push_back("VK_KHR_swapchain");

		// Enumerate physical device extension
		uint32_t properties_count;
		ImVector<VkExtensionProperties> properties;
		vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, nullptr);
		properties.resize(properties_count);
		vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
				device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

		const float queue_priority[] = {1.0f};
		VkDeviceQueueCreateInfo queue_info[1] = {};

		auto *first = &queue_info[0];
		first->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		first->queueFamilyIndex = g_QueueFamily;
		first->queueCount = 1;
		first->pQueuePriorities = queue_priority;

		VkDeviceCreateInfo create_info = {};

		auto *p_create_info = &create_info;
		p_create_info->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		p_create_info->queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		p_create_info->pQueueCreateInfos = queue_info;
		p_create_info->enabledExtensionCount = (uint32_t) device_extensions.Size;
		p_create_info->ppEnabledExtensionNames = device_extensions.Data;

		err = vkCreateDevice(g_PhysicalDevice, p_create_info, g_Allocator, &g_Device);
		check_vk_result(err);
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);

		return err;
	}

	void SelectGraphicsQueueFamily() {
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
		VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *) malloc(
				sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++)
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				g_QueueFamily = i;
				break;
			}
		free(queues);
		IM_ASSERT(g_QueueFamily != (uint32_t) -1);
	}

	SDL_Window *SetupVulkan() {
		VkResult err;
		auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL2+Vulkan example", SDL_WINDOWPOS_CENTERED,
											  SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

		ImVector<const char *> extensions;
		uint32_t extensions_count = 0;
		SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, nullptr);
		extensions.resize(extensions_count);
		SDL_Vulkan_GetInstanceExtensions(window, &extensions_count, extensions.Data);

		// Create Vulkan Instance
		CreateVulkanInstance(extensions, err);

		// Select Physical Device (GPU)
		g_PhysicalDevice = SetupVulkan_SelectPhysicalDevice();

		// Select graphics queue family
		SelectGraphicsQueueFamily();

		// Create Logical Device (with 1 queue)
		err = CreateLogicalDevice(err);

		// Create Descriptor Pool
		// The example only requires a single combined image sampler descriptor for the font image and only uses one descriptor set (for that)
		// If you wish to load e.g. additional textures you may need to alter pools sizes.
		CreateDescriptorPool(err);

		return window;
	}

	void
	SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, const VkSurfaceKHR &surface, const int &width, const int &height) {
		wd->Surface = surface;

		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
		if (res != VK_TRUE) {
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
													  VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface,
																  requestSurfaceImageFormat,
																  (size_t) IM_ARRAYSIZE(requestSurfaceImageFormat),
																  requestSurfaceColorSpace);

		// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0],
															  IM_ARRAYSIZE(present_modes));
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		IM_ASSERT(g_MinImageCount >= 2);
		ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator,
											   width, height, g_MinImageCount);
	}

	void CleanupVulkan() {
		vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
		vkDestroyDevice(g_Device, g_Allocator);
		vkDestroyInstance(g_Instance, g_Allocator);
	}

	void CleanupVulkanWindow() {
		ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, &g_MainWindowData, g_Allocator);
	}
}