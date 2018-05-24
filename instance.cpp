#if defined(USE_VULKAN_HPP)
  #include <vulkan/vulkan.hpp>
#else
  #include <vulkan/vulkan.h>
#endif

#include <iostream>

/// \file Create Vulkan instance

static constexpr const char* app_name = __FILE__;
static constexpr const char* engine_name = "Vulkan.hpp";

int main() {
#if defined(USE_VULKAN_HPP)
  try {
    // Application Info
    vk::ApplicationInfo appInfo(
      app_name, 1, engine_name, 1, VK_API_VERSION_1_1);
    // Instance
    vk::UniqueInstance instance = vk::createInstanceUnique(
      vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo));
  } catch (vk::SystemError& err) {
    std::cout << "vk::SystemError" << std::endl;
    exit(-1);
  } catch (...) {
    std::cout << "Unknown Error" << std::endl;
    exit(-1);
  }
#else
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pNext = nullptr;
  app_info.pApplicationName = app_name;
  app_info.applicationVersion = 1;
  app_info.pEngineName = engine_name;
  app_info.engineVersion = 1;
  app_info.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo inst_info = {};
  inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  inst_info.pNext = nullptr;
  inst_info.flags = 0;
  inst_info.pApplicationInfo = &app_info;
  inst_info.enabledExtensionCount = 0;
  inst_info.ppEnabledExtensionNames = nullptr;
  inst_info.enabledLayerCount = 0;
  inst_info.ppEnabledLayerNames = nullptr;

  VkInstance inst;
  VkResult res = vkCreateInstance(&inst_info, nullptr, &inst);

  if(res == VK_ERROR_INCOMPATIBLE_DRIVER) {
    std::cout << "Incompatible driver" << std::endl;
    exit(-1);
  } else if(res){
    std::cout << "Unknown error" << std::endl;
    exit(-1);
  }
  vkDestroyInstance(inst, nullptr);
#endif
}