// License

#ifndef MP3AUDIOLOADER_HPP
#define MP3AUDIOLOADER_HPP

#include <AudioLoaders/DRAudioLoader.hpp>

class CMp3AudioLoader : public CDRAudioLoader
{
    public:
        CMp3AudioLoader() : CDRAudioLoader() {}

        bool Load(const godot::String &_Path) override;

        ~CMp3AudioLoader() = default;
};

#endif