// License

#ifndef GDFILEWATCHER_HPP
#define GDFILEWATCHER_HPP

#include <Godot.hpp>
#include <Reference.hpp>
#include <FileWatch.hpp>
#include <String.hpp>
#include <string>

class GDFilewatcher : public godot::Reference
{
    GODOT_CLASS(GDFilewatcher, godot::Reference);
    public:
        GDFilewatcher() : m_Watcher(nullptr) {}

        void _init() { }

        static void _register_methods();

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