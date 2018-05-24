#if defined(USE_VULKAN_HPP)
  #include <vulkan/vulkan.hpp>
#else
  #include <vulkan/vulkan.h>
#endif

#include <iostream>
#include <algorithm>

/// \file initialize devices

static constexpr const char* app_name = __FILE__;
static constexpr const char* engine_name = "Vulkan.hpp";

int main() {
#if defined(USE_VULKAN_HPP)
  // vulkan.hpp
  try {
    vk::ApplicationInfo appInfo(
      app_name, 1, engine_name, 1, VK_API_VERSION_1_1);
    vk::UniqueInstance instance =
      vk::createInstanceUnique(vk::InstanceCreateInfo({}, &appInfo));
    std::vector<vk::PhysicalDevice> physicalDevices =
      instance->enumeratePhysicalDevices();
    assert(!physicalDevices.empty());

    // get queue family properties
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      physicalDevices[0].getQueueFamilyProperties();

    // get first index of graphics queue
    size_t graphicsQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(
        queueFamilyProperties.begin(), queueFamilyProperties.end(),
        [](auto& p) { return p.queueFlags & vk::QueueFlagBits::eGraphics; }));

    // set QueueCreateInfo
    float queuePriority = 0.f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
      vk::DeviceQueueCreateFlags(),
      static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority);

    // create logical device
    vk::UniqueDevice device = physicalDevices[0].createDeviceUnique(
      vk::DeviceCreateInfo(vk::DeviceCreateFlags(), 1, &deviceQueueCreateInfo));

  }catch(vk::SystemError& e) {
    std::cout << "vk::SystemError" << std::endl;
    exit(-1);
  } catch(...) {
    std::cout << "Unknown error" << std::endl;
    exit(-1);
  }
#else
  // TODO
#endif
}