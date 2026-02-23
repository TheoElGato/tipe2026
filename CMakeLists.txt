cmake_minimum_required(VERSION 3.28)

project(TIPE2026 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-profile-arcs -fno-test-coverage") # Disable coverage

option(PCSCHOOL "" OFF)

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

if(PCSCHOOL)
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

# Disable coverage for csv
set_target_properties(csv PROPERTIES 
    COMPILE_FLAGS "-fno-profile-arcs -fno-test-coverage"
    INTERFACE_COMPILE_OPTIONS ""
)

# To test for memory leaks, uncomment the line below :
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

# Common (non-graphics) sources
set(COMMON_SRCS
    src/main.cpp
    src/physics.cpp
    src/brain.cpp
    src/threadsmg.cpp
    src/reproduction.cpp
    src/filemanager.cpp
    src/client.cpp
    src/server.cpp
    src/creature.cpp
)

# Graphical-only sources
set(GRAPHICS_SRCS
    src/displayservice.cpp
)

# Headless-only sources
set(HEADLESS_SRCS
    src/displayservice_headless.cpp
)

# Graphical executable (uses SFML graphics/window)
add_executable(ursafsim
    ${COMMON_SRCS}
    ${GRAPHICS_SRCS}
)

target_compile_definitions(ursafsim PRIVATE ASIO_STANDALONE USE_SFML_GRAPHICS)
target_include_directories(ursafsim PRIVATE include ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(ursafsim PRIVATE nlohmann_json::nlohmann_json sfml-graphics sfml-window sfml-system sfml-audio csv websocketpp "${TORCH_LIBRARIES}" )

# Headless executable (NO sfml-graphics or sfml-window)
add_executable(ursafsim_headless
    ${COMMON_SRCS}
    ${HEADLESS_SRCS}
)

target_compile_definitions(ursafsim_headless PRIVATE ASIO_STANDALONE)
target_include_directories(ursafsim_headless PRIVATE include ${asio_SOURCE_DIR}/asio/include)
target_link_libraries(ursafsim_headless PRIVATE nlohmann_json::nlohmann_json sfml-system sfml-audio sfml-network gcov csv websocketpp "${TORCH_LIBRARIES}" )


