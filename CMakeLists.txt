cmake_minimum_required(VERSION 3.15...3.31)
project(mini-engine
    VERSION 0.1
    DESCRIPTION "A ECS based mini-engine with deferred rendering")

# choosing compiler and setting flags

set(CMAKE_CXX_STANDARD 17)
if(WIN32)
    set(CMAKE_CXX_FLAGS "/Wall /EHsc /Qspectre")
elseif(UNIX)
    find_program(CLANG_PATH NAMES clang)
    find_program(CLANGXX_PATH NAMES clang++)

    if (CLANG_PATH) 
        set(CMAKE_C_COMPILER ${CLANG_PATH})
    endif()
    if (CLANGXX_PATH) 
        set(CMAKE_CXX_COMPILER ${CLANGXX_PATH})
    endif()

    set(CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions")
endif()

###########################################

set(MINI_ENGINE_EXECUTABLE_NAME mini)

add_executable(${MINI_ENGINE_EXECUTABLE_NAME} src/main.cpp src/WorldRegistry.cpp src/Platform.cpp src/glad/src/glad.c)

target_include_directories(${MINI_ENGINE_EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/glad/include)

# Adding platform specific sources
if (WIN32)
    target_sources(${MINI_ENGINE_EXECUTABLE_NAME} PRIVATE src/PlatformWIN32.cpp)
elseif(UNIX)
    target_sources(${MINI_ENGINE_EXECUTABLE_NAME} PRIVATE src/PlatformLinux.cpp)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# declaring and locating dependencies
# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${MINI_ENGINE_EXECUTABLE_NAME} OpenGL::GL assimp)
# GLFW
if (WIN32)
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/glfw/include")
    set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2022/glfw3.lib")
    target_include_directories(${MINI_ENGINE_EXECUTABLE_NAME} PUBLIC ${GLFW_INCLUDE_DIR})
    target_link_libraries(${MINI_ENGINE_EXECUTABLE_NAME} ${GLFW_LIBRARY})
elseif(UNIX)
    target_link_libraries(${MINI_ENGINE_EXECUTABLE_NAME} glfw)
endif()

###########################################

add_custom_target(run
    COMMAND ${MINI_ENGINE_EXECUTABLE_NAME}
    DEPENDS ${MINI_ENGINE_EXECUTABLE_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
    COMMENT "Running... "
)

add_executable(testing src/testing.cpp src/WorldRegistry.cpp)
target_include_directories(testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(test
    COMMAND testing
    DEPENDS testing
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
    COMMENT "Running tests... "
)
