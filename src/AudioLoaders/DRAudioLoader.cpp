// License

#include <AudioLoaders/DRAudioLoader.hpp>

size_t CDRAudioLoader::dr_read_proc(void* _UserData, void* _BufferOut, size_t _BytesToRead)
{
    CDRAudioLoader *THIS = (CDRAudioLoader*)_UserData;
    return THIS->Read(_BufferOut, _BytesToRead);
}

int CDRAudioLoader::dr_seek_proc(void* _UserData, int _Offset, int _Origin)
{
    CDRAudioLoader *THIS = (CDRAudioLoader*)_UserData;
    return THIS->Seek(_Offset, _Origin);
}

size_t CDRAudioLoader::Read(void *_BufferOut, size_t _BytesToRead)
{
    if(m_File.is_null() || (m_File->get_position() + _BytesToRead > m_File->get_len()))
        return 0;

    godot::PoolByteArray data = m_File->get_buffer(_BytesToRead);
    memcpy(_BufferOut, data.read().ptr(), data.size());
    return data.size();
}

int CDRAudioLoader::Seek(int _Offset, int _Origin)
{
    if(m_File.is_null())
        return 0;

    switch (_Origin)
    {
        case 0: m_File->seek(_Offset); break;
        case 1: m_File->seek(m_File->get_position() + _Offset); break;
    }

    return 1;
}