// #define USE_VULKAN_HPP 1
// #define VK_USE_PLATFORM_WIN32_KHR 1
// #undef UNICODE

#if defined(USE_VULKAN_HPP)
  #include <vulkan/vulkan.hpp>
#else
  #include <vulkan/vulkan.h>
#endif

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>

/// \file importing Vulkan-Hpp from Vulkan SDK

static const char* app_name = __FILE__;
static const char* engine_name = "Vulkan.hpp";

[[deprecated]] std::u16string utf8_to_utf16(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
  return converter.from_bytes(str);
}

std::vector<const char*> getInstanceExtensions() {
  std::vector<const char*> extensions;
  // push surface extension
  extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  // push supported platform extensions
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
  extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
  extensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
  extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MIR_KHR)
  extensions.push_back(VK_KHR_MIR_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_VI_NN)
  extensions.push_back(VK_NN_VI_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
  extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
  extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
  extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
  extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
  extensions.push_back(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME);
#endif

  return extensions;
}

std::vector<const char*> getDeviceExtensions(){
  std::vector<const char*> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  return extensions;
}
#if defined(VK_USE_PLATFORM_WIN32_KHR)

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CLOSE:
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return (DefWindowProc(hWnd, uMsg, wParam, lParam));
  }
  return 0;
}

HWND initializeWindow(std::u16string& className, std::u16string& windowName, LONG width, LONG height){
  // create window class
  WNDCLASSEX windowClass = {};
  HINSTANCE instance = GetModuleHandle(nullptr);
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.style  = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = WindowProc;
  windowClass.hInstance = instance;
  windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  windowClass.lpszClassName = (LPCWSTR)className.c_str();
  windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

  if (!RegisterClassEx(&windowClass)) {
    throw std::runtime_error("Failed to register WNDCLASSEX");
  }

  RECT windowRect = {0, 0, width, height};
  AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

  HWND window = CreateWindowEx(
    0, (LPCWSTR)className.c_str(), (LPCWSTR)windowName.c_str(),
    WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU, 100, 100,
    windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
    nullptr, nullptr, instance, nullptr);

  if (!window)  throw std::runtime_error("Failed to create window"); 
  return window;
}

#else
static_assert(false, "Currently Windows only");
#endif

int main() {
#if defined(USE_VULKAN_HPP)
  try {
    vk::ApplicationInfo appInfo(
      app_name, 1, engine_name, 1, VK_API_VERSION_1_1);
    // get extensions
    std::vector<const char*> instanceExtensions = getInstanceExtensions();
    // create instance wiht instance extensions
    vk::InstanceCreateInfo instanceCreateInfo(
      {}, &appInfo, 0, nullptr,
      static_cast<uint32_t>(instanceExtensions.size()),
      instanceExtensions.data());
    vk::UniqueInstance instance = vk::createInstanceUnique(instanceCreateInfo);

    // get physical devices
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();
    assert(!physicalDevices.empty());

    uint32_t width = 500;
    uint32_t height = 500;
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    std::u16string windowName = u"Sample Window";
    std::u16string appName = utf8_to_utf16(app_name);
    HWND window = initializeWindow(appName,  windowName, width, height);
    vk::UniqueSurfaceKHR surface = instance->createWin32SurfaceKHRUnique(
      vk::Win32SurfaceCreateInfoKHR({}, GetModuleHandle(nullptr), window));
#else
    static_assert(false, "Currently windows only");
#endif

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      physicalDevices[0].getQueueFamilyProperties();

    size_t graphicsQueueFamilyIndex = std::distance(
      queueFamilyProperties.begin(),
      std::find_if(
        queueFamilyProperties.begin(), queueFamilyProperties.end(),
        [](auto& p) { return p.queueFlags & vk::QueueFlagBits::eGraphics; }));

    // atleast one graphics queue should be found  
    assert(graphicsQueueFamilyIndex != queueFamilyProperties.size());

    size_t presentQueueFamilyIndex =
      physicalDevices[0].getSurfaceSupportKHR(
        static_cast<uint32_t>(graphicsQueueFamilyIndex), surface.get())
        ? graphicsQueueFamilyIndex
        : queueFamilyProperties.size();

    // try to find a queue family which supports both graphics and present queue operations.
    if (presentQueueFamilyIndex == queueFamilyProperties.size()) {
      for (int i = 0; i < queueFamilyProperties.size(); ++i) {
        if (
          queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics &&
          physicalDevices[0].getSurfaceSupportKHR(
            static_cast<uint32_t>(i), surface.get())) {
          graphicsQueueFamilyIndex = i;
          presentQueueFamilyIndex = i;
        }
      }
    }

    // try to find a queue family which supports present queue operation.
    if (presentQueueFamilyIndex == queueFamilyProperties.size()) {
      for (int i = 0; i < queueFamilyProperties.size(); ++i) {
        if (physicalDevices[0].getSurfaceSupportKHR(
              static_cast<uint32_t>(i), surface.get()))
          presentQueueFamilyIndex = i;
      }
    }
    // could not find present queue
    if(presentQueueFamilyIndex == queueFamilyProperties.size())
      throw std::runtime_error("Present queue is not supported on this device");

    float queuePriority = 0.f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
      {}, static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &queuePriority);

    std::vector<const char*> deviceExtensionNames = getDeviceExtensions();

    vk::UniqueDevice device =
      physicalDevices[0].createDeviceUnique(vk::DeviceCreateInfo(
        {}, 1, &deviceQueueCreateInfo, 0, nullptr,
        static_cast<uint32_t>(deviceExtensionNames.size()),
        deviceExtensionNames.data()));

    std::vector<vk::SurfaceFormatKHR> formats =
      physicalDevices[0].getSurfaceFormatsKHR(surface.get());

    assert(!formats.empty());

    vk::Format format = (formats[0].format == vk::Format::eUndefined)
                          ? vk::Format::eB8G8R8A8Unorm
                          : formats[0].format;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities =
      physicalDevices[0].getSurfaceCapabilitiesKHR(surface.get());

    VkExtent2D swapchainExtent;
    if (
      surfaceCapabilities.currentExtent.width ==
      std::numeric_limits<uint32_t>::max()) {
      std::clamp(
        width, surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width);
      std::clamp(
        height, surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height);
    } else {
      swapchainExtent = surfaceCapabilities.currentExtent;
    }

    vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eFifo;

    vk::SurfaceTransformFlagBitsKHR preTransform =
      (surfaceCapabilities.supportedTransforms &
       vk::SurfaceTransformFlagBitsKHR::eIdentity)
        ? vk::SurfaceTransformFlagBitsKHR::eIdentity
        : surfaceCapabilities.currentTransform;

    vk::CompositeAlphaFlagBitsKHR compositeAlpha =
      (surfaceCapabilities.supportedCompositeAlpha &
       vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
        ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
        : (surfaceCapabilities.supportedCompositeAlpha &
           vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
            ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
            : (surfaceCapabilities.supportedCompositeAlpha &
               vk::CompositeAlphaFlagBitsKHR::eInherit)
                ? vk::CompositeAlphaFlagBitsKHR::eInherit
                : vk::CompositeAlphaFlagBitsKHR::eOpaque;

    vk::SwapchainCreateInfoKHR swapChainCreateInfo(
      {}, surface.get(), surfaceCapabilities.minImageCount, format,
      vk::ColorSpaceKHR::eSrgbNonlinear, swapchainExtent, 1,
      vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0,
      nullptr, preTransform, compositeAlpha, swapchainPresentMode, true,
      nullptr);

    uint32_t queueFamilyIndicies[2] = {
      static_cast<uint32_t>(graphicsQueueFamilyIndex),
      static_cast<uint32_t>(presentQueueFamilyIndex)};

    if(graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
      swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
      swapChainCreateInfo.queueFamilyIndexCount = 2;
      swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndicies;
    }

    vk::UniqueSwapchainKHR swapChain =
      device->createSwapchainKHRUnique(swapChainCreateInfo);
    std::vector<vk::Image> swapChainImages =
      device->getSwapchainImagesKHR(swapChain.get());

    std::vector<vk::UniqueImageView> imageViews;
    imageViews.reserve(swapChainImages.size());
    for(auto&& image : swapChainImages){
      vk::ComponentMapping componentMapping(
        vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
        vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA);
      vk::ImageSubresourceRange subResourceRange(
        vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
      vk::ImageViewCreateInfo imageViewCreateInfo(
        vk::ImageViewCreateFlags(), image, vk::ImageViewType::e2D, format,
        componentMapping, subResourceRange);
      imageViews.push_back(device->createImageViewUnique(imageViewCreateInfo));
    }
#if defined(VK_USE_PLATFORM_WIN32_KHR)
  DestroyWindow(window);
#else
  static_assert(false);
#endif

  } catch (vk::SystemError& e) {
    std::cout << "vk::SystemError:" << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown error" << std::endl;
  }
#else
  // TODO
#endif
}