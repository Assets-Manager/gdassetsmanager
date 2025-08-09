// License

#ifndef GDASSIMP_HPP
#define GDASSIMP_HPP

#include <godot_cpp/variant/typed_array.hpp>
#include <GDError.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

#include <assimp/scene.h>

class GDAssimpLoader : public godot::RefCounted
{
    GDCLASS(GDAssimpLoader, godot::RefCounted);
    public:
        static void _bind_methods();

        /**
         * @brief Loads a 3D model file.
         * 
         * @return Returns a new PackedScene or null on error.
         */
        godot::Ref<godot::PackedScene> Load(godot::String _File);

        godot::TypedArray<GDError> GetErrors()
        {
            return m_Errors;
        }
    private:
        const aiScene *m_CurrentScene{};
        godot::TypedArray<GDError> m_Errors;

        godot::Ref<godot::ImageTexture> LoadTexture(godot::String _BasePath, const aiMaterial *_Material, aiTextureType _Type);

        godot::Node3D *LoadTree(godot::String _BasePath, godot::Node3D *_Owner, godot::Node3D *_Parent, const aiNode *_Node);

        godot::Ref<godot::StandardMaterial3D> aiMaterialToGodot(godot::String _BasePath, const aiMaterial *_Material);
};

#endif