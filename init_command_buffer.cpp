// #define USE_VULKAN_HPP 1

#if defined(USE_VULKAN_HPP)
  #include <vulkan/vulkan.hpp>
#else
  #include <vulkan/vulkan.h>
#endif

#include <iostream>

/// \file importing Vulkan-Hpp from Vulkan SDK

static constexpr const char* app_name = __FILE__;
static constexpr const char* engine_name = "Vulkan.hpp";

int main() {
#if defined(USE_VULKAN_HPP)
  try {
    vk::ApplicationInfo appInfo(
      app_name, 1, engine_name, 1, VK_API_VERSION_1_1);

    vk::UniqueInstance instance =
      vk::createInstanceUnique(vk::InstanceCreateInfo({}, &appInfo));

    std::vector<vk::PhysicalDevice> physicalDevices =
      instance->enumeratePhysicalDevices();

    assert(!physicalDevices.empty());

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      physicalDevices[0].getQueueFamilyProperties();

    size_t graphicsQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(
        queueFamilyProperties.begin(), queueFamilyProperties.end(),
        [](auto& p) { return p.queueFlags & vk::QueueFlagBits::eGraphics; }));

    float queuePriority = 0.f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
      {}, static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority);

    vk::UniqueDevice device = physicalDevices[0].createDeviceUnique(
      vk::DeviceCreateInfo({}, 1, &deviceQueueCreateInfo));

    // create command pool
    // queue family is graphics
    vk::UniqueCommandPool commandPool = device->createCommandPoolUnique(
      vk::CommandPoolCreateInfo({}, deviceQueueCreateInfo.queueFamilyIndex));

    // allocate command buffer
    std::vector<vk::UniqueCommandBuffer> commandBuffers =
      device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(
        commandPool.get(), vk::CommandBufferLevel::ePrimary, 1));

  } catch (vk::SystemError& e) {
    std::cout << "vk::SystemError" << std::endl;
    exit(-1);
  } catch (...) {
    std::cout << "Unknown error" << std::endl;
    exit(-1);
  }

#else
  // TODO
#endif
}