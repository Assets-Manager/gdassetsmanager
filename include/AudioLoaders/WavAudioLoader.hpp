// License

#ifndef WAVAUDIOLOADER_HPP
#define WAVAUDIOLOADER_HPP

#include <AudioLoaders/DRAudioLoader.hpp>

class CWavAudioLoader : public CDRAudioLoader
{
    public:
        CWavAudioLoader() : CDRAudioLoader() {}

        bool Load(const godot::String &_Path) override;

        ~CWavAudioLoader() = default;
};

#endif