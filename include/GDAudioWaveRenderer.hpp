// License

#ifndef GDAUDIOWAVERENDERER_HPP
#define GDAUDIOWAVERENDERER_HPP

#include <Godot.hpp>
#include <Reference.hpp>
#include <Texture.hpp>

class GDAudioWaveRenderer : public godot::Reference
{
    GODOT_CLASS(GDAudioWaveRenderer, godot::Reference);
    public:
        GDAudioWaveRenderer() = default;

        void _init() { }

        static void _register_methods();

        godot::Ref<godot::Texture> RenderAudioWave(godot::String _File, godot::Vector2 _Size) const;

        ~GDAudioWaveRenderer() = default;
};

#endif