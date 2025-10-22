cmake_minimum_required(VERSION 3.3.0)
project(gdassetsmanager VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GODOTCPP_TARGET "template_release")

# For now godot needs to be linked dynamicall since heavy use of the STL.
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(GODOTCPP_USE_STATIC_CPP OFF)
endif()

OPTION(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT "" FALSE)
OPTION(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT "" FALSE)
OPTION(BUILD_SHARED_LIBS "" OFF)
OPTION(ASSIMP_BUILD_ZLIB "" ON)
OPTION(ASSIMP_INSTALL "" OFF)
OPTION(ASSIMP_INSTALL_PDB "" OFF)
OPTION(ASSIMP_BUILD_ASSIMP_VIEW "" OFF)
OPTION(ASSIMP_INJECT_DEBUG_POSTFIX "" OFF)
OPTION(ASSIMP_BUILD_TESTS "" OFF)
OPTION(ASSIMP_WARNINGS_AS_ERRORS "" OFF)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
set(ASSIMP_BUILD_GLTF_IMPORTER TRUE)
set(GODOTCPP_DISABLE_EXCEPTIONS OFF)

# ASSIMP_BUILD_${name}_EXPORTER
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/assimp assimp)
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/godot-cpp godot-cpp)

set(SRCS "${PROJECT_SOURCE_DIR}/main.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAssimpLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAssimpIOStream.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAudioWaveRenderer.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDFilewatcher.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/DRAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/WavAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/Mp3AudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/FlacAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/OggAudioLoader.cpp"
         )
set(LIBS "")

# if(WIN32)
#     set(LIBS ${LIBS} "godot-cpp.windows.release.64")
# else(WIND32)
#     set(LIBS ${LIBS} "godot-cpp.linux.release.64")
# endif(WIN32)


include_directories("${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include/core"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include/gen"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/godot-headers"
                    "${PROJECT_SOURCE_DIR}/third-party/assimp/include"
                    "${PROJECT_SOURCE_DIR}/third-party/dr_libs"
                    "${PROJECT_SOURCE_DIR}/third-party/stb"
                    "${PROJECT_SOURCE_DIR}/third-party/filewatcher"
                    "${PROJECT_SOURCE_DIR}/include")

# link_directories("${PROJECT_SOURCE_DIR}/third-party/godot-cpp/bin")

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} ${LIBS} assimp godot-cpp)