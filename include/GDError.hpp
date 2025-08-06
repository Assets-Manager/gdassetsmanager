// License

#ifndef GDERROR_HPP
#define GDERROR_HPP

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

class GDError : public godot::RefCounted
{
    GDCLASS(GDError, godot::RefCounted);
    public:
        int ErrorCode;
        godot::String Message;

        GDError() = default;

        int GetErrorCode() const { return ErrorCode; }
        godot::String GetMessage() const { return Message; }

        static void _bind_methods()
        {
            godot::ClassDB::bind_method(godot::D_METHOD("get_error_code"), &GDError::GetErrorCode);
            godot::ClassDB::bind_method(godot::D_METHOD("get_message"), &GDError::GetMessage);

            ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "error_code"), "", "get_error_code");
            ADD_PROPERTY(godot::PropertyInfo(godot::Variant::FLOAT, "message"), "", "get_message");
        }

        ~GDError() = default;
};

#endif