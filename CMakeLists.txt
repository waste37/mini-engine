cmake_minimum_required(VERSION 3.15...3.31)
project(ecs 
    VERSION 0.1
    DESCRIPTION "A ECS based mini-engine with deferred rendering"
    LANGUAGES CXX)

find_program(CLANG_PATH NAMES clang)
find_program(CLANGXX_PATH NAMES clang++)
if (CLANG_PATH) 
    set(CMAKE_C_COMPILER ${CLANG_PATH})
endif()
if (CLANGXX_PATH) 
    set(CMAKE_CXX_COMPILER ${CLANGXX_PATH})
endif()

if(WIN32)
    set (CMAKE_CXX_FLAGS "/Wall /EHsc /Qspectre")
else()
    set (CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions")
endif()

find_package(OpenGL REQUIRED)
add_executable(ecs src/main.cpp src/WorldRegistry.cpp)

if (WIN32)
    set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/glfw/include")
    set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2022/glfw3.lib")
    include_directories(${GLFW_INCLUDE_DIR})
    target_link_libraries(ecs ${GLFW_LIBRARY})
else()
    target_link_libraries(ecs glfw)
endif()

target_link_libraries(ecs OpenGL::GL)

target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(ecs PRIVATE cxx_std_17)

add_custom_target(run
    COMMAND ecs
    DEPENDS ecs
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
    COMMENT "Running... "
)

add_executable(testing src/testing.cpp src/WorldRegistry.cpp)
target_include_directories(testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(testing PRIVATE cxx_std_17)
add_custom_target(test
    COMMAND testing
    DEPENDS testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
    COMMENT "Running tests... "
)
