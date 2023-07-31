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
        godot::Ref<godot::PackedScene> Load(godot::String _File) const;

    private:
        godot::Spatial *LoadTree(godot::Spatial *_Owner, const aiScene *_Scene, const aiNode *_Node) const;

        godot::Ref<godot::SpatialMaterial> aiMaterialToGodot(const aiMaterial *_Material) const;
};

#endif