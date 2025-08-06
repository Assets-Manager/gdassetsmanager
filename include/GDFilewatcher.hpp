// License

#ifndef GDFILEWATCHER_HPP
#define GDFILEWATCHER_HPP

#include <FileWatch.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

class GDFilewatcher : public godot::RefCounted
{
    GDCLASS(GDFilewatcher, godot::RefCounted);
    public:
        GDFilewatcher() : m_Watcher(nullptr) {}

        static void _bind_methods();

        /**
         * @brief Opens a directory to observe.
         */
        void Open(godot::String _Path);

        /**
         * @brief Closes the last opened directory.
         */
        void Close();

        ~GDFilewatcher() { Close(); }
    private:
        filewatch::FileWatch<std::string> *m_Watcher;
};

#endif