cmake_minimum_required(VERSION 3.28)

project(TIPE2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PcLycee FALSE)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

if(PcLycee)
    set(SFML_DIR "/nfs/home/rbenichou/Documents/TIPE/SFML-2.5.1/lib/cmake/SFML/")
    set(SFML_FIND_COMPONENTS system window graphics audio network)
    find_package(SFML 2 REQUIRED COMPONENTS graphics window system audio network)
else()
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.5.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()


find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(main
    src/main.cpp
    src/physics.cpp
    src/creature.cpp
    src/brain.cpp
    src/threadsmg.cpp
    src/reproduction.cpp
    src/filemanager.cpp
)

target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json sfml-graphics "${TORCH_LIBRARIES}" )

# The following code block is suggested to be used on Windows.
# According to https://github.com/pytorch/pytorch/issues/25457,
# the DLLs need to be copied to avoid memory errors.
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET example-app
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:example-app>)
endif (MSVC)

