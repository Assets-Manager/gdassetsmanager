// License

#include <GDFilewatcher.hpp>

void GDFilewatcher::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("open"), &GDFilewatcher::Open);
    godot::ClassDB::bind_method(godot::D_METHOD("close"), &GDFilewatcher::Close);

    ADD_SIGNAL(godot::MethodInfo("file_changed", godot::PropertyInfo(godot::Variant::STRING, "path"), godot::PropertyInfo(godot::Variant::INT, "type")));
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
