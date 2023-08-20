// License

#ifndef FLACAUDIOLOADER_HPP
#define FLACAUDIOLOADER_HPP

#include <AudioLoaders/DRAudioLoader.hpp>

class CFlacAudioLoader : public CDRAudioLoader
{
    public:
        CFlacAudioLoader() : CDRAudioLoader() {}

        bool Load(const godot::String &_Path) override;

        ~CFlacAudioLoader() = default;
};

#endif