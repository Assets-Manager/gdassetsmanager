// License

#ifndef GDASSIMP_HPP
#define GDASSIMP_HPP

#include <Godot.hpp>
#include <Reference.hpp>
#include <PackedScene.hpp>
#include <Spatial.hpp>
#include <assimp/scene.h>
#include <SpatialMaterial.hpp>

class GDAssimpLoader : public godot::Reference
{
    GODOT_CLASS(GDAssimpLoader, godot::Reference);
    public:
        void _init() { }

        static void _register_methods();

        /**
         * @brief Loads a 3D model file.
         * 
         * @return Returns a new PackedScene or null on error.
         */
        godot::Ref<godot::PackedScene> Load(godot::String _File);

        godot::Array GetErrors()
        {
            return m_Errors;
        }
    private:
        godot::Array m_Errors;

        godot::Ref<godot::Texture> LoadTexture(godot::String _BasePath, const aiMaterial *_Material, aiTextureType _Type);

        godot::Spatial *LoadTree(godot::String _BasePath, godot::Spatial *_Owner, godot::Spatial *_Parent, const aiScene *_Scene, const aiNode *_Node);

        godot::Ref<godot::SpatialMaterial> aiMaterialToGodot(godot::String _BasePath, const aiMaterial *_Material);
};

#endif