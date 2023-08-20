cmake_minimum_required(VERSION 3.3.0)
project(gdassimp VERSION 0.1.0 LANGUAGES CXX C)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_FBX_IMPORTER TRUE)
set(ASSIMP_BUILD_OBJ_IMPORTER TRUE)
set(ASSIMP_BUILD_GLTF_IMPORTER TRUE)

# ASSIMP_BUILD_${name}_EXPORTER
add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/assimp assimp)

set(SRCS "${PROJECT_SOURCE_DIR}/main.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAssimpLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAssimpIOStream.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDNativeFileDialog.cpp"
         "${PROJECT_SOURCE_DIR}/src/GDAudioWaveRenderer.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/DRAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/WavAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/Mp3AudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/FlacAudioLoader.cpp"
         "${PROJECT_SOURCE_DIR}/src/AudioLoaders/OggAudioLoader.cpp")
set(LIBS "")

if(WIN32)
    set(LIBS ${LIBS} "godot-cpp.windows.release.64")
else(WIND32)
    set(LIBS ${LIBS} "godot-cpp.linux.release.64")
endif(WIN32)


include_directories("${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include/core"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/include/gen"
                    "${PROJECT_SOURCE_DIR}/third-party/godot-cpp/godot-headers"
                    "${PROJECT_SOURCE_DIR}/third-party/assimp/include"
                    "${PROJECT_SOURCE_DIR}/third-party/portable-file-dialogs"
                    "${PROJECT_SOURCE_DIR}/third-party/dr_libs"
                    "${PROJECT_SOURCE_DIR}/third-party/stb"
                    "${PROJECT_SOURCE_DIR}/include")

link_directories("${PROJECT_SOURCE_DIR}/third-party/godot-cpp/bin")


add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} ${LIBS} assimp)