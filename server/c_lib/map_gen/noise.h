#pragma once

#include <stdlib.h>
#include <limits.h>

int seed_max = INT_MAX;

#include <c_lib/t_map/t_map.h>

typedef union f3 {
    float xyz[3];
    float x, y, z;
} float3;

typedef union f4 {
    float xyzw[4];
    float x, y, z, w;
} float4;

const float GRAD3[][3] = {
    {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0}, 
    {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1}, 
    {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
    {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}};

#define fastfloor(n) (int)(n) - (((n) < 0.0f) & ((n) != (int)(n)))


#define PERM_SIZE 256
// At the possible cost of unaligned access, we use char instead of
// int here to try to ensure that this table fits in L1 cache
//const unsigned char PERM[] = {
unsigned char PERM[] = {
  (unsigned char)151, (unsigned char)160, (unsigned char)137, (unsigned char)91, (unsigned char)90, (unsigned char)15, (unsigned char)131, (unsigned char)13, (unsigned char)201, (unsigned char)95, (unsigned char)96, (unsigned char)53, (unsigned char)194, (unsigned char)233, (unsigned char)7, (unsigned char)225, (unsigned char)140,
  (unsigned char)36, (unsigned char)103, (unsigned char)30, (unsigned char)69, (unsigned char)142, (unsigned char)8, (unsigned char)99, (unsigned char)37, (unsigned char)240, (unsigned char)21, (unsigned char)10, (unsigned char)23, (unsigned char)190, (unsigned char)6, (unsigned char)148, (unsigned char)247, (unsigned char)120,
  (unsigned char)234, (unsigned char)75, (unsigned char)0, (unsigned char)26, (unsigned char)197, (unsigned char)62, (unsigned char)94, (unsigned char)252, (unsigned char)219, (unsigned char)203, (unsigned char)117, (unsigned char)35, (unsigned char)11, (unsigned char)32, (unsigned char)57, (unsigned char)177, (unsigned char)33,
  (unsigned char)88, (unsigned char)237, (unsigned char)149, (unsigned char)56, (unsigned char)87, (unsigned char)174, (unsigned char)20, (unsigned char)125, (unsigned char)136, (unsigned char)171, (unsigned char)168, (unsigned char)68, (unsigned char)175, (unsigned char)74, (unsigned char)165, (unsigned char)71,
  (unsigned char)134, (unsigned char)139, (unsigned char)48, (unsigned char)27, (unsigned char)166, (unsigned char)77, (unsigned char)146, (unsigned char)158, (unsigned char)231, (unsigned char)83, (unsigned char)111, (unsigned char)229, (unsigned char)122, (unsigned char)60, (unsigned char)211, (unsigned char)133,
  (unsigned char)230, (unsigned char)220, (unsigned char)105, (unsigned char)92, (unsigned char)41, (unsigned char)55, (unsigned char)46, (unsigned char)245, (unsigned char)40, (unsigned char)244, (unsigned char)102, (unsigned char)143, (unsigned char)54, (unsigned char)65, (unsigned char)25, (unsigned char)63, (unsigned char)161,
  (unsigned char)1, (unsigned char)216, (unsigned char)80, (unsigned char)73, (unsigned char)209, (unsigned char)76, (unsigned char)132, (unsigned char)187, (unsigned char)208, (unsigned char)89, (unsigned char)18, (unsigned char)169, (unsigned char)200, (unsigned char)196, (unsigned char)135, (unsigned char)130,
  (unsigned char)116, (unsigned char)188, (unsigned char)159, (unsigned char)86, (unsigned char)164, (unsigned char)100, (unsigned char)109, (unsigned char)198, (unsigned char)173, (unsigned char)186, (unsigned char)3, (unsigned char)64, (unsigned char)52, (unsigned char)217, (unsigned char)226, (unsigned char)250,
  (unsigned char)124, (unsigned char)123, (unsigned char)5, (unsigned char)202, (unsigned char)38, (unsigned char)147, (unsigned char)118, (unsigned char)126, (unsigned char)255, (unsigned char)82, (unsigned char)85, (unsigned char)212, (unsigned char)207, (unsigned char)206, (unsigned char)59, (unsigned char)227,
  (unsigned char)47, (unsigned char)16, (unsigned char)58, (unsigned char)17, (unsigned char)182, (unsigned char)189, (unsigned char)28, (unsigned char)42, (unsigned char)223, (unsigned char)183, (unsigned char)170, (unsigned char)213, (unsigned char)119, (unsigned char)248, (unsigned char)152, (unsigned char)2, (unsigned char)44,
  (unsigned char)154, (unsigned char)163, (unsigned char)70, (unsigned char)221, (unsigned char)153, (unsigned char)101, (unsigned char)155, (unsigned char)167, (unsigned char)43, (unsigned char)172, (unsigned char)9, (unsigned char)129, (unsigned char)22, (unsigned char)39, (unsigned char)253, (unsigned char)19, (unsigned char)98,
  (unsigned char)108, (unsigned char)110, (unsigned char)79, (unsigned char)113, (unsigned char)224, (unsigned char)232, (unsigned char)178, (unsigned char)185, (unsigned char)112, (unsigned char)104, (unsigned char)218, (unsigned char)246, (unsigned char)97, (unsigned char)228, (unsigned char)251, (unsigned char)34,
  (unsigned char)242, (unsigned char)193, (unsigned char)238, (unsigned char)210, (unsigned char)144, (unsigned char)12, (unsigned char)191, (unsigned char)179, (unsigned char)162, (unsigned char)241, (unsigned char)81, (unsigned char)51, (unsigned char)145, (unsigned char)235, (unsigned char)249, (unsigned char)14,
  (unsigned char)239, (unsigned char)107, (unsigned char)49, (unsigned char)192, (unsigned char)214, (unsigned char)31, (unsigned char)181, (unsigned char)199, (unsigned char)106, (unsigned char)157, (unsigned char)184, (unsigned char)84, (unsigned char)204, (unsigned char)176, (unsigned char)115, (unsigned char)121,
  (unsigned char)50, (unsigned char)45, (unsigned char)127, (unsigned char)4, (unsigned char)150, (unsigned char)254, (unsigned char)138, (unsigned char)236, (unsigned char)205, (unsigned char)93, (unsigned char)222, (unsigned char)114, (unsigned char)67, (unsigned char)29, (unsigned char)24, (unsigned char)72, (unsigned char)243,
  (unsigned char)141, (unsigned char)128, (unsigned char)195, (unsigned char)78, (unsigned char)66, (unsigned char)215, (unsigned char)61, (unsigned char)156, (unsigned char)180, (unsigned char)151, (unsigned char)160, (unsigned char)137, (unsigned char)91, (unsigned char)90, (unsigned char)15, (unsigned char)131,
  (unsigned char)13, (unsigned char)201, (unsigned char)95, (unsigned char)96, (unsigned char)53, (unsigned char)194, (unsigned char)233, (unsigned char)7, (unsigned char)225, (unsigned char)140, (unsigned char)36, (unsigned char)103, (unsigned char)30, (unsigned char)69, (unsigned char)142, (unsigned char)8, (unsigned char)99, (unsigned char)37,
  (unsigned char)240, (unsigned char)21, (unsigned char)10, (unsigned char)23, (unsigned char)190, (unsigned char)6, (unsigned char)148, (unsigned char)247, (unsigned char)120, (unsigned char)234, (unsigned char)75, (unsigned char)0, (unsigned char)26, (unsigned char)197, (unsigned char)62, (unsigned char)94, (unsigned char)252,
  (unsigned char)219, (unsigned char)203, (unsigned char)117, (unsigned char)35, (unsigned char)11, (unsigned char)32, (unsigned char)57, (unsigned char)177, (unsigned char)33, (unsigned char)88, (unsigned char)237, (unsigned char)149, (unsigned char)56, (unsigned char)87, (unsigned char)174, (unsigned char)20, (unsigned char)125,
  (unsigned char)136, (unsigned char)171, (unsigned char)168, (unsigned char)68, (unsigned char)175, (unsigned char)74, (unsigned char)165, (unsigned char)71, (unsigned char)134, (unsigned char)139, (unsigned char)48, (unsigned char)27, (unsigned char)166, (unsigned char)77, (unsigned char)146, (unsigned char)158,
  (unsigned char)231, (unsigned char)83, (unsigned char)111, (unsigned char)229, (unsigned char)122, (unsigned char)60, (unsigned char)211, (unsigned char)133, (unsigned char)230, (unsigned char)220, (unsigned char)105, (unsigned char)92, (unsigned char)41, (unsigned char)55, (unsigned char)46, (unsigned char)245,
  (unsigned char)40, (unsigned char)244, (unsigned char)102, (unsigned char)143, (unsigned char)54, (unsigned char)65, (unsigned char)25, (unsigned char)63, (unsigned char)161, (unsigned char)1, (unsigned char)216, (unsigned char)80, (unsigned char)73, (unsigned char)209, (unsigned char)76, (unsigned char)132, (unsigned char)187,
  (unsigned char)208, (unsigned char)89, (unsigned char)18, (unsigned char)169, (unsigned char)200, (unsigned char)196, (unsigned char)135, (unsigned char)130, (unsigned char)116, (unsigned char)188, (unsigned char)159, (unsigned char)86, (unsigned char)164, (unsigned char)100, (unsigned char)109, (unsigned char)198,
  (unsigned char)173, (unsigned char)186, (unsigned char)3, (unsigned char)64, (unsigned char)52, (unsigned char)217, (unsigned char)226, (unsigned char)250, (unsigned char)124, (unsigned char)123, (unsigned char)5, (unsigned char)202, (unsigned char)38, (unsigned char)147, (unsigned char)118, (unsigned char)126,
  (unsigned char)255, (unsigned char)82, (unsigned char)85, (unsigned char)212, (unsigned char)207, (unsigned char)206, (unsigned char)59, (unsigned char)227, (unsigned char)47, (unsigned char)16, (unsigned char)58, (unsigned char)17, (unsigned char)182, (unsigned char)189, (unsigned char)28, (unsigned char)42, (unsigned char)223,
  (unsigned char)183, (unsigned char)170, (unsigned char)213, (unsigned char)119, (unsigned char)248, (unsigned char)152, (unsigned char)2, (unsigned char)44, (unsigned char)154, (unsigned char)163, (unsigned char)70, (unsigned char)221, (unsigned char)153, (unsigned char)101, (unsigned char)155, (unsigned char)167,
  (unsigned char)43, (unsigned char)172, (unsigned char)9, (unsigned char)129, (unsigned char)22, (unsigned char)39, (unsigned char)253, (unsigned char)19, (unsigned char)98, (unsigned char)108, (unsigned char)110, (unsigned char)79, (unsigned char)113, (unsigned char)224, (unsigned char)232, (unsigned char)178, (unsigned char)185,
  (unsigned char)112, (unsigned char)104, (unsigned char)218, (unsigned char)246, (unsigned char)97, (unsigned char)228, (unsigned char)251, (unsigned char)34, (unsigned char)242, (unsigned char)193, (unsigned char)238, (unsigned char)210, (unsigned char)144, (unsigned char)12, (unsigned char)191, (unsigned char)179,
  (unsigned char)162, (unsigned char)241, (unsigned char)81, (unsigned char)51, (unsigned char)145, (unsigned char)235, (unsigned char)249, (unsigned char)14, (unsigned char)239, (unsigned char)107, (unsigned char)49, (unsigned char)192, (unsigned char)214, (unsigned char)31, (unsigned char)181, (unsigned char)199,
  (unsigned char)106, (unsigned char)157, (unsigned char)184, (unsigned char)84, (unsigned char)204, (unsigned char)176, (unsigned char)115, (unsigned char)121, (unsigned char)50, (unsigned char)45, (unsigned char)127, (unsigned char)4, (unsigned char)150, (unsigned char)254, (unsigned char)138, (unsigned char)236,
  (unsigned char)205, (unsigned char)93, (unsigned char)222, (unsigned char)114, (unsigned char)67, (unsigned char)29, (unsigned char)24, (unsigned char)72, (unsigned char)243, (unsigned char)141, (unsigned char)128, (unsigned char)195, (unsigned char)78, (unsigned char)66, (unsigned char)215, (unsigned char)61, (unsigned char)156,
  (unsigned char)180
};

//float noisemap[xmax*ymax*zmax];
float* noisemap = NULL;
int noisemap_inited = 0;
float* noise_init(int x, int y, int z);
void noise_destroy();

int _base_seed = 1;
int _set_base_seed = 0;
int _seed = 1;
int seed_noise(int seed);
int next_seed();
int set_seed_grp(int grp);

void set_terrain_height(int x, int y, int z, int baseline, int maxheight, int tile);
void set_terrain_density(int x, int y, int z, float threshold, int tile);
void clear_noisemap();
void invert_map(int x, int y, int z, int tile);

int _oct = 1;
float _per = 0.6f;
float _amp = 1.0f;
float _lac = 2.0f;
float _freq = 1.0f;

float xnoise_scale = 1.0f;
float ynoise_scale = 1.0f;
float znoise_scale = 1.0f;

void set_noise_parameters(int octaves, float persistence, float amplitude, float lacunarity, float frequency);
void set_noise_scale(float xscale, float yscale, float zscale);
