/*
 * MIT License
 *
 * Copyright (c) 2021 Christian Tost
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <GDAssimpLoader.hpp>
#include <GDAudioWaveRenderer.hpp>
#include <GDFilewatcher.hpp>
#include <GDError.hpp>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_assets_manager_module(ModuleInitializationLevel p_level) 
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;

	GDREGISTER_RUNTIME_CLASS(GDError);
    GDREGISTER_RUNTIME_CLASS(GDFilewatcher);
    GDREGISTER_RUNTIME_CLASS(GDAudioWaveRenderer);
    GDREGISTER_RUNTIME_CLASS(GDAssimpLoader);
}

void uninitialize_assets_manager_module(ModuleInitializationLevel p_level) 
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
		return;
}

extern "C" 
{
    // Initialization.
    GDExtensionBool GDE_EXPORT assets_manager_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) 
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_assets_manager_module);
        init_obj.register_terminator(uninitialize_assets_manager_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}

// #ifdef __cplusplus
// extern "C" {
// #endif

// void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
//     godot::Godot::gdnative_init(o);
// }

// void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
//     godot::Godot::gdnative_terminate(o);
// }

// void GDN_EXPORT godot_nativescript_init(void *handle) {
//     godot::Godot::nativescript_init(handle);

//     godot::register_class<GDAssimpLoader>();
//     godot::register_class<GDNativeFileDialog>();
//     godot::register_class<GDAudioWaveRenderer>();
//     godot::register_class<GDFilewatcher>();
//     godot::register_class<GDError>();
// }

// #ifdef __cplusplus
// }
// #endif