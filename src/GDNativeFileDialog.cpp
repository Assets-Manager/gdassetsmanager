// License

#include <GDNativeFileDialog.hpp>
#include <portable-file-dialogs.h>

void GDNativeFileDialog::_register_methods()
{
    godot::register_property<GDNativeFileDialog, godot::String>("title", &GDNativeFileDialog::SetTitle, &GDNativeFileDialog::GetTitle, "");
    godot::register_property<GDNativeFileDialog, godot::String>("initial_path", &GDNativeFileDialog::SetInitialPath, &GDNativeFileDialog::GetInitialPath, "");
    godot::register_property<GDNativeFileDialog, godot::PoolStringArray>("filters", &GDNativeFileDialog::SetFilters, &GDNativeFileDialog::GetFilters, godot::PoolStringArray());
    godot::register_property<GDNativeFileDialog, int>("dialog_type", &GDNativeFileDialog::SetDialogType, &GDNativeFileDialog::GetDialogType, (int) FileDialogType::OPEN_FILE, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_ENUM, "OPEN_FILE, SAVE_FILE, SELECT_DIR");
    godot::register_property<GDNativeFileDialog, bool>("multiselect", &GDNativeFileDialog::AllowMultiselect, &GDNativeFileDialog::IsMultiselect, false);
    godot::register_method("show_modal", &GDNativeFileDialog::ShowModal);
}

void GDNativeFileDialog::SetTitle(godot::String _Title)
{
    m_Title = _Title;
}

godot::String GDNativeFileDialog::GetTitle() const
{
    return m_Title;
}

void GDNativeFileDialog::SetInitialPath(godot::String _InitialPath)
{
    m_InitialPath = _InitialPath;
}

godot::String GDNativeFileDialog::GetInitialPath() const
{
    return m_InitialPath;
}

void GDNativeFileDialog::SetFilters(godot::PoolStringArray _Filters)
{
    m_Filters = _Filters;
}

godot::PoolStringArray GDNativeFileDialog::GetFilters() const
{
    return m_Filters;
}

void GDNativeFileDialog::SetDialogType(int _Type)
{
    m_Type = (FileDialogType)_Type;
}

int GDNativeFileDialog::GetDialogType() const
{
    return (int) m_Type;
}

void GDNativeFileDialog::AllowMultiselect(bool _Multiselect)
{
    m_Multiselect = _Multiselect;
}

bool GDNativeFileDialog::IsMultiselect() const
{
    return m_Multiselect;
}

godot::PoolStringArray GDNativeFileDialog::ShowModal()
{
    godot::PoolStringArray result;
    std::vector<std::string> filters;

    // Converts the godot filter to a pfd one.
    for (size_t i = 0; i < m_Filters.size(); i++)
    {
        auto filter = m_Filters[i].split(";");
        if(filter.size() == 2)
        {
            filters.push_back(filter[1].strip_edges().utf8().get_data());
            filters.push_back(filter[0].strip_edges().replace(",", ";").utf8().get_data());      
        }
    }

    switch (m_Type)
    {
        case FileDialogType::OPEN_FILE:
        {
            auto pfdResults = pfd::open_file(m_Title.utf8().get_data(), m_InitialPath.utf8().get_data(), filters, m_Multiselect ? pfd::opt::multiselect : pfd::opt::none).result();
            for (auto &&pfdResult : pfdResults)
                result.push_back(pfdResult.c_str());
        } break;
        
        case FileDialogType::SAVE_FILE:
        {
            auto pfdResult = pfd::save_file(m_Title.utf8().get_data(), m_InitialPath.utf8().get_data(), filters, pfd::opt::none).result();
            if(!pfdResult.empty())
                result.push_back(pfdResult.c_str());
        } break;

        case FileDialogType::SELECT_DIR:
        {
            auto pfdResult = pfd::select_folder(m_Title.utf8().get_data(), m_InitialPath.utf8().get_data()).result();
            if(!pfdResult.empty())
                result.push_back(pfdResult.c_str());
        } break;
    }

    return result;
}