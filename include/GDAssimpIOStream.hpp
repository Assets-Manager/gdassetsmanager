// License

#ifndef GDASSIMPIOSTREAM_HPP
#define GDASSIMPIOSTREAM_HPP

#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <Godot.hpp>
#include <File.hpp>
#include <Reference.hpp>

class GDAssimpIOSystem;
class GDAssimpIOStream : public Assimp::IOStream
{
    friend GDAssimpIOSystem;
    public:
        ~GDAssimpIOStream() = default;

        size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override;
        size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override;
        aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;
        size_t Tell() const override;
        size_t FileSize() const override;
        void Flush() override;
    protected:
        GDAssimpIOStream(const char* _File, const std::string& _Mode);

    private:
        godot::Ref<godot::File> m_File;
};

class GDAssimpIOSystem : public Assimp::IOSystem
{
    public:
        GDAssimpIOSystem() : Assimp::IOSystem() {}

        bool Exists(const char* _File) const override
        {
            godot::Ref<godot::File> file = godot::File::_new();
            return file->file_exists(_File);
        }

        char getOsSeparator() const override
        {
            return '/';
        }

        inline Assimp::IOStream* Open(const char* _File, const char* _Mode) override
        {
            return new GDAssimpIOStream(_File, std::string(_Mode));
        }

        inline void Close(Assimp::IOStream* _File) override { delete _File; }

        ~GDAssimpIOSystem() = default;
};

#endif