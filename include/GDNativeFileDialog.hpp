// License

#ifndef NATIVEFILEDIALOG_HPP
#define NATIVEFILEDIALOG_HPP

#include <Godot.hpp>
#include <Node.hpp>

enum FileDialogType
{
    OPEN_FILE,
    SAVE_FILE,
    SELECT_DIR
};

class GDNativeFileDialog : public godot::Node
{
    GODOT_CLASS(GDNativeFileDialog, godot::Node);
    public:
        GDNativeFileDialog() : m_Type(FileDialogType::OPEN_FILE), m_Multiselect(false) {}

        void _init() { }

        static void _register_methods();

        /**
         * @brief Sets the dialog title.
         */
        void SetTitle(godot::String _Title);
        godot::String GetTitle() const;

        /**
         * @brief Sets the initial path of the filedialog.
         */
        void SetInitialPath(godot::String _InitialPath);
        godot::String GetInitialPath() const;

        /**
         * @brief Sets file filters.
         */
        void SetFilters(godot::PoolStringArray _Filters);
        godot::PoolStringArray GetFilters() const;

        /**
         * @brief Sets the file dialog type.
         */
        void SetDialogType(int _Type);
        int GetDialogType() const;

        /**
         * @brief Enable file multiselect.
         */
        void AllowMultiselect(bool _Multiselect);
        bool IsMultiselect() const;

        /**
         * @return Returns the selected paths of the filedialog or empty, if the user canceled the request.
         */
        godot::PoolStringArray ShowModal();

        // pfd::save_file::save_file(std::string const &title,
        //                   std::string const &initial_path,
        //                   std::vector<std::string> filters = { "All Files", "*" },
        //                   pfd::opt option = pfd::opt::none);

        ~GDNativeFileDialog() {}

    private:
        godot::String m_Title;
        godot::String m_InitialPath;
        godot::PoolStringArray m_Filters;
        FileDialogType m_Type;
        bool m_Multiselect;
};

#endif