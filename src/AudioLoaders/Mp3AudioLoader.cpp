// License

#include <AudioLoaders/Mp3AudioLoader.hpp>

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#define DR_MP3_NO_WCHAR
#include <dr_mp3.h>

bool CMp3AudioLoader::Load(const godot::String &_Path)
{
    m_File.instance();
    if(m_File->open(_Path, godot::File::READ) != godot::Error::OK)
        return false;

    drmp3 mp3;
    if(!drmp3_init(&mp3, &CDRAudioLoader::dr_read_proc, (drmp3_seek_proc)&CDRAudioLoader::dr_seek_proc, this, nullptr))
        return false;

    m_ChannelCount = mp3.channels;
    m_SampleRate = mp3.sampleRate;

    drmp3_uint64 totalPCMFrames = drmp3_get_pcm_frame_count(&mp3);

    m_PCMFloatFrames.resize(totalPCMFrames * m_ChannelCount);
    if(drmp3_read_pcm_frames_f32(&mp3, totalPCMFrames, m_PCMFloatFrames.write().ptr()) < m_PCMFloatFrames.size())
    {
        drmp3_uninit(&mp3);
        m_File->close();
        m_File.unref();
        return false;
    }

    drmp3_uninit(&mp3);
    m_File->close();
    m_File.unref();
    return true;
}