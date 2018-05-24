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
  std::cout << "Hello, Vulkan!" << std::endl;
  // vulkan.hpp
#else
  // vulkan.h
  std::cout << "Hello, Vulkan!" << std::endl;
#endif
}