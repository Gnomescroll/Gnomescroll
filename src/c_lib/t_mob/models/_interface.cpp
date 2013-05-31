#include "_interface.hpp"

#include <t_mob/models/assimp_includes.hpp>
#include <t_mob/models/model_loader.hpp>
#include <t_mob/models/body_mesh.hpp>

namespace t_mob
{

static class ModelLoader* model_loader;
static class BodyMesh* body_mesh;

void init_models()
{
    GS_ASSERT(model_loader == NULL);
    GS_ASSERT(body_mesh == NULL);
    size_t bsize = 0;
    char* buffer = read_file_to_buffer(MEDIA_PATH "mesh/player.dae", &bsize);
    IF_ASSERT(buffer == NULL) return;
    int aFlag = (aiProcess_Triangulate |
                 //aiProcess_GenUVCoords |
                 //aiProcess_TransformUVCoords |
                 //aiProcess_RemoveComponent | //strip components on
                 aiProcess_ValidateDataStructure);
    char* aHint = NULL;
    aiPropertyStore* property_store = aiCreatePropertyStore();
    int aFlagRemove = (aiComponent_TANGENTS_AND_BITANGENTS | aiComponent_COLORS |
                       aiComponent_LIGHTS | aiComponent_CAMERAS |
                       aiComponent_TEXTURES | aiComponent_MATERIALS);

    aiSetImportPropertyInteger(property_store, AI_CONFIG_PP_RVC_FLAGS, aFlagRemove);
    //const struct aiScene* pScene = aiImportFileFromMemory(buffer, bsize, aFlag , aHint);
    const struct aiScene* pScene = aiImportFileFromMemoryWithProperties(buffer, bsize, aFlag , aHint, property_store);
    free(buffer);
    aiReleasePropertyStore(property_store);

    model_loader = new ModelLoader;
    model_loader->init(pScene);

    body_mesh = new BodyMesh;
    body_mesh->load(model_loader);

    body_mesh->draw_prep();
}

void draw_models()
{
    struct Vec3 p = ClientState::location_pointer;
    if (!ClientState::location_pointer_set) p = vec3_init(128.0f);
    body_mesh->draw_prep();
    body_mesh->draw(p.x, p.y, p.z + 3.0f);
    //bt->draw_skeleton(p.x+0.0, p.y+0.0f, p.z + 5.0f);
}

void teardown_models()
{
    delete model_loader;
    delete body_mesh;
}

}   // t_mob
