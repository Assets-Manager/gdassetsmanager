// License

#include <AudioLoaders/WavAudioLoader.hpp>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#define DR_WAV_NO_WCHAR
#include <dr_wav.h>

bool CWavAudioLoader::Load(const godot::String &_Path)
{
    m_File = godot::FileAccess::open(_Path, godot::FileAccess::READ);
    if(!m_File.is_valid())
        return false;

    drwav wav;
    if(!drwav_init(&wav, &CDRAudioLoader::dr_read_proc, (drwav_seek_proc)&CDRAudioLoader::dr_seek_proc, this, nullptr))
        return false;

    m_ChannelCount = wav.channels;
    m_SampleRate = wav.sampleRate;

    m_PCMFloatFrames.resize(wav.totalPCMFrameCount * m_ChannelCount);
    if(drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, m_PCMFloatFrames.ptrw()) < wav.totalPCMFrameCount)
    {
        drwav_uninit(&wav);
        m_File->close();
        m_File.unref();
        return false;
    }

    drwav_uninit(&wav);
    m_File->close();
    m_File.unref();
    return true;
}