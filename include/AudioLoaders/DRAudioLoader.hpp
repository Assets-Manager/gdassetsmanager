// License

#ifndef DRAUDIOLOADER_HPP
#define DRAUDIOLOADER_HPP

#include "godot_cpp/classes/file_access.hpp"
#include <AudioLoaders/IAudioLoader.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref.hpp>

class CDRAudioLoader : public IAudioLoader
{
    public:
        CDRAudioLoader() : IAudioLoader() {}
        ~CDRAudioLoader() = default;

    protected:
        godot::Ref<godot::FileAccess> m_File;

        static size_t dr_read_proc(void* _UserData, void* _BufferOut, size_t _BytesToRead);
        static int dr_seek_proc(void* _UserData, int _Offset, int _Origin);

    private:
        size_t Read(void *_BufferOut, size_t _BytesToRead);
        int Seek(int _Offset, int _Origin);
};

#endif