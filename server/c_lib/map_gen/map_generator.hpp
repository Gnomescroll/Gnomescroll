#pragma once

#include <options.hpp>
#include <t_map/t_map.hpp>
#include <map_gen/noise.h>
#include <map_gen/perlin.h>
#include <map_gen/simplex.h>
#include <map_gen/ridged_mf.h>
#include <map_gen/interpolator.h>
#include <map_gen/perturb.h>
#include <map_gen/gradient.h>
#include <map_gen/features.h>

namespace MapGen
{

typedef enum
{
    NOISE_NONE,
    NOISE_PERLIN,
    NOISE_SIMPLEX,
    //NOISE_RMF,
} NoiseType;

class Generator
{
    public:
    
    static int seed_value;
    static int seed_group;
    
    int x,y,z;
    int base_tile;

    bool use_interpolate;
    int interpolate_x,interpolate_y,interpolate_z;

    bool use_gradient;
    float gx0,gx1,gy0,gy1,gz0,gz1;
    bool use_gradient2;
    float gx,gy,gz;

    int octaves;
    float persistence;
    float frequency;
    float amplitude;
    float lacunarity;
    int repeat_x, repeat_y, repeat_z;

    int dimensions;
    NoiseType noise_type;

    float density_threshold;

    bool use_rmf;

    bool use_heightmap;
    bool use_reverse_heightmap;

    int baseline;
    int maxheight;
    int reverse_baseline;
    int reverse_maxheight;
    int reverse_minheight;

    // this is the base tile to stack a heightmap on top of
    bool use_heightmap_tile;
    int heightmap_tile;
    
    bool use_density;
    
    bool use_perturb;
    float turbulence;
    int perturb_height_clamp;

    bool add_grass;
    int dirt_tile;
    int grass_tile;

    float xscale, yscale, zscale;

    void repeat(int x, int y, int z)
    {
        this->repeat_x = x;
        this->repeat_y = y;
        this->repeat_z = z;
    }

    void size(int x, int y, int z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void max_size() // use largest size available
    {
        this->x = map_dim.x;
        this->y = map_dim.y;
        this->z = map_dim.z;
    }

    void tile(int tile)
    {
        this->base_tile = tile;
    }

    void tile(char* name);

    void interpolate(int x, int y, int z)
    {
        this->use_interpolate = true;
        this->interpolate_x = x;
        this->interpolate_y = y;
        this->interpolate_z = z;
    }

    void enable_heightmap(int baseline, int maxheight)
    {
        this->use_heightmap = true;
        if (baseline  + maxheight > map_dim.z)
        {
            printf("MapGen heightmap settings - baseline + maxheight exceeds map height. Scaling down\n");
            float scale = ((float)map_dim.z) / ((float)(baseline+maxheight));
            baseline = (int)(((float)baseline)*scale);
            maxheight = (int)(((float)maxheight)*scale);
            int i=0;
            while (baseline + maxheight < map_dim.z)
            {
                if (i) baseline += 1;
                else maxheight += 1;
                i++;
                i %= 2;
            }
        }

        this->baseline = baseline;
        this->maxheight = maxheight;
    }
    
    void enable_heightmap(int baseline, int maxheight, int tile)
    {
        this->enable_heightmap(baseline, maxheight);
        //this->heightmap_tile = tile;
        //this->use_heightmap_tile = true;
    }

    void enable_reverse_heightmap(int baseline, int  maxheight, int minheight, int tile)
    {
        this->use_reverse_heightmap = true;
        if (baseline + maxheight > map_dim.z)
            printf("WARNING: MapGen reverse heightmap -- baseline + maxheight exceeds map height\n");
        if (baseline - minheight < 0)
            printf("WARNING: MapGen reverse heightmap -- baseline - minheight is negative\n");

        this->reverse_baseline = baseline;
        this->reverse_maxheight = maxheight;
        this->reverse_minheight = minheight;
        //this->heightmap_tile = tile;
        //this->use_heightmap_tile = true;
    }

    void density(float threshold)
    {
        this->use_density = true;
        this->density_threshold = threshold;
    }

    void gradient(float x0, float x1, float y0, float y1, float z0, float z1)
    {
        this->use_gradient = true;
        this->gx0 = x0;
        this->gx1 = x1;
        this->gy0 = y0;
        this->gy1 = y1;
        this->gz0 = z0;
        this->gz1 = z1;
    }

    void gradient2(float x, float y, float z)
    {
        this->use_gradient2 = true;
        this->gx = x;
        this->gy = y;
        this->gz = z;
    }

    void set_noise_parameters(int octaves, float persistence, float amplitude, float frequency, float lacunarity)
    {
        this->octaves = octaves;
        this->persistence = persistence;
        this->amplitude = amplitude;
        this->frequency = frequency;
        this->lacunarity = lacunarity;
    }

    void p1()
    {
        this->dimensions = 1;
        this->noise_type = NOISE_PERLIN;
    }
    
    void p2()
    {
        this->dimensions = 2;
        this->noise_type = NOISE_PERLIN;
    }

    void p3()
    {
        this->dimensions = 3;
        this->noise_type = NOISE_PERLIN;
    }

    void s1()
    {
        this->dimensions = 1;
        this->noise_type = NOISE_SIMPLEX;
    }
    
    void s2()
    {
        this->dimensions = 2;
        this->noise_type = NOISE_SIMPLEX;
    }

    void s3()
    {
        this->dimensions = 3;
        this->noise_type = NOISE_SIMPLEX;
    }

    void rmf()
    {
        this->use_rmf = true;
    }

    void scale(float x, float y, float z)
    {
        this->xscale = x;
        this->yscale = y;
        this->zscale = z;
    }

    void seed(unsigned int s)
    {
        Generator::seed_value = s;
        Options::seed = s;
    }

    void group(int g)
    {
        Generator::seed_group = g;
        set_seed_grp(g);
    }

    void enable_grass()
    {
        this->add_grass = true;
    }

    void perturb(float turbulence, int height_clamp)
    {
        this->use_perturb = true;
        this->turbulence = turbulence;
        this->perturb_height_clamp = height_clamp;
    }

    void start()
    {
        long t = _GET_MS_TIME();

        static int inited_noise = 0;
        if (!(inited_noise++)) noise_init(this->x, this->y, this->z);

        set_noise_parameters(octaves, persistence, amplitude, lacunarity, frequency);
        set_noise_scale(xscale, yscale, zscale);

        if (this->use_rmf)
        {
            if (this->noise_type == NOISE_PERLIN)
            {
                switch (this->dimensions)
                {
                    case 1:
                        rmf_perlin1_fill(x, repeat_x, this->base_tile);
                        break;
                    case 2:
                        rmf_perlin2_fill(x,y, repeat_x, repeat_y, this->base_tile);
                        break;
                    case 3:
                        rmf_perlin3_fill(x,y,z, repeat_x, repeat_y, repeat_z, this->base_tile);
                        break;
                    default:
                        printf("RMF perlin dimension %d not implemented\n", this->dimensions);
                        break;
                }
            }
            else if (this->noise_type == NOISE_SIMPLEX)
            {
                switch (this->dimensions)
                {
                    case 2:
                        rmf_simplex2_fill(x,y);
                        break;
                    case 3:
                        rmf_simplex3_fill(x,y,z);
                        break;
                    default:
                        printf("RMF Simplex dimension %d not implemented\n", this->dimensions);
                        break;
                }            
            }
        }
        else if (this->noise_type == NOISE_PERLIN)
        {
            switch (this->dimensions)
            {
                case 1:
                    perlin1_fill(x, repeat_x, this->base_tile);
                    break;
                case 2:
                    perlin2_fill(x,y, repeat_x, repeat_y, this->base_tile);
                    break;
                case 3:
                    perlin3_fill(x,y,z, repeat_x, repeat_y, repeat_z, this->base_tile);
                    break;
                default:
                    printf("Perlin dimension %d not implemented\n", this->dimensions);
                    break;
            }            
        }
        else if (this->noise_type == NOISE_SIMPLEX)
        {
            switch (this->dimensions)
            {
                case 2:
                    simplex2_fill(x,y);
                    break;
                case 3:
                    simplex3_fill(x,y,z);
                    break;
                default:
                    printf("Simplex dimension %d not implemented\n", this->dimensions);
                    break;
            }
        }
        else printf("WARNING: Generator::start() -- unknown noise_type %d\n", this->noise_type);

        // call assigned noise function
        // call interpolation
        if (this->use_interpolate)
        {
            if (this->use_rmf)
            {
                if (this->noise_type == NOISE_PERLIN)
                {
                    switch (this->dimensions)
                    {
                        case 1:
                            apply_interp1_rmf_perlin(x, interpolate_x, repeat_x, this->base_tile);
                            break;
                        case 2:
                            apply_interp2_rmf_perlin(x,y, interpolate_x, interpolate_y, repeat_x, repeat_y, this->base_tile);
                            break;
                        case 3:
                            apply_interp3_rmf_perlin(x,y,z, interpolate_x, interpolate_y, interpolate_z, repeat_x, repeat_y, repeat_z, this->base_tile);
                            break;
                        default:
                            printf("RMF perlin dimension %d not implemented\n", this->dimensions);
                            break;
                    }
                }
                else if (this->noise_type == NOISE_SIMPLEX)
                {
                    switch (this->dimensions)
                    {
                        case 2:
                            apply_interp2_rmf_simplex(x,y, interpolate_x, interpolate_y);
                            break;
                        case 3:
                            apply_interp3_rmf_simplex(x,y,z, interpolate_x, interpolate_y, interpolate_z);
                            break;
                        default:
                            printf("RMF Simplex dimension %d not implemented\n", this->dimensions);
                            break;
                    }            
                }
            }
            else if (this->noise_type == NOISE_PERLIN)
            {
                switch (this->dimensions)
                {
                    case 1:
                        apply_interp1_perlin(x, interpolate_x, repeat_x, this->base_tile);
                        break;
                    case 2:
                        apply_interp2_perlin(x,y, interpolate_x, interpolate_y, repeat_x, repeat_y, this->base_tile);
                        break;
                    case 3:
                        apply_interp3_perlin(x,y,z, interpolate_x, interpolate_y, interpolate_z, repeat_x, repeat_y, repeat_z, this->base_tile);
                        break;
                    default:
                        printf("Perlin dimension %d not implemented\n", this->dimensions);
                        break;
                }            
            }
            else if (this->noise_type == NOISE_SIMPLEX)
            {
                switch (this->dimensions)
                {
                    case 2:
                        apply_interp2_simplex(x,y, interpolate_x, interpolate_y);
                        break;
                    case 3:
                        apply_interp3_simplex(x,y,z, interpolate_x, interpolate_y, interpolate_z);
                        break;
                    default:
                        printf("Simplex dimension %d not implemented\n", this->dimensions);
                        break;
                }
            }
            else printf("WARNING: Generator::start() -- unknown noise_type %d\n", this->noise_type);
        }

        if (this->use_gradient2)
            apply_grad3_falloff(x,y,z, gx,gy,gz);
        else if (this->use_gradient)
            apply_grad3(x,y,z, gx0,gy0,gz0, gx1,gy1,gz1);

        if (this->use_density)
            set_terrain_density(x,y,z, this->density_threshold, this->base_tile);
        else if (this->use_heightmap)
        {
            set_terrain_height(x,y,z, this->baseline, this->maxheight, this->base_tile);
            if (this->use_perturb)
                perturb_heightmap(x,y, this->turbulence, this->base_tile, this->perturb_height_clamp);
        }

        if (this->use_reverse_heightmap)
            reverse_heightmap(x,y,z, this->reverse_baseline, this->reverse_maxheight, this->reverse_minheight, this->base_tile);

        if (this->add_grass)
            _grass(x,y, this->base_tile, this->dirt_tile, this->grass_tile);

        next_seed();

        float elapsed = ((float)(_GET_MS_TIME() - t)) / 1000.0f;
        printf("Took %0.2f seconds\n", elapsed);
    }

    ~Generator()
    {
        noise_destroy();
    }
    
    Generator()
    :
    x(0), y(0), z(0),
    base_tile(2),
    use_interpolate(false), interpolate_x(1), interpolate_y(1), interpolate_z(1),
    use_gradient(false), gx0(0.0f), gx1(0.0f), gy0(0.0f), gy1(0.0f), gz0(0.0f), gz1(0.0f),
    use_gradient2(false), gx(0.0f), gy(0.0f), gz(0.0),
    octaves(1), persistence(0.5f), frequency(1.0f), amplitude(1.0f), lacunarity(2.0f),
    repeat_x(1024), repeat_y(1024), repeat_z(1024),
    dimensions(2),
    noise_type(NOISE_PERLIN),
    density_threshold(0.5f),
    use_rmf(false),
    use_heightmap(false), use_reverse_heightmap(false),
    baseline(0), maxheight(0), reverse_baseline(0), reverse_maxheight(0), reverse_minheight(0),
    use_heightmap_tile(true), heightmap_tile(0),
    use_density(false),
    use_perturb(false), turbulence(1.0f), perturb_height_clamp(1),
    add_grass(false),
    dirt_tile(0),
    grass_tile(0),
    xscale(1.0f), yscale(1.0f), zscale(1.0f)
    {
        set_noise_parameters(octaves, persistence, amplitude, lacunarity, frequency);
        set_noise_scale(xscale, yscale, zscale);
    }

};

int Generator::seed_value = 666;
int Generator::seed_group = 0;


/* Interface */

Generator* generator = NULL;

void init()
{
    if (generator == NULL) generator = new Generator;
}

void reset()
{
    if (generator != NULL) delete generator;
    generator = new Generator;
}

void teardown()
{
    if (generator != NULL) delete generator;
}

} // MapGen
