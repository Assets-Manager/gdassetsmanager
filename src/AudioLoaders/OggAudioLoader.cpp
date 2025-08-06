// License

#include <AudioLoaders/OggAudioLoader.hpp>
#include <godot_cpp/classes/file_access.hpp>

#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_STDIO
#include <stb_vorbis.c>

bool COggAudioLoader::Load(const godot::String &_Path)
{
    godot::Ref<godot::FileAccess> file = godot::FileAccess::open(_Path, godot::FileAccess::READ);
    if(!file.is_valid())
        return false;

    godot::PackedByteArray data = file->get_buffer(file->get_length());

    int error;
    stb_vorbis *vorbis = stb_vorbis_open_memory(data.ptr(), data.size(), &error, nullptr);
    if(!vorbis)
        return false;

    stb_vorbis_info info = stb_vorbis_get_info(vorbis);
    m_ChannelCount = info.channels;
    m_SampleRate = info.sample_rate;

    m_PCMFloatFrames.resize(stb_vorbis_stream_length_in_samples(vorbis) * m_ChannelCount);
    stb_vorbis_get_samples_float_interleaved(vorbis, m_ChannelCount, m_PCMFloatFrames.ptrw(), m_PCMFloatFrames.size());

    stb_vorbis_close(vorbis);
    return true;
}