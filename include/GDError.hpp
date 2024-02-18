// License

#ifndef GDERROR_HPP
#define GDERROR_HPP

#include <Godot.hpp>
#include <Reference.hpp>

class GDError : public godot::Reference
{
    GODOT_CLASS(GDError, godot::Reference);
    public:
        int ErrorCode;
        godot::String Message;

        GDError() = default;

        void _init() { }

        static void _register_methods()
        {
            godot::register_property("error_code", &GDError::ErrorCode, (int)godot::Error::OK);
            godot::register_property("message", &GDError::Message, godot::String());
        }

        ~GDError() = default;
};

#endif