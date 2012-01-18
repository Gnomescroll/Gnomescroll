

struct Vec3 
{
    union
    {
        float f[3];
        struct 
        {
            float x,y,z;
        };
    };
};

/*
    vector operations
*/

struct Vec3 vec3_normalize(struct Vec3 v);

struct Vec3 vec3_normalize(struct Vec3 v) 
{
    float l = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x /= l; 
    v.y /=l;
    v.z /=l;
    return v;
}
/*
    scalar return
*/
float vec3_dot(struct Vec4 v1, struct Vec4 v2);

float vec3_dot(struct Vec4 v1, struct Vec4 v2) 
{
    float d;
    d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    return d;
}

float vec3_length(struct Vec3 v);

float vec3_length(struct Vec3 v) 
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}


/*
    diagnostic
*/
void vec3_print_dot(struct Vector v1, struct Vector v2);
void vec3_print_length(struct Vec3 v);



void vec3_print_length(struct Vec3 v) 
{
    float l = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("vector_length= %fs \n", l);
}


//diagnostic
void vec3_print_dot(struct Vector v1, struct Vector v2) 
{
    float d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}

void vec3_print_length(struct Vec3 v) 
{
    float l = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    printf("vector_length= %fs \n", l);
}