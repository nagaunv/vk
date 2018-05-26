
cmake_minimum_required (VERSION 3.8)

project(rt)

# ------------------------------------------
# Enable CTest
# ------------------------------------------
include(CTest)
enable_testing()

# ------------------------------------------
# Find Vulkan (requires Vulkan SDK)
# ------------------------------------------
find_package(Vulkan REQUIRED)
set(VULKAN_INCLUDE_DIR ${Vulkan_INCLUDE_DIR})
set(VULKAN_LIBRARY ${Vulkan_LIBRARY})
include_directories(rt ${VULKAN_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

# macro to create new target
function(TARGET_FILE NAME)
add_executable(
  ${NAME}
  ${NAME}.cpp
)
target_link_libraries(
  ${NAME}
  ${VULKAN_LIBRARY}
  -static
)
add_executable(
  ${NAME}-cpp
  ${NAME}.cpp
)
target_link_libraries(
  ${NAME}-cpp
  ${VULKAN_LIBRARY}
  -static
)
target_compile_definitions(${NAME} PRIVATE UNICODE=1 VK_USE_PLATFORM_WIN32_KHR=1)
target_compile_definitions(${NAME}-cpp PRIVATE UNICODE=1 VK_USE_PLATFORM_WIN32_KHR=1)
target_compile_definitions(${NAME}-cpp PRIVATE USE_VULKAN_HPP=1)
add_test(${NAME} ${NAME} COMMAND ${NAME})
add_test(${NAME}-cpp ${NAME}-cpp COMMAND ${NAME}-cpp)
endfunction()

# ------------------------------------------
# targets
# ------------------------------------------
TARGET_FILE(vulkan_hpp)
TARGET_FILE(instance)
TARGET_FILE(enumerate_devices)
TARGET_FILE(init_command_buffer)
TARGET_FILE(init_swapchain)

