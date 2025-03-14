cmake_minimum_required(VERSION 3.15...3.31)
project(ecs 
    VERSION 0.1
    DESCRIPTION "A ECS based mini-engine with deferred rendering"
    LANGUAGES CXX)

add_executable(ecs-testing main.cpp)
target_link_libraries(ecs-testing glfw GL)

add_custom_target(run
    COMMAND ecs-testing
    DEPENDS ecs-testing
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} 
    COMMENT "Building and running ... "
)
