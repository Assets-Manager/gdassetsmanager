// License

#include "godot_cpp/core/memory.hpp"
#include <godot_cpp/variant/variant.hpp>
#include <GDAudioWaveRenderer.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <AudioLoaders/WavAudioLoader.hpp>
#include <AudioLoaders/Mp3AudioLoader.hpp>
#include <AudioLoaders/FlacAudioLoader.hpp>
#include <AudioLoaders/OggAudioLoader.hpp>

void GDAudioWaveRenderer::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("render_audio_wave"), &GDAudioWaveRenderer::RenderAudioWave);
}

godot::Ref<godot::Texture> GDAudioWaveRenderer::RenderAudioWave(godot::String _File, godot::Vector2 _Size) const
{
    IAudioLoader *loader;
    godot::String ext = _File.get_extension().to_lower();

    // Creates an instance of a loader.
    if(ext == "wav")
        loader = new CWavAudioLoader();
    else if(ext == "ogg")
        loader = new COggAudioLoader();
    else if(ext == "flac")
        loader = new CFlacAudioLoader();
    else if(ext == "mp3")
        loader = new CMp3AudioLoader();
    else
        return nullptr;

    // Loads the audio file.
    if(!loader->Load(_File))
        return nullptr;

    const godot::PackedRealArray &data = loader->GetPCMFloatFrames();
    float step = data.size() / _Size.x;

    // Creates the image for the thumbnail.
    godot::Ref<godot::Image> thumbnail = memnew(godot::Image);
    thumbnail->create(_Size.x, _Size.y, false, godot::Image::FORMAT_RGBA8);
    
    // Renders the thumbnail.
    for (size_t x = 0; x < _Size.x; x++)
    {
        uint32_t from = x * step;
        uint32_t to = (x + 1) * step;

        from = std::min(from, (uint32_t)data.size());
        to = std::min(to, (uint32_t)data.size());

        float min = 1, max = -1;
        for (size_t i = from; i < to; i++)
        {
            float val = data.ptr()[i];
            max = std::max(val, max);
            min = std::min(val, min);
        }

        max *= -_Size.y;
        min *= -_Size.y;

        max = (max + _Size.y) * 0.5;
        min = (min + _Size.y) * 0.5;

        for (size_t y = max; y < min; y++)
            thumbnail->set_pixel(x, y, godot::Color(0.223529, 0.560784, 0.658824));
    }

    godot::Ref<godot::ImageTexture> result = memnew(godot::ImageTexture);
    result->create_from_image(thumbnail);
    return result;
}