// License

#include <MeshInstance.hpp>
#include <ArrayMesh.hpp>
#include <GDAssimpLoader.hpp>
#include <GDAssimpIOStream.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <Image.hpp>
#include <ImageTexture.hpp>
#include <GDError.hpp>
#include <Variant.hpp>

void GDAssimpLoader::_register_methods()
{
    godot::register_method("load", &GDAssimpLoader::Load);
    godot::register_method("get_errors", &GDAssimpLoader::GetErrors);
}

godot::Ref<godot::Texture> GDAssimpLoader::LoadTexture(godot::String _BasePath, const aiMaterial *_Material, aiTextureType _Type)
{
    aiString path;
    aiTextureMapMode mappingMode;
    if(_Material->GetTexture(_Type, 0, &path, nullptr, nullptr, nullptr, nullptr, &mappingMode) == AI_SUCCESS)
    {
        godot::Ref<godot::Image> img = godot::Image::_new();
        auto gdpath = _BasePath.plus_file(path.C_Str());
        if(img->load(gdpath) == godot::Error::OK)
        {
            godot::Ref<godot::ImageTexture> texture = godot::ImageTexture::_new();
            int32_t flags = godot::Texture::FLAGS_DEFAULT;

            if (mappingMode == aiTextureMapMode_Clamp) 
                flags = flags & ~godot::Texture::FLAG_REPEAT;
            else if (mappingMode == aiTextureMapMode_Mirror)
                flags = flags | godot::Texture::FLAG_MIRRORED_REPEAT;

            texture->create_from_image(img, flags);
            return texture;
        }
        else
        {
            godot::Ref<GDError> err = GDError::_new();
            err->ErrorCode = (int)godot::Error::ERR_FILE_CANT_OPEN;
            err->Message = gdpath;
            m_Errors.append(err);
        }
    }    

    return nullptr;
}

godot::Ref<godot::SpatialMaterial> GDAssimpLoader::aiMaterialToGodot(godot::String _BasePath, const aiMaterial *_Material)
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

    float emissive;
    if(_Material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissive) == AI_SUCCESS)
    {
        ret->set_emission_energy(emissive);
        ret->set_feature(godot::SpatialMaterial::FEATURE_EMISSION, true);
    }

    float opacity;
    if(_Material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
    {
        if(opacity != 1.0)
        {
            godot::Color c = ret->get_albedo();
            c.a = opacity;
            ret->set_albedo(c);
            ret->set_feature(godot::SpatialMaterial::FEATURE_TRANSPARENT, true);
            ret->set_depth_draw_mode(godot::SpatialMaterial::DepthDrawMode::DEPTH_DRAW_ALPHA_OPAQUE_PREPASS);
            ret->set_cull_mode(godot::SpatialMaterial::CULL_DISABLED);
        }
    }

    float specular;
    if(_Material->Get(AI_MATKEY_SPECULAR_FACTOR, specular) == AI_SUCCESS)
        ret->set_specular(specular);

    int blendMode;
    if(_Material->Get(AI_MATKEY_BLEND_FUNC, blendMode) == AI_SUCCESS)
    {
        switch (blendMode)
        {
            case aiBlendMode::aiBlendMode_Additive: ret->set_blend_mode(godot::SpatialMaterial::BLEND_MODE_ADD); break;
        }
    }

    float metal;
    if(_Material->Get(AI_MATKEY_METALLIC_FACTOR, metal) == AI_SUCCESS)
        ret->set_metallic(metal);

    float roughness;
    if((_Material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) || (_Material->Get(AI_MATKEY_GLOSSINESS_FACTOR, roughness) == AI_SUCCESS))
        ret->set_roughness(roughness);

    float anisotropy;
    if(_Material->Get(AI_MATKEY_ANISOTROPY_FACTOR, roughness) == AI_SUCCESS)
    {
        ret->set_anisotropy(anisotropy);
        ret->set_feature(godot::SpatialMaterial::FEATURE_ANISOTROPY, true);
    }

    float clearcoat;
    if(_Material->Get(AI_MATKEY_CLEARCOAT_FACTOR, clearcoat) == AI_SUCCESS)
    {
        if(clearcoat > 0)
        {
            ret->set_feature(godot::SpatialMaterial::FEATURE_CLEARCOAT, true);
            ret->set_clearcoat(clearcoat);

            if(_Material->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, clearcoat) == AI_SUCCESS)
                ret->set_clearcoat_gloss(clearcoat);
        }
    }

    // Albedo
    auto texture = LoadTexture(_BasePath, _Material, aiTextureType_DIFFUSE);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_BASE_COLOR);

    if(texture.is_valid())
    {
        if (texture->get_data()->detect_alpha() != godot::Image::ALPHA_NONE) 
        {
            ret->set_feature(godot::SpatialMaterial::FEATURE_TRANSPARENT, true);
            ret->set_depth_draw_mode(godot::SpatialMaterial::DepthDrawMode::DEPTH_DRAW_ALPHA_OPAQUE_PREPASS);
            ret->set_cull_mode(godot::SpatialMaterial::CULL_DISABLED); // since you can see both sides in transparent mode
        }

        ret->set_texture(godot::SpatialMaterial::TEXTURE_ALBEDO, texture);
    }

    // Normals
    texture = LoadTexture(_BasePath, _Material, aiTextureType_NORMALS);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_NORMAL_CAMERA);

    if(texture.is_valid())
    {
        ret->set_feature(godot::SpatialMaterial::Feature::FEATURE_NORMAL_MAPPING, true);
        ret->set_texture(godot::SpatialMaterial::TEXTURE_NORMAL, texture);
    }

    // Emission
    texture = LoadTexture(_BasePath, _Material, aiTextureType_EMISSION_COLOR);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_EMISSIVE);

    if(texture.is_valid())
    {
        ret->set_feature(godot::SpatialMaterial::Feature::FEATURE_EMISSION, true);
        ret->set_texture(godot::SpatialMaterial::TEXTURE_EMISSION, texture);
    }

    // Metalness
    texture = LoadTexture(_BasePath, _Material, aiTextureType_METALNESS);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_SPECULAR);

    if(texture.is_valid())
        ret->set_texture(godot::SpatialMaterial::TEXTURE_METALLIC, texture);

    // Roughness
    texture = LoadTexture(_BasePath, _Material, aiTextureType_DIFFUSE_ROUGHNESS);
    if(texture.is_valid())
        ret->set_texture(godot::SpatialMaterial::TEXTURE_ROUGHNESS, texture);

    // AO
    texture = LoadTexture(_BasePath, _Material, aiTextureType_AMBIENT_OCCLUSION);
    if(texture.is_valid())
    {
        ret->set_feature(godot::SpatialMaterial::Feature::FEATURE_AMBIENT_OCCLUSION, true);
        ret->set_texture(godot::SpatialMaterial::TEXTURE_AMBIENT_OCCLUSION, texture);
    }

    ret->set_uv1_scale(godot::Vector3(1, -1, 1));
    ret->set_uv1_offset(godot::Vector3(0, 1, 0));

    ret->set_uv2_scale(godot::Vector3(1, -1, 1));
    ret->set_uv2_offset(godot::Vector3(0, 1, 0));

    return ret;
}

godot::Spatial *GDAssimpLoader::LoadTree(godot::String _BasePath, godot::Spatial *_Owner, godot::Spatial *_Parent, const aiScene *_Scene, const aiNode *_Node)
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

                        float d = normals[j].cross(tangent).dot(bitangent) > 0.f ? 1.f : -1.f;
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
                    for (size_t j = 0; j < mesh->mNumVertices; j++)
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
                material = aiMaterialToGodot(_BasePath, _Scene->mMaterials[mesh->mMaterialIndex]);
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
        godot::Spatial *child = LoadTree(_BasePath, _Owner, ret, _Scene, _Node->mChildren[i]);
        child->set_owner(_Owner);
    }

    return ret;
}

godot::Ref<godot::PackedScene> GDAssimpLoader::Load(godot::String _File)
{
    m_Errors.clear();

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
        godot::Ref<GDError> err = GDError::_new();
        err->ErrorCode = (int)godot::Error::ERR_CANT_OPEN;
        err->Message = _File + " Error: " + importer.GetErrorString();
        m_Errors.append(err);

        // ERR_PRINT(importer.GetErrorString());
        return nullptr;
    }

    godot::Ref<godot::PackedScene> ret = godot::PackedScene::_new();
    ret->pack(LoadTree(_File.get_base_dir(), nullptr, nullptr, scene, scene->mRootNode));
    return ret;
}