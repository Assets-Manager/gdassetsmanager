// License

#include <AudioLoaders/FlacAudioLoader.hpp>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_WCHAR
#include <dr_flac.h>

bool CFlacAudioLoader::Load(const godot::String &_Path)
{
    m_File.instance();
    if(m_File->open(_Path, godot::File::READ) != godot::Error::OK)
        return false;

    drflac *flac = drflac_open(&CDRAudioLoader::dr_read_proc, (drflac_seek_proc)&CDRAudioLoader::dr_seek_proc, this, nullptr);
    if(!flac)
        return false;

    m_ChannelCount = flac->channels;
    m_SampleRate = flac->sampleRate;

    m_PCMFloatFrames.resize(flac->totalPCMFrameCount * m_ChannelCount);
    if(drflac_read_pcm_frames_f32(flac, flac->totalPCMFrameCount, m_PCMFloatFrames.write().ptr()) < m_PCMFloatFrames.size())
    {
        drflac_close(flac);
        m_File->close();
        m_File.unref();
        return false;
    }

    drflac_close(flac);
    m_File->close();
    m_File.unref();
    return true;
}