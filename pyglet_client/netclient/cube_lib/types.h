struct Vertex {
    float x,y,z;
    float tx,ty;
    unsigned char r,g,b,a;
}

struct VBO {
    int v_num;
    Vertex* vlist;
    int VBO_id;
}
