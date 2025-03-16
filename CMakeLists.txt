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

set (CMAKE_CXX_FLAGS "-Wall -Wextra -fno-exceptions")


add_executable(ecs main.cpp WorldRegistry.cpp)
target_link_libraries(ecs glfw GL)
target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(run
    COMMAND ecs
    DEPENDS ecs
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
    COMMENT "Running... "
)

add_executable(testing testing.cpp)
target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(test
    COMMAND testing
    DEPENDS testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
    COMMENT "Running tests... "
)
