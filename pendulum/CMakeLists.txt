project(pendulum)
add_executable(${PROJECT_NAME} main.cpp window.cpp sphere.cpp line.cpp physics.cpp)
enable_abcg(${PROJECT_NAME})