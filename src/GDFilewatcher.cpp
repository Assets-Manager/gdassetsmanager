// License

#include <GDFilewatcher.hpp>

void GDFilewatcher::_register_methods()
{
    godot::register_method("open", &GDFilewatcher::Open);
    godot::register_method("close", &GDFilewatcher::Close);
    godot::register_signal<GDFilewatcher>("file_changed", "path", godot::Variant::STRING, "type", godot::Variant::INT);
}

void GDFilewatcher::Open(godot::String _Path)
{
    m_Watcher = new filewatch::FileWatch<std::string>(_Path.utf8().get_data(), 
    	[this](const std::string& _Path, const filewatch::Event _ChangeEvent) {
            this->emit_signal("file_changed", godot::String(_Path.c_str()), (int)_ChangeEvent);
	    }
    );
}

void GDFilewatcher::Close()
{
    if(m_Watcher)
    {
        delete m_Watcher;
        m_Watcher = nullptr;
    }
}
