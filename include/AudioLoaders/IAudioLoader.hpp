// License

#ifndef IAUDIOLOADER_HPP
#define IAUDIOLOADER_HPP

#include <PoolArrays.hpp>
#include <String.hpp>

class IAudioLoader
{
    public:
        IAudioLoader() : m_SampleRate(0), m_ChannelCount(0) {}

        /**
         * @brief Loads a given audio file.
         * @param _Path: Path to the audio file.
         * @return Returns true on success.
         */
        virtual bool Load(const godot::String &_Path) = 0;

        /**
         * @return Returns the sample rate.
         */
        int GetSamplerate() const
        {
            return m_SampleRate;
        }

        /**
         * @return Returns the channel count.
         */
        int GetChannelCount() const
        {
            return m_ChannelCount;
        }

        /**
         * @return Returns all audio samples.
         */
        const godot::PoolRealArray &GetPCMFloatFrames() const
        {
            return m_PCMFloatFrames;
        }

    protected:
        int m_SampleRate;
        int m_ChannelCount;
        godot::PoolRealArray m_PCMFloatFrames;
};

#endif