// License

#include <godot_cpp/classes/base_material3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <cstdint>
#include <cstring>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/classes/image.hpp>
#include <GDAssimpLoader.hpp>
#include <GDAssimpIOStream.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <GDError.hpp>

void GDAssimpLoader::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("load"), &GDAssimpLoader::Load);
    godot::ClassDB::bind_method(godot::D_METHOD("get_errors"), &GDAssimpLoader::GetErrors);
}

godot::Ref<godot::ImageTexture> GDAssimpLoader::LoadTexture(godot::String _BasePath, const aiMaterial *_Material, aiTextureType _Type)
{
    aiString path;
    aiTextureMapMode mappingMode;
    if(_Material->GetTexture(_Type, 0, &path, nullptr, nullptr, nullptr, nullptr, &mappingMode) == AI_SUCCESS)
    {
        godot::Ref<godot::Image> img;
        godot::String textureName;
        godot::Error error = godot::Error::OK;
        if(auto texture = m_CurrentScene->GetEmbeddedTexture(path.C_Str()))
        {
            textureName = texture->mFilename.C_Str();
            // Image data is compressed.
            if(texture->mHeight == 0)
            {
                godot::PackedByteArray data;
                data.resize(texture->mWidth);
                memcpy(data.ptrw(), texture->pcData, texture->mWidth);

                img.instantiate();
                if(strcmp(texture->achFormatHint, "jpg") == 0)
                    error = img->load_jpg_from_buffer(data);
                else if(strcmp(texture->achFormatHint, "png") == 0)
                    error = img->load_png_from_buffer(data);
                else if(strcmp(texture->achFormatHint, "webp") == 0)
                    error = img->load_webp_from_buffer(data);
                else
                    error = godot::Error::ERR_FILE_UNRECOGNIZED;
            }
            else
            {
                godot::PackedByteArray data;
                data.resize(texture->mWidth * texture->mHeight * 4);
                for (uint32_t x = 0; x < texture->mWidth; x++) 
                {
                    for (uint32_t y = 0; y < texture->mHeight; y++) 
                    {
                        data[x * 4 + texture->mWidth * 4 * y] = texture->pcData[x + texture->mWidth * y].r;
                        data[x * 4 + 1 + texture->mWidth * 4 * y] = texture->pcData[x + texture->mWidth * y].g;
                        data[x * 4 + 2 + texture->mWidth * 4 * y] = texture->pcData[x + texture->mWidth * y].b;
                        data[x * 4 + 3 + texture->mWidth * 4 * y] = texture->pcData[x + texture->mWidth * y].a;
                    }
                }

                img = godot::Image::create_from_data(texture->mWidth, texture->mHeight, false, godot::Image::FORMAT_RGBA8, data);
            }
        }
        else
        {
            img.instantiate();
            auto gdpath = _BasePath.path_join(path.C_Str());

            // If the user already moved all textures to the library, load them from there.
            if(!godot::FileAccess::file_exists(gdpath))
            {
                // Gets the autoload AssetsLibrary, to get the real path of the relative one.
                auto tree = Object::cast_to<godot::SceneTree>(godot::Engine::get_singleton()->get_main_loop());
                auto *assetsLibrary = tree->get_root()->get_node_or_null("/root/AssetsLibrary");
                if(assetsLibrary)
                    gdpath = assetsLibrary->call("get_library_path_from_relative_path", path.C_Str());
            }

            textureName = gdpath;
            error = img->load(gdpath);
        }

        if(img.is_valid() && error == godot::Error::OK)
        {
            godot::Ref<godot::ImageTexture> texture = godot::ImageTexture::create_from_image(img);
            if (mappingMode == aiTextureMapMode_Clamp)
                texture->set_meta("repeat", false);

            return texture;
        }
        else
        {
            godot::Ref<GDError> err = memnew(GDError);

            if(error == godot::Error::OK)
                err->ErrorCode = (int)godot::Error::ERR_FILE_CANT_OPEN;
            else
                err->ErrorCode = (int)error;
            err->Message = "Failed to load texture (" + textureName + ")";
            m_Errors.append(err);
        }
    }    

    return nullptr;
}

godot::Ref<godot::StandardMaterial3D> GDAssimpLoader::aiMaterialToGodot(godot::String _BasePath, const aiMaterial *_Material)
{
    godot::Ref<godot::StandardMaterial3D> ret = memnew(godot::StandardMaterial3D);

    int twosided = 0;
    if(_Material->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
        ret->set_cull_mode(twosided ? godot::StandardMaterial3D::CULL_DISABLED : godot::StandardMaterial3D::CULL_BACK);

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
        ret->set_emission_intensity(emissive);
        ret->set_feature(godot::StandardMaterial3D::FEATURE_EMISSION, true);
    }

    float opacity;
    if(_Material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
    {
        if(opacity != 1.0)
        {
            godot::Color c = ret->get_albedo();
            c.a = opacity;
            ret->set_albedo(c);
            ret->set_transparency(godot::StandardMaterial3D::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS);
            ret->set_cull_mode(godot::StandardMaterial3D::CULL_DISABLED);
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
            case aiBlendMode::aiBlendMode_Additive: ret->set_blend_mode(godot::StandardMaterial3D::BLEND_MODE_ADD); break;
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
        ret->set_feature(godot::StandardMaterial3D::FEATURE_ANISOTROPY, true);
    }

    float clearcoat;
    if(_Material->Get(AI_MATKEY_CLEARCOAT_FACTOR, clearcoat) == AI_SUCCESS)
    {
        if(clearcoat > 0)
        {
            ret->set_feature(godot::StandardMaterial3D::FEATURE_CLEARCOAT, true);
            ret->set_clearcoat(clearcoat);

            if(_Material->Get(AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, clearcoat) == AI_SUCCESS)
                ret->set_clearcoat_roughness(clearcoat);
        }
    }

    // Albedo
    auto texture = LoadTexture(_BasePath, _Material, aiTextureType_DIFFUSE);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_BASE_COLOR);

    if(texture.is_valid())
    {
        if (texture->get_image()->detect_alpha() != godot::Image::ALPHA_NONE) 
        {
            ret->set_transparency(godot::StandardMaterial3D::TRANSPARENCY_ALPHA_DEPTH_PRE_PASS);
            ret->set_cull_mode(godot::StandardMaterial3D::CULL_DISABLED); // since you can see both sides in transparent mode
        }

        if(texture->has_meta("repeat"))
        {
            auto value = texture->get_meta("repeat");
            if(value.get_type() == godot::Variant::BOOL)
                ret->set_flag(godot::BaseMaterial3D::Flags::FLAG_USE_TEXTURE_REPEAT, value);
        }
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_ALBEDO, texture);
    }

    // Normals
    texture = LoadTexture(_BasePath, _Material, aiTextureType_NORMALS);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_NORMAL_CAMERA);

    if(texture.is_valid())
    {
        ret->set_feature(godot::StandardMaterial3D::Feature::FEATURE_NORMAL_MAPPING, true);
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_NORMAL, texture);
    }

    // Emission
    texture = LoadTexture(_BasePath, _Material, aiTextureType_EMISSION_COLOR);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_EMISSIVE);

    if(texture.is_valid())
    {
        ret->set_feature(godot::StandardMaterial3D::Feature::FEATURE_EMISSION, true);
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_EMISSION, texture);
    }

    // Metalness
    texture = LoadTexture(_BasePath, _Material, aiTextureType_METALNESS);
    if(texture.is_null())
        texture = LoadTexture(_BasePath, _Material, aiTextureType_SPECULAR);

    if(texture.is_valid())
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_METALLIC, texture);

    // Roughness
    texture = LoadTexture(_BasePath, _Material, aiTextureType_DIFFUSE_ROUGHNESS);
    if(texture.is_valid())
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_ROUGHNESS, texture);

    // AO
    texture = LoadTexture(_BasePath, _Material, aiTextureType_AMBIENT_OCCLUSION);
    if(texture.is_valid())
    {
        ret->set_feature(godot::StandardMaterial3D::Feature::FEATURE_AMBIENT_OCCLUSION, true);
        ret->set_texture(godot::StandardMaterial3D::TEXTURE_AMBIENT_OCCLUSION, texture);
    }

    ret->set_uv1_scale(godot::Vector3(1, -1, 1));
    ret->set_uv1_offset(godot::Vector3(0, 1, 0));

    ret->set_uv2_scale(godot::Vector3(1, -1, 1));
    ret->set_uv2_offset(godot::Vector3(0, 1, 0));

    return ret;
}

godot::Node3D *GDAssimpLoader::LoadTree(godot::String _BasePath, godot::Node3D *_Owner, godot::Node3D *_Parent, const aiNode *_Node)
{
    godot::Node3D *ret = nullptr;

    // Checks for meshes
    if(!_Node->mNumMeshes)
        ret = memnew(godot::Node3D);
    else
    {
        godot::MeshInstance3D *instance = memnew(godot::MeshInstance3D);
        godot::Ref<godot::ArrayMesh> arrayMesh = memnew(godot::ArrayMesh);
        godot::Dictionary materials;

        for (int i = 0; i < _Node->mNumMeshes; i++)
        {
            const aiMesh *mesh = m_CurrentScene->mMeshes[_Node->mMeshes[i]];

            godot::PackedVector3Array vertices;
            godot::PackedInt32Array indices;

            vertices.resize(mesh->mNumVertices);

            memcpy(vertices.ptrw(), mesh->mVertices, sizeof(ai_real) * 3 * mesh->mNumVertices);

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
                godot::PackedVector3Array normals;
                normals.resize(mesh->mNumVertices);
                memcpy(normals.ptrw(), mesh->mNormals, sizeof(ai_real) * 3 * mesh->mNumVertices);
                data[godot::ArrayMesh::ARRAY_NORMAL] = normals;

                if(mesh->HasTangentsAndBitangents()) {
                    godot::PackedRealArray tangents;
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
                godot::PackedColorArray colors;
                colors.resize(mesh->mNumVertices);
                memcpy(colors.ptrw(), mesh->mColors[0], sizeof(ai_real) * 4 * mesh->mNumVertices);
                data[godot::ArrayMesh::ARRAY_COLOR] = colors;
            }

            // Godot only supports two uv channels
            for (size_t i = 0; i < 2; i++)
            {
                if(mesh->mTextureCoords[i])
                {
                    godot::PackedVector2Array uvs;
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

            godot::Ref<godot::StandardMaterial3D> material;
            if(!materials.has(mesh->mMaterialIndex))
            {
                material = aiMaterialToGodot(_BasePath, m_CurrentScene->mMaterials[mesh->mMaterialIndex]);
                materials[mesh->mMaterialIndex] = material;
            }
            else
                material = materials[mesh->mMaterialIndex];

            if(mesh->mColors[0])
                material->set_flag(godot::BaseMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);

            arrayMesh->surface_set_material(arrayMesh->get_surface_count() - 1, material);
        }
        
        instance->set_mesh(arrayMesh);
        ret = instance;
    }

    aiVector3D position, scale, rotation;
    _Node->mTransformation.Decompose(scale, rotation, position);

    ret->set_position(godot::Vector3(position.x, position.y, position.z));
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
        godot::Node3D *child = LoadTree(_BasePath, _Owner, ret, _Node->mChildren[i]);
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

    m_CurrentScene = importer.ReadFile(_File.utf8().get_data(), 
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
    if(!m_CurrentScene)
    {
        // scene->GetEmbeddedTexture()
        godot::Ref<GDError> err = memnew(GDError);
        err->ErrorCode = (int)godot::Error::ERR_CANT_OPEN;
        err->Message = _File + " Error: " + importer.GetErrorString();
        m_Errors.append(err);

        // ERR_PRINT(importer.GetErrorString());
        return nullptr;
    }

    godot::Ref<godot::PackedScene> ret = memnew(godot::PackedScene);
    ret->pack(LoadTree(_File.get_base_dir(), nullptr, nullptr, m_CurrentScene->mRootNode));
    return ret;
}