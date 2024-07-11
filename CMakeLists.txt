cmake_minimum_required(VERSION 3.6)

project(RoofBuilder)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp" "src/*.c")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/cpp-dotenv)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(geos REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX geos cpp_dotenv)

target_compile_definitions(
  ${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/" PYTHON_PATH="${CMAKE_CURRENT_SOURCE_DIR}/src/py/" CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/")