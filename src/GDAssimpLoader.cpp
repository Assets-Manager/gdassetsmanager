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

    int twosided = 0;
    if(_Material->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
        ret->set_cull_mode(twosided ? godot::SpatialMaterial::CULL_DISABLED : godot::SpatialMaterial::CULL_BACK);

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
            ret->set_feature(godot::SpatialMaterial::FEATURE_TRANSPARENT, true);
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


// #define AI_MATKEY_REFLECTIVITY "$mat.reflectivity", 0, 0

// #define AI_MATKEY_REFRACTI "$mat.refracti", 0, 0
// #define AI_MATKEY_COLOR_DIFFUSE "$clr.diffuse", 0, 0
// #define AI_MATKEY_COLOR_AMBIENT "$clr.ambient", 0, 0
// #define AI_MATKEY_COLOR_SPECULAR "$clr.specular", 0, 0
// #define AI_MATKEY_COLOR_EMISSIVE "$clr.emissive", 0, 0
// #define AI_MATKEY_COLOR_REFLECTIVE "$clr.reflective", 0, 0
// #define AI_MATKEY_GLOBAL_BACKGROUND_IMAGE "?bg.global", 0, 0

// // ---------------------------------------------------------------------------
// // PBR material support
// // --------------------
// // Properties defining PBR rendering techniques
// #define AI_MATKEY_USE_COLOR_MAP "$mat.useColorMap", 0, 0

// // Metallic/Roughness Workflow
// // ---------------------------
// // Base RGBA color factor. Will be multiplied by final base color texture values if extant
// // Note: Importers may choose to copy this into AI_MATKEY_COLOR_DIFFUSE for compatibility
// // with renderers and formats that do not support Metallic/Roughness PBR
// #define AI_MATKEY_BASE_COLOR "$clr.base", 0, 0
// #define AI_MATKEY_BASE_COLOR_TEXTURE aiTextureType_BASE_COLOR, 0
// #define AI_MATKEY_USE_METALLIC_MAP "$mat.useMetallicMap", 0, 0
// // Metallic factor. 0.0 = Full Dielectric, 1.0 = Full Metal
// #define AI_MATKEY_METALLIC_FACTOR "$mat.metallicFactor", 0, 0
// #define AI_MATKEY_METALLIC_TEXTURE aiTextureType_METALNESS, 0
// #define AI_MATKEY_USE_ROUGHNESS_MAP "$mat.useRoughnessMap", 0, 0
// // Roughness factor. 0.0 = Perfectly Smooth, 1.0 = Completely Rough
// #define AI_MATKEY_ROUGHNESS_FACTOR "$mat.roughnessFactor", 0, 0
// #define AI_MATKEY_ROUGHNESS_TEXTURE aiTextureType_DIFFUSE_ROUGHNESS, 0
// // Anisotropy factor. 0.0 = isotropic, 1.0 = anisotropy along tangent direction,
// // -1.0 = anisotropy along bitangent direction
// #define AI_MATKEY_ANISOTROPY_FACTOR "$mat.anisotropyFactor", 0, 0

// // Specular/Glossiness Workflow
// // ---------------------------
// // Diffuse/Albedo Color. Note: Pure Metals have a diffuse of {0,0,0}
// // AI_MATKEY_COLOR_DIFFUSE
// // Specular Color.
// // Note: Metallic/Roughness may also have a Specular Color
// // AI_MATKEY_COLOR_SPECULAR
// #define AI_MATKEY_SPECULAR_FACTOR "$mat.specularFactor", 0, 0
// // Glossiness factor. 0.0 = Completely Rough, 1.0 = Perfectly Smooth
// #define AI_MATKEY_GLOSSINESS_FACTOR "$mat.glossinessFactor", 0, 0

// // Sheen
// // -----
// // Sheen base RGB color. Default {0,0,0}
// #define AI_MATKEY_SHEEN_COLOR_FACTOR "$clr.sheen.factor", 0, 0
// // Sheen Roughness Factor.
// #define AI_MATKEY_SHEEN_ROUGHNESS_FACTOR "$mat.sheen.roughnessFactor", 0, 0
// #define AI_MATKEY_SHEEN_COLOR_TEXTURE aiTextureType_SHEEN, 0
// #define AI_MATKEY_SHEEN_ROUGHNESS_TEXTURE aiTextureType_SHEEN, 1

// // Clearcoat
// // ---------
// // Clearcoat layer intensity. 0.0 = none (disabled)
// #define AI_MATKEY_CLEARCOAT_FACTOR "$mat.clearcoat.factor", 0, 0
// #define AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR "$mat.clearcoat.roughnessFactor", 0, 0
// #define AI_MATKEY_CLEARCOAT_TEXTURE aiTextureType_CLEARCOAT, 0
// #define AI_MATKEY_CLEARCOAT_ROUGHNESS_TEXTURE aiTextureType_CLEARCOAT, 1
// #define AI_MATKEY_CLEARCOAT_NORMAL_TEXTURE aiTextureType_CLEARCOAT, 2

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

godot::Spatial *GDAssimpLoader::LoadTree(godot::Spatial *_Owner, godot::Spatial *_Parent, const aiScene *_Scene, const aiNode *_Node) const
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

            godot::PoolVector3Array vertices;
            godot::PoolIntArray indices;

            vertices.resize(mesh->mNumVertices);

            memcpy(vertices.write().ptr(), mesh->mVertices, sizeof(ai_real) * 3 * mesh->mNumVertices);

            for (size_t f = 0; f < mesh->mNumFaces; f++)
            {
                const aiFace face = mesh->mFaces[f];
                for (size_t idx = 0; idx < face.mNumIndices; idx++)
                    indices.push_back(face.mIndices[idx]);
            }

            godot::Array data;
            data.resize(godot::ArrayMesh::ARRAY_MAX);
            data[godot::ArrayMesh::ARRAY_VERTEX] = vertices;
            data[godot::ArrayMesh::ARRAY_INDEX] = indices;

            // Normals are optional
            if(mesh->mNormals)
            {
                godot::PoolVector3Array normals;
                normals.resize(mesh->mNumVertices);
                memcpy(normals.write().ptr(), mesh->mNormals, sizeof(ai_real) * 3 * mesh->mNumVertices);
                data[godot::ArrayMesh::ARRAY_NORMAL] = normals;

                if(mesh->HasTangentsAndBitangents()) {
                    godot::PoolRealArray tangents;
                    tangents.resize(mesh->mNumVertices * 4);

                    for (size_t j = 0; j < mesh->mNumVertices; j++)
                    {
                        auto aiTangents = mesh->mTangents[j];
                        godot::Vector3 tangent(aiTangents.x, aiTangents.y, aiTangents.z);
                        auto aiBitangent = mesh->mBitangents[j];
                        godot::Vector3 bitangent(aiBitangent.x, aiBitangent.y, aiBitangent.z);
                        float d = ((godot::PoolVector3Array)data[godot::ArrayMesh::ARRAY_NORMAL])[j].cross(tangent).dot(bitangent) > 0.f ? 1.f : -1.f;
                        tangents.set(j * 4, tangent.x);
                        tangents.set(j * 4 + 1, tangent.y);
                        tangents.set(j * 4 + 2, tangent.z);
                        tangents.set(j * 4 + 3, d);
                    }
                    data[godot::ArrayMesh::ARRAY_TANGENT] = tangents;   
                }
            }

            // Vertex colors are optional
            // Since Godot only supports one color "channel", only the first one of assimp will be used.
            if(mesh->mColors[0])
            {
                godot::PoolColorArray colors;
                colors.resize(mesh->mNumVertices);
                memcpy(colors.write().ptr(), mesh->mColors[0], sizeof(ai_real) * 4 * mesh->mNumVertices);
                data[godot::ArrayMesh::ARRAY_COLOR] = colors;
            }

            // Godot only supports two uv channels
            for (size_t i = 0; i < 2; i++)
            {
                if(mesh->mTextureCoords[i])
                {
                    godot::PoolVector2Array uvs;
                    uvs.resize(mesh->mNumVertices);
                    for (size_t j = 0; j < mesh->mNumVertices; i++)
                    {
                        auto vec = mesh->mTextureCoords[i][j];
                        uvs.set(j, godot::Vector2(vec.x, vec.y));
                    }

                    // Assigns channel one and two if needed.
                    data[godot::ArrayMesh::ARRAY_TEX_UV + i] = uvs;
                }
                else
                    break;
            }

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

    aiVector3D position, scale, rotation;
    _Node->mTransformation.Decompose(scale, rotation, position);

    ret->set_translation(godot::Vector3(position.x, position.y, position.z));
    ret->set_scale(godot::Vector3(scale.x, scale.y, scale.z));
    ret->set_rotation(godot::Vector3(rotation.x, rotation.y, rotation.z));

    // Sets the owner for the packed scene.
    if(!_Owner)
        _Owner = ret;

    // Sets the node name
    if(_Node->mName.length > 0)
        ret->set_name(_Node->mName.C_Str());

    if(_Parent)
        _Parent->add_child(ret);

    // Go through all children
    for(int i = 0; i < _Node->mNumChildren; i++)
    {
        godot::Spatial *child = LoadTree(_Owner, ret, _Scene, _Node->mChildren[i]);
        child->set_owner(_Owner);
    }

    return ret;
}

godot::Ref<godot::PackedScene> GDAssimpLoader::Load(godot::String _File) const
{
    Assimp::Importer importer;
    importer.SetIOHandler(new GDAssimpIOSystem());
    importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
    importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

    const aiScene *scene = importer.ReadFile(_File.utf8().get_data(), 
                                            aiProcess_CalcTangentSpace |
                                            aiProcess_FlipWindingOrder | 
                                            aiProcess_ImproveCacheLocality |
                                            aiProcess_Triangulate | 
                                            aiProcess_GenUVCoords |
                                            aiProcess_JoinIdenticalVertices | 
                                            aiProcess_TransformUVCoords |
                                            aiProcess_FindInstances |
                                            aiProcess_RemoveRedundantMaterials | 
                                            aiProcess_PopulateArmatureData |
                                            aiProcess_OptimizeMeshes);
    if(!scene)
    {
        ERR_PRINT(importer.GetErrorString());
        return nullptr;
    }

    godot::Ref<godot::PackedScene> ret = godot::PackedScene::_new();
    ret->pack(LoadTree(nullptr, nullptr, scene, scene->mRootNode));
    return ret;
}