// License

#include <GDAssimpIOStream.hpp>
#include <string.h>

GDAssimpIOStream::GDAssimpIOStream(const char* _File, const std::string& _Mode) : Assimp::IOStream() 
{
    godot::FileAccess::ModeFlags openMode;

    if(_Mode == "wb" || _Mode == "w" || _Mode == "wt")
        openMode = godot::FileAccess::WRITE;
    else
        openMode = godot::FileAccess::READ;

    m_File = godot::FileAccess::open(_File, openMode);
}

size_t GDAssimpIOStream::Read(void* pvBuffer, size_t pSize, size_t pCount)
{
    if(m_File.is_null() || (Tell() + (pSize * pCount) > FileSize()))
        return 0;

    godot::PackedByteArray buffer = m_File->get_buffer(pSize * pCount);
    memcpy(pvBuffer, buffer.ptr(), buffer.size());
    return buffer.size() / pSize;
}

size_t GDAssimpIOStream::Write(const void* pvBuffer, size_t pSize, size_t pCount)
{
    if(m_File.is_null())
        return 0;

    godot::PackedByteArray buffer;
    buffer.resize(pSize * pCount);
    memcpy(buffer.ptrw(), pvBuffer, buffer.size());

    m_File->store_buffer(buffer);
    return buffer.size() / pSize;
}

aiReturn GDAssimpIOStream::Seek(size_t pOffset, aiOrigin pOrigin)
{
    if(m_File.is_null() || pOffset >= FileSize())
        return AI_FAILURE;

    switch (pOrigin)
    {
        case aiOrigin::aiOrigin_SET: m_File->seek(pOffset); break;
        case aiOrigin::aiOrigin_CUR: m_File->seek(Tell() + pOffset); break;
        case aiOrigin::aiOrigin_END: m_File->seek_end(pOffset); break;
    }

    return AI_SUCCESS;
}

size_t GDAssimpIOStream::Tell() const
{
    if(m_File.is_null())
        return 0;

    return m_File->get_position();
}

size_t GDAssimpIOStream::FileSize() const
{
    if(m_File.is_null())
        return 0;

    return m_File->get_length();
}

void GDAssimpIOStream::Flush()
{
    if(m_File.is_null())
        return;

    m_File->flush();
}