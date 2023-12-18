# gdassetsmanager

This library extends the Godot Engine with features like the native file dialog, runtime 3d model and audio import.

## Prerequirements

Ensure you have the following prerequisites before getting started:

- [cmake](https://cmake.org/)
- C++ compiler supporting at least C++11 (Currently tested with gcc, compatibility with LLVM and MSVC expected)
- git (optional)
- scons

### Building

Follow these steps to build the library:

1. Clone this repo <br>
`git clone --recursive https://github.com/Assets-Manager/gdassetsmanager.git`

2. Clone the godot headers <br>
```bash
git clone --recursive https://github.com/godotengine/godot-cpp third-party/godot-cpp -b 3.5 # Call inside the root directory of gdassetsmanager
cd third-party/godot-cpp
scons platform=<your platform> generate_bindings=yes
cd ../../
mkdir build
cd build
cmake ..
cmake --build .
```

Replace `<your platform>` with either `windows`, `linux`, `osx` or `android`.

For more information please visit the official [repository](https://github.com/godotengine/godot-cpp/tree/3.5).

3. Copy the libgdassetsmanager.EXTENSION to the `Native` directory of the `Assets Manager` project.

## Who is using this library?

- [Assets Manager](https://github.com/Assets-Manager/AssetsManager) on [itch.io](https://vailor1.itch.io/assets-manager)