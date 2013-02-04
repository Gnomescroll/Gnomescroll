#pragma once

namespace t_mob
{

class ModelLoader
{
    public:

        aiScene* pScene;    //the scene
        aiNode** nl;    //node list
        int nlm;        //node list max
        int nli;        //node list index (just a counter variable)
        struct _Mesh* _ml;
        int _mlm;

        struct Node
        {
            char* name;
            struct aiNode* node;
            struct Mat4 mTransformation;
            struct Node* p;     //parent
            struct Node** c;     //children
            int cn;             //children number
            int index;          //index for list
        };

        struct Node* _nl;
        int _nlm;

        struct BoneNode
        {
            char* name;
            struct Mat4 mOffsetMatrix;
            struct aiNode* parent_node;   //parent node
            int parent_index;
        };

        struct BoneNode* bnl;
        int bnlm;

    ModelLoader() :
        pScene(NULL), nl(NULL), nlm(0), nli(0), _ml(NULL), _mlm(0),
        _nl(NULL), _nlm(0), bnl(NULL), bnlm(0)
    {
    }

    ~ModelLoader()
    {
        if (this->pScene != NULL) aiReleaseImport(this->pScene);
        if (this->nl != NULL) delete[] this->nl;
    }

    void init(aiScene* pScene);
    void count_nodes(aiNode* pNode);
    void set_node_parents(aiNode* pNode);
    bool bone_in_list(aiBone* bone, int bone_count);
    int get_bone_index(aiBone* bone);
    bool node_in_list(struct aiNode* node, int node_count);
    int node_index_from_list(struct aiNode* node);
    aiNode* find_node_recursively_by_name(aiNode* pNode, char* node_name);
    aiNode* find_node_by_name(char* node_name);
    struct Mat4 get_anim_matrix(int frame_time, aiNodeAnim** node_channels, int node_channel_max, aiNode* node);

    private:
    void _set_node_index(int* index, Node* node);
    void init_node_list();
    void initialize_meshes1();
    void initialize_meshes2();
    void init_bone_list();
    void init_bone_list2();
};


}   // t_mob
