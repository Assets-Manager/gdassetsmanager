// License

#ifndef GDAUDIOWAVERENDERER_HPP
#define GDAUDIOWAVERENDERER_HPP

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/texture.hpp>

class GDAudioWaveRenderer : public godot::RefCounted
{
    GDCLASS(GDAudioWaveRenderer, godot::RefCounted);
    public:
        GDAudioWaveRenderer() = default;

        void _init() { }

        static void _bind_methods();

        godot::Ref<godot::Texture> RenderAudioWave(godot::String _File, godot::Vector2i _Size) const;

        ~GDAudioWaveRenderer() = default;
};

#endif