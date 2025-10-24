cmake_minimum_required(VERSION 3.28)

project(TIPE2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PcLycee FALSE)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  csv  
  GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
  GIT_SHALLOW TRUE 
  GIT_TAG 2.3.0
)
FetchContent_MakeAvailable(csv)

FetchContent_Declare(
  websocketpp
  GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
  GIT_SHALLOW TRUE 
  GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)
add_library(websocketpp INTERFACE)
target_include_directories(websocketpp INTERFACE ${websocketpp_SOURCE_DIR})

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-28-0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(asio)

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

# To test for memory leaks, uncomment the line below :
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_executable(main
    src/main.cpp
    src/physics.cpp
    src/creature.cpp
    src/brain.cpp
    src/threadsmg.cpp
    src/reproduction.cpp
    src/filemanager.cpp
    src/client.cpp
    src/server.cpp
)

target_compile_definitions(main PRIVATE ASIO_STANDALONE)
target_include_directories(main PRIVATE include ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json sfml-graphics csv websocketpp "${TORCH_LIBRARIES}" )

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

