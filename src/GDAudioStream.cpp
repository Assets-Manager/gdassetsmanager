#include "AudioLoaders/FlacAudioLoader.hpp"
#include "AudioLoaders/Mp3AudioLoader.hpp"
#include "AudioLoaders/OggAudioLoader.hpp"
#include "AudioLoaders/WavAudioLoader.hpp"
#include "godot_cpp/core/memory.hpp"
#include <cstdint>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <GDAudioStream.hpp>

Error GDAudioStream::Load(String path)
{
    if(m_Loader)
    {
        delete m_Loader;
        m_Loader = nullptr;
    }

    auto v = path.utf8().get_data();

    if(!FileAccess::file_exists(path))
        return godot::ERR_FILE_NOT_FOUND;

    godot::String ext = path.get_extension().to_lower();

    // Creates an instance of a loader.
    if(ext == "wav")
        m_Loader = new CWavAudioLoader();
    else if(ext == "ogg")
        m_Loader = new COggAudioLoader();
    else if(ext == "flac")
        m_Loader = new CFlacAudioLoader();
    else if(ext == "mp3")
        m_Loader = new CMp3AudioLoader();
    else
        return godot::ERR_FILE_UNRECOGNIZED;

    if(!m_Loader->Load(path))
        return godot::ERR_FILE_CANT_OPEN;
    return godot::OK;
}

Ref<AudioStreamPlayback> GDAudioStream::_instantiate_playback() const
{
	Ref<GDAudioStreamPlayback> sample;
    sample.instantiate();
    sample->Stream = Ref<GDAudioStream>(this);

    return sample;
}

String GDAudioStream::_get_stream_name() const
{
    return "";
}

double GDAudioStream::_get_length() const
{
    return (double)m_Loader->GetPCMFloatFrames().size() / m_Loader->GetChannelCount() / m_Loader->GetSamplerate();
}

bool GDAudioStream::_is_monophonic() const
{
    return m_Loader->GetChannelCount() == 1;
}

Dictionary GDAudioStream::_get_tags() const
{
    return {};
}

bool GDAudioStream::_has_loop() const
{
    return false;
}

int32_t GDAudioStreamPlayback::_mix_resampled(AudioFrame *p_dst_buffer, int32_t p_frame_count)
{
    auto loader = Stream->m_Loader;
    if(loader->GetPCMFloatFrames().is_empty() || !m_Active)
    {
        for (int i = 0; i < p_frame_count; i++)
            p_dst_buffer[i] = {0, 0};
        return 0;
    }

    uint64_t totalSamples = loader->GetPCMFloatFrames().size();
    totalSamples /= loader->GetChannelCount();
    auto ptr = loader->GetPCMFloatFrames().ptr();

    int32_t todo = p_frame_count;
    while (todo > 0) 
    {
        // Check for end of samples
        if((uint64_t)m_Offset >= totalSamples)
        {
            m_Active = false;
            break;
        }

        auto ofs = m_Offset * loader->GetChannelCount();
        p_dst_buffer[p_frame_count - todo] = {
            ptr[ofs],
            ((loader->GetChannelCount() == 1) ? ptr[ofs] : ptr[ofs + 1])
        };
        m_Offset++;
        todo--;
    }

    if(todo)
    {
        auto ofs = p_frame_count - todo;
        for (int i = ofs; i < p_frame_count; i++)
            p_dst_buffer[i] = {0, 0};
        return ofs;
    }

    return p_frame_count;
}

float GDAudioStreamPlayback::_get_stream_sampling_rate() const
{
    return Stream->m_Loader->GetSamplerate();
}

void GDAudioStreamPlayback::_start(double p_from_pos)
{
    seek(p_from_pos);
    m_Active = true;
    begin_resample();
}

void GDAudioStreamPlayback::_stop()
{
    m_Active = false;
}

bool GDAudioStreamPlayback::_is_playing() const
{
    return m_Active;
}

int32_t GDAudioStreamPlayback::_get_loop_count() const
{
    return 0;
}

double GDAudioStreamPlayback::_get_playback_position() const
{
    return double(m_Offset) / _get_stream_sampling_rate();
}

void GDAudioStreamPlayback::_seek(double p_position)
{
    double max = Stream->get_length();
	if (p_position < 0)
		p_position = 0;
	else if (p_position >= max)
		p_position = max - 0.001;

	m_Offset = int64_t(p_position * _get_stream_sampling_rate());
}