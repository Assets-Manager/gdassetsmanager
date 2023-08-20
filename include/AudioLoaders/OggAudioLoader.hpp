// License

#ifndef OGGAUDIOLOADER_HPP
#define OGGAUDIOLOADER_HPP

#include <AudioLoaders/IAudioLoader.hpp>

class COggAudioLoader : public IAudioLoader
{
    public:
        COggAudioLoader() : IAudioLoader() {}

        bool Load(const godot::String &_Path) override;

        ~COggAudioLoader() = default;
};

#endif