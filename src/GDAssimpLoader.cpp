// License

#include <MeshInstance.hpp>
#include <ArrayMesh.hpp>
#include <GDAssimpLoader.hpp>
#include <GDAssimpIOStream.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void GDAssimpLoader::_register_methods()
{
    godot::register_method("load", &GDAssimpLoader::Load);
}

godot::Ref<godot::SpatialMaterial> GDAssimpLoader::aiMaterialToGodot(const aiMaterial *_Material) const
{
    godot::Ref<godot::SpatialMaterial> ret = godot::SpatialMaterial::_new();

    aiString name;
    if(_Material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
        ret->set_name(name.C_Str());

    aiColor3D color;
    if(_Material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        ret->set_albedo(godot::Color(color.r, color.g, color.b));

    if(_Material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
        ret->set_emission(godot::Color(color.r, color.g, color.b));

    float opacity;
    if(_Material->Get(AI_MATKEY_OPACITY, color) == AI_SUCCESS)
    {
        if(opacity != 1.0)
        {
            godot::Color c = ret->get_albedo();
            c.a = opacity;
            ret->set_albedo(c);
        }
    }

    float shiny;
    if(_Material->Get(AI_MATKEY_SHININESS_STRENGTH, shiny) == AI_SUCCESS)
        ret->set_specular(shiny);

    int blendMode;
    if(_Material->Get(AI_MATKEY_BLEND_FUNC, blendMode) == AI_SUCCESS)
    {
        switch (blendMode)
        {
            case aiBlendMode::aiBlendMode_Additive: ret->set_blend_mode(godot::SpatialMaterial::BLEND_MODE_ADD); break;
        }
    }
    
    int twosided = 0;
    if(_Material->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
        ret->set_cull_mode(twosided ? godot::SpatialMaterial::CULL_DISABLED : godot::SpatialMaterial::CULL_BACK);

    
	// void set_clearcoat(const real_t clearcoat);
	// void set_clearcoat_gloss(const real_t clearcoat_gloss);

	// void set_emission_energy(const real_t emission_energy);
	// void set_emission_operator(const int64_t _operator);

	// void set_feature(const int64_t feature, const bool enable);

	// void set_flag(const int64_t flag, const bool enable);

	// void set_metallic(const real_t metallic);
	// void set_metallic_texture_channel(const int64_t channel);

	// void set_refraction(const real_t refraction);
	// void set_refraction_texture_channel(const int64_t channel);

	// void set_rim(const real_t rim);
	// void set_rim_tint(const real_t rim_tint);

	// void set_roughness(const real_t roughness);
	// void set_roughness_texture_channel(const int64_t channel);

	// void set_specular(const real_t specular);
	// void set_specular_mode(const int64_t specular_mode);

	// void set_subsurface_scattering_strength(const real_t strength);

	// void set_texture(const int64_t param, const Ref<Texture> texture);

	// void set_transmission(const Color transmission);

	// void set_uv1_offset(const Vector3 offset);
	// void set_uv1_scale(const Vector3 scale);
	// void set_uv1_triplanar_blend_sharpness(const real_t sharpness);

	// void set_uv2_offset(const Vector3 offset);
	// void set_uv2_scale(const Vector3 scale);
	// void set_uv2_triplanar_blend_sharpness(const real_t sharpness);

    return ret;
}



	// void set_ao_light_affect(const real_t amount);
	// void set_ao_texture_channel(const int64_t channel);

    // void set_depth_deep_parallax(const bool enable);
	// void set_depth_deep_parallax_flip_binormal(const bool flip);
	// void set_depth_deep_parallax_flip_tangent(const bool flip);
	// void set_depth_deep_parallax_max_layers(const int64_t layer);
	// void set_depth_deep_parallax_min_layers(const int64_t layer);
	// void set_depth_draw_mode(const int64_t depth_draw_mode);
	// void set_depth_scale(const real_t depth_scale);

	// void set_detail_blend_mode(const int64_t detail_blend_mode);
	// void set_detail_uv(const int64_t detail_uv);
	// void set_diffuse_mode(const int64_t diffuse_mode);
	// void set_distance_fade(const int64_t mode);
	// void set_distance_fade_max_distance(const real_t distance);
	// void set_distance_fade_min_distance(const real_t distance);

	// void set_grow(const real_t amount);
	// void set_grow_enabled(const bool enable);
	// void set_line_width(const real_t line_width);

	// void set_normal_scale(const real_t normal_scale);
	// void set_particles_anim_h_frames(const int64_t frames);
	// void set_particles_anim_loop(const bool loop);
	// void set_particles_anim_v_frames(const int64_t frames);
	// void set_point_size(const real_t point_size);
	// void set_proximity_fade(const bool enabled);
	// void set_proximity_fade_distance(const real_t distance);

godot::Spatial *GDAssimpLoader::LoadTree(godot::Spatial *_Owner, const aiScene *_Scene, const aiNode *_Node) const
{
    godot::Spatial *ret = nullptr;

    // Checks for meshes
    if(!_Node->mNumMeshes)
        ret = godot::Spatial::_new();
    else
    {
        godot::MeshInstance *instance = godot::MeshInstance::_new();
        godot::Ref<godot::ArrayMesh> arrayMesh = godot::ArrayMesh::_new();
        godot::Dictionary materials;

        for (int i = 0; i < _Node->mNumMeshes; i++)
        {
            const aiMesh *mesh = _Scene->mMeshes[_Node->mMeshes[i]];

            godot::PoolVector3Array vertices, normals;
            godot::PoolVector2Array uvs;
            godot::PoolIntArray indices;

            vertices.resize(mesh->mNumVertices);
            normals.resize(mesh->mNumVertices);
            // uvs.resize(mesh->mNumVertices);

            memcpy(vertices.write().ptr(), mesh->mVertices, sizeof(ai_real) * 3 * mesh->mNumVertices);
            memcpy(normals.write().ptr(), mesh->mNormals, sizeof(ai_real) * 3 * mesh->mNumVertices);

            for (size_t f = 0; f < mesh->mNumFaces; f++)
            {
                const aiFace face = mesh->mFaces[f];
                for (size_t idx = 0; idx < face.mNumIndices; idx++)
                    indices.push_back(face.mIndices[idx]);
            }

            godot::Array data;
            data.resize(godot::ArrayMesh::ARRAY_MAX);
            data[godot::ArrayMesh::ARRAY_VERTEX] = vertices;
            data[godot::ArrayMesh::ARRAY_NORMAL] = normals;
            data[godot::ArrayMesh::ARRAY_INDEX] = indices;

            arrayMesh->add_surface_from_arrays(godot::ArrayMesh::PRIMITIVE_TRIANGLES, data);

            godot::Ref<godot::SpatialMaterial> material;
            if(!materials.has(mesh->mMaterialIndex))
            {
                material = aiMaterialToGodot(_Scene->mMaterials[mesh->mMaterialIndex]);
                materials[mesh->mMaterialIndex] = material;
            }
            else
                material = materials[mesh->mMaterialIndex];

            arrayMesh->surface_set_material(arrayMesh->get_surface_count() - 1, material);
        }
        
        instance->set_mesh(arrayMesh);
        ret = instance;
    }

    // Sets the owner for the packed scene.
    if(!_Owner)
        _Owner = ret;

    // Sets the node name
    if(_Node->mName.length > 0)
        ret->set_name(_Node->mName.C_Str());

    // Go through all children
    for(int i = 0; i < _Node->mNumChildren; i++)
    {
        godot::Spatial *child = LoadTree(_Owner, _Scene, _Node->mChildren[i]);
        ret->add_child(child);
        child->set_owner(_Owner);
    }

    return ret;
}

godot::Ref<godot::PackedScene> GDAssimpLoader::Load(godot::String _File) const
{
    Assimp::Importer importer;
    importer.SetIOHandler(new GDAssimpIOSystem());

    const aiScene *scene = importer.ReadFile(_File.utf8().get_data(), aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes);
    if(!scene)
    {
        ERR_PRINT(importer.GetErrorString());
        return nullptr;
    }

    godot::Ref<godot::PackedScene> ret = godot::PackedScene::_new();
    ret->pack(LoadTree(nullptr, scene, scene->mRootNode));
    return ret;
}