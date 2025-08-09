// License

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <godot_cpp/variant/variant.hpp>
#include <GDAudioWaveRenderer.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <AudioLoaders/WavAudioLoader.hpp>
#include <AudioLoaders/Mp3AudioLoader.hpp>
#include <AudioLoaders/FlacAudioLoader.hpp>
#include <AudioLoaders/OggAudioLoader.hpp>

void line(godot::Ref<godot::Image> image, godot::Vector2i p0, godot::Vector2i p1)
{
    int dx =  abs(p1.x - p0.x), sx = p0.x < p1.x ? 1 : -1;
    int dy = -abs(p1.y - p0.y), sy = p0.y < p1.y ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    while (1) 
    {
        image->set_pixelv(p0, godot::Color(0.223529, 0.560784, 0.658824));
        if (p0.x == p1.x && p0.y == p1.y) 
            break;

        e2 = 2 * err;
        if (e2 > dy) 
        { 
            err += dy; 
            p0.x += sx; 
        }

        if (e2 < dx) 
        { 
            err += dx; 
            p0.y += sy; 
        }
    }
}

void GDAudioWaveRenderer::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("render_audio_wave"), &GDAudioWaveRenderer::RenderAudioWave);
}

godot::Ref<godot::Texture> GDAudioWaveRenderer::RenderAudioWave(godot::String _File, godot::Vector2i _Size) const
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
    auto sampleCount = data.size() / loader->GetChannelCount();
    uint32_t step = sampleCount / _Size.x;

    // Creates the image for the thumbnail.
    godot::Ref<godot::Image> thumbnail = godot::Image::create(_Size.x, _Size.y, false, godot::Image::FORMAT_RGBA8);
    
    godot::PackedRealArray avgSamples;
    double maxSample = 0;
    for (size_t x = 0; x < _Size.x; x++)
    {
        double sumSamples = 0;
        auto count = 0;
        for (uint32_t sample = 0; sample < step; sample++) 
        {
            auto index = x * step + sample;
            if(index < sampleCount)
            {
                sumSamples += data.ptr()[index];
                count++;
            }
            else
                break;
        }
        auto sample = sumSamples / std::max(1, count);
        avgSamples.append(sample);
        maxSample = std::max(std::fabs(sample), maxSample);
    }

    auto halfSize = _Size.y * 0.5f;

    int prevY = 0;

    // Renders the thumbnail.
    for (size_t x = 0; x < _Size.x; x++)
    {
        double norm = avgSamples[x] / maxSample;
        int y = halfSize - static_cast<int>(norm * halfSize);
        if(y >= _Size.y)
            y = _Size.y - 1;
        else if(y < 0)
            y = 0;

        godot::Vector2i p0(x, y), p1(x, y);
        if(x > 0)
        {
            p0.x = x - 1;
            p0.y = prevY;
        }
        prevY = y;
        line(thumbnail, p0, p1);
    }

    godot::Ref<godot::ImageTexture> result = godot::ImageTexture::create_from_image(thumbnail);
    return result;
}