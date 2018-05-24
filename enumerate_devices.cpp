#if defined(USE_VULKAN_HPP)
  #include <vulkan/vulkan.hpp>
#else
  #include <vulkan/vulkan.h>
#endif

#include <iostream>

/// \file Enumerate devices

static constexpr const char* app_name = __FILE__;
static constexpr const char* engine_name = "Vulkan.hpp";

int main() {
#if defined(USE_VULKAN_HPP)
  try {
    vk::ApplicationInfo appInfo(
      app_name, 1, engine_name, 1, VK_API_VERSION_1_0);
    vk::UniqueInstance instance =
      vk::createInstanceUnique(vk::InstanceCreateInfo({}, &appInfo));
    
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();

    std::cout << "Physical Devices:" << std::endl;
    for(auto&& d :physicalDevices) {
      auto properties = d.getProperties();
      std::cout << "  [" << properties.deviceName << "]" << std::endl;
    }
  }catch(vk::SystemError& e) {
    std::cout << "vk::SystemError" << std::endl;
    exit(-1);
  }catch(...) {
    exit(-1);
    std::cout << "Unknown error" << std::endl;
  }
#else
  // TODO
#endif
}