#include "dragon.hpp"

#include <t_map/t_map.hpp>
#include <map_gen/features.hpp>
#include <math.h>
//#include <common/random.h>

#include "hopcroft-karp.cpp"

namespace Dragon
{

struct Point
{
    double x,y;
};

struct iPoint
{
    int x,y;
};

struct Location
{
    int x,y,z;
};

class L_System
{
    private:
        int char_in_string(char c, char* str)
        {
            int i;
            for (i=0; str[i] != '\0'; i++)
                if (str[i] == c)
                    return i;
            return -1;
        }

        void set_n_rules(int n)
        {
            this->n_rules = n;
            this->rules = (char**)calloc(n, sizeof(char*));
            this->rule_lengths = (int*)calloc(n, sizeof(int));
        }

    public:
        char* visited_tiles;
    
        int z_min;
        int z_max;
        int tile;

        int width,height;

        int n_tokens;
        char* tokens;

        char* start;
        int n_rules;
        char** rules;
        int* rule_lengths;

        int iterations;

        // save initial state
        double ogx,ogy;
        double ogdegrees;

        double x,y;
        double degrees;
        double dx,dy;
        double xstep,ystep;

        struct Point* points;
        int n_points;

        double p;
        void set_probability(double p)
        {
            this->p = p;
        }

        double draw_width;
        void set_draw_width(double draw_width)
        {
            this->draw_width = draw_width;
        }

        void reset_state()
        {
            x = ogx;
            y = ogy;
            degrees = ogdegrees;
        }
        
        void turn_left()
        {
            degrees += 90;
            compute_delta(degrees);
        };

        void turn_right()
        {
            degrees -= 90;
            compute_delta(degrees);
        }

        void turn_left_p()
        {
            if (randf() < this->p) turn_left();
        };

        void turn_right_p()
        {
            if (randf() < this->p) turn_right();
        }

        void forward()
        {
            double _x = x;
            double _y = y;
            _x += dx*xstep;
            _y += dy*ystep;
            //trace_set_map(x, y, _x, _y);
            x = _x;
            y = _y;

            this->add_node(x,y);
        }

        void forward_p()
        {
            if (randf() < this->p) forward();
        }

        void add_node(double x, double y)
        {
            static int points_size = 1024;
            if (this->points == NULL)
                this->points = (struct Point*)malloc(points_size * sizeof(struct Point));
            if (this->n_points >= points_size)
            {
                points_size += 1024;
                this->points = (struct Point*)realloc(this->points, points_size * sizeof(struct Point));
            }

            this->points[this->n_points].x = x;
            this->points[this->n_points].y = y;
            this->n_points++;
        }

        inline void rotate90(double* x, double* y)
        {
            double _x = *x;
            *x = -*y;
            *y = _x;
        }

        //void trace_set_map(double x, double y, double x1, double y1) {
            //// step along path to set map
            //double step = 0.1;
            //double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            //int steps = ceil(len/step);
            //step = len/steps;   // resize
            //double xstep = step * ((x1-x)/len);
            //double ystep = step * ((y1-y)/len);
            //double mx=x,my=y;

            //double wstep = 0.1;
            //int wsteps = ceil(draw_width/wstep);
            //wstep = draw_width / wsteps; // resize
            //double wx=x,wy=y,wx1=x1,wy1=y1;
            //rotate90(&wx, &wy);
            //rotate90(&wx1, &wy1);
            //double wxstep = wstep * ((wx1-wx)/len);
            //double wystep = wstep * ((wy1-wy)/len);
            //mx -= wxstep*((wsteps+1)/2);
            //my -= wystep*((wsteps+1)/2);

            //const int initial_h = 8;

            //int i,j,k;
            //int ix,iy;
            //double mmx,mmy;
            //mmx=mx;mmy=my;
            //for (j=0; j<wsteps; j++)
            //{
                //mmx = mx;
                //mmy = my;
                //for (i=0; i<steps; i++)
                //{
                    //mmx += xstep;
                    //mmy += ystep;
                    //ix = (int)mmx;
                    //iy = (int)mmy;
                    //if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                    //if (visited_tiles[ix + map_dim.x * iy]) continue;
                    //visited_tiles[ix + map_dim.x*iy] = 1;
                    ////for (k=this->z_min; k<this->z_max+1;k++) {
                        ////t_map::set(ix, iy, k, this->tile);
                    ////}
                    //int dh = distancef(x,y,ogx,ogy) / 6;
                    //dh = initial_h - dh;
                    //int start_h = t_map::get_highest_solid_block(ix,iy);
                    ////if (dh < 0)
                        ////for (k=dh; k<=0; k++)
                            ////t_map::set(ix,iy,start_h+k, 0);
                    ////else
                    //if (dh > 0)
                        //for (k=1; k<=dh; k++)
                            //t_map::set(ix,iy,start_h+k,this->tile);
                    ////printf("%d\n", dh);
                //}
                //mx += wxstep;
                //my += wystep;
            //}
        //}
        
        //void trace_set_map(double x, double y, double x1, double y1) {
            //// step along path to set map
            //double step = 0.1;
            //double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            //int steps = ceil(len/step);
            //step = len/steps;   // resize
            //double xstep = step * ((x1-x)/len);
            //double ystep = step * ((y1-y)/len);
            //int i;
            //int ix,iy;
            //double mx=x,my=y;

            //double wstep = 0.1;
            //int wsteps = ceil(draw_width/wstep);
            //wstep = draw_width / wsteps; // resize
            //double wx=x,wy=y,wx1=x1,wy1=y1;
            //rotate90(&wx, &wy);
            //rotate90(&wx1, &wy1);
            //double wxstep = wstep * ((wx1-wx)/len);
            //double wystep = wstep * ((wy1-wy)/len);
            //mx -= wxstep*((wsteps+1)/2);
            //my -= wystep*((wsteps+1)/2);

            //int j;
            //double mmx,mmy;
            //mmx=mx;mmy=my;
            //for (j=0; j<wsteps; j++) {
                //mmx = mx;
                //mmy = my;
                //for (i=0; i<steps; i++) {
                    //mmx += i*xstep;
                    //mmy += i*ystep;
                    //ix = (int)mmx;
                    //iy = (int)mmy;
                    //if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                    //_set(ix, iy, this->z, this->tile);
                //}
                //mx += wxstep;
                //my += wystep;
            //}
        //}

        //void trace_set_map(double x, double y, double x1, double y1) {
            //// step along path to set map
            //double step = 0.1;
            //double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            //int steps = ceil(len/step);
            //step = len/steps;   // resize
            //double xstep = step * ((x1-x)/len);
            //double ystep = step * ((y1-y)/len);
            //int i;
            //int ix,iy;
            //double mx=x,my=y;

            //for (i=0; i<steps; i++) {
                //mx += xstep;
                //my += ystep;
                //ix = (int)mx;
                //iy = (int)my;
                //if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                //_set(ix, iy, this->z, this->tile);
            //}
        //}

        void compute_delta(double degrees)
        {
            const double pi = 3.14159;
            double radians = degrees * (pi / 180);
            dx = cos(radians);
            dy = sin(radians);
        }

        void set_starting_position(double x, double y)
        {
            this->x = x;
            this->y = y;
            this->ogx = x;
            this->ogy = y;
        }

        void set_starting_degrees(double degrees)
        {
            compute_delta(degrees);
        }

        void set_dimensions(int w, int h)
        {
            this->width = w;
            this->height = h;
        }

        void set_degrees(double degrees)
        {
            this->degrees = degrees;
            this->ogdegrees = degrees;
        }

        void set_step_size(double step)
        {
            this->xstep = step;
            this->ystep = step;
        }
        
        void set_step_size(double xstep, double ystep)
        {
            this->xstep = xstep;
            this->ystep = ystep;
        }

        void set_iterations(int i)
        {
            this->iterations = i;
        }

        void set_map_tile(int tile)
        {
            this->tile = tile;
        }

        void set_map_z_level(int z_min, int z_max)
        {
            this->z_min = z_min;
            this->z_max = z_max;
        }
        void set_map_z_level(int z)
        {
            this->z_min = z;
            this->z_max = z;
        }

        void set_init_condition(char* cond)
        {
            this->start = (char*)malloc((strlen(cond)+1) * sizeof(char));
            strcpy(this->start, cond);
        }

        void set_tokens(char* tokens)
        {
            this->tokens = (char*)malloc((strlen(tokens)+1) * sizeof(char));
            this->n_tokens = strlen(tokens);
            strcpy(this->tokens, tokens);
            set_n_rules(strlen(this->tokens));
        }

        char* parse_rule(char* rule, int* token_id)
        {
            // get token id
            char token = rule[0];
            *token_id = char_in_string(token, this->tokens);
            assert(*token_id >= 0 && *token_id < this->n_tokens);
            
            // calculate size of rule
            int rule_length=0;
            int start_index = -1;
            for (int i=1; rule[i] != '\0'; i++) // skip first token; A -> B
            {
                if (char_in_string(rule[i], this->tokens) < 0) continue;
                if (start_index < 0) start_index = i;
                rule_length++;
            }
            assert(start_index >= 0);
            assert(rule_length > 0);

            // copy rule
            char* parsed = (char*)malloc((rule_length+1) * sizeof(char));
            assert(parsed != NULL);
            for (int i=0; i<rule_length; i++)
                parsed[i] = rule[start_index+i];
            parsed[rule_length] = '\0';
            return parsed;
        }

        void add_rule(char* rule)
        {
            int token_id;
            char* new_rule = parse_rule(rule, &token_id);
            assert(new_rule != NULL);
            assert(token_id < n_rules && token_id >= 0);
            assert(this->rules[token_id] == NULL);
            assert(this->rule_lengths[token_id] == 0);
            this->rules[token_id] = new_rule;
            this->rule_lengths[token_id] = strlen(new_rule);    // Invalid read of size 4
        }

        //returns length of next rule
        int apply_rule(char* rule)
        {
            int i;
            int len=0;
            int token_id;
            char token;
            for (i=0; rule[i] != '\0'; i++)
            {
                token = rule[i];
                token_id = char_in_string(token, this->tokens);
                if (token_id < 0)
                {
                    printf("apply_rule WARNING unrecognized token %c. should not occur.\n", token);
                    continue;
                }
                switch(token)
                {
                    case '+':
                        turn_right();
                        break;
                    case ']':
                        turn_right_p();
                        break;
                    case '-':
                        turn_left();
                        break;
                    case '[':
                        turn_left_p();
                        break;
                    case 'F':
                        forward();
                        break;
                    case 'f':
                        forward_p();
                        break;
                    default:
                        break;
                }
                len += rule_lengths[token_id];
            }
            return len;
        }

        void get_next_rule(char* rule, char* next)
        {
            int i,j;
            int rule_len;
            int next_index = 0;
            char token;
            int token_id;
            char* insert_rule;
            for (i=0; rule[i] != '\0'; i++)
            {
                token = rule[i];
                token_id = char_in_string(token, this->tokens);
                if (token_id < 0)
                {
                    printf("get_next_rule WARNING unrecognized token %c. should not occur\n", token);
                    continue;
                }
                rule_len = rule_lengths[token_id];
                insert_rule = rules[token_id];
                for (j=0; j<rule_len; j++)
                {
                    next[next_index] = insert_rule[j];
                    next_index++;
                }
            }
            next[next_index] = '\0';
        }

        void run()
        {
            char* curr = start;
            char* next;
            int next_len;
            next_len = apply_rule(curr);
            next = (char*)malloc((next_len+1) * sizeof(char));
            get_next_rule(curr, next);
            curr = next;
            int i;
            for (i=1; i<iterations-1; i++)
            {
                next_len = apply_rule(curr);
                next = (char*)malloc((next_len+1) * sizeof(char));
                get_next_rule(curr, next);
                free(curr);
                curr = next;
            }
            next_len = apply_rule(curr);
            free(curr);
        }

    L_System() :
    n_rules(0), rules(NULL), rule_lengths(NULL),
    iterations(0),
    points(NULL), n_points(0),
    p(1.0),draw_width(0.1)
    {
        visited_tiles = (char*)calloc(map_dim.x*map_dim.y, sizeof(char));
    }
    
    ~L_System()
    {
        if (start != NULL) free(start);
        if (rules != NULL)
        {
            for (int i=0; i<n_rules; i++)
                if (rules[i] != NULL)
                    free(rules[i]);
            free(rules);
        }
        if (rule_lengths != NULL) free(rule_lengths);
        if (tokens != NULL) free(tokens);
        if (visited_tiles != NULL) free(visited_tiles);
        if (points != NULL) free(points);
    }
};

void init(int z)
{

    L_System* L = new L_System();

    int tile;
    tile = 0;
    tile = 101;
    //int z_min = 13;
    //int z_max = 13;
    char tokens[]= "XYF+-B[]f";
    int iterations = 20;
    double degrees = 90;
    //double degrees = 60;
    double step = 4;
    double xstep = 1;
    double ystep = 1;
    L->set_tokens(tokens);
    L->set_iterations(iterations);
    L->set_degrees(degrees);
    L->set_step_size(step);
    L->set_step_size(xstep, ystep);
    L->set_starting_degrees(0);
    //L->set_starting_position(width/2+0.5,height/2+0.5);
    L->set_starting_position(64.5,64.5);
    L->set_map_tile(tile);
    //L->set_map_z_level(z_min, z_max);
    L->set_map_z_level(z,z);
    L->set_dimensions(width,height);
    L->set_draw_width(.1);
    L->set_probability(0.7);

    // rules
    L->add_rule((char*) "B > B");
    L->add_rule((char*) "X > X]YF");
    L->add_rule((char*) "Y > FX[Y");
    //L->add_rule((char*) "X > X+YF");
    //L->add_rule((char*) "Y > FX-Y");
    L->add_rule((char*) "F > F");
    //L->add_rule((char*) "+ > ]");
    //L->add_rule((char*) "- > [");
    //L->add_rule((char*) "] > +");
    //L->add_rule((char*) "[ > -");
    L->add_rule((char*) "+ > +");
    L->add_rule((char*) "- > -");
    L->add_rule((char*) "] > ]");
    L->add_rule((char*) "[ > [");
    L->add_rule((char*) "f > f");

    // start condition
    L->set_init_condition((char*) "FX");

    L->run();

    //L->reset_state();
    ////L->set_starting_degrees(90);
    //L->set_step_size(15,7);
    //L->run();

    delete L;
}

L_System* _caves()
{
    L_System* L = new L_System;

    int tile = 50;
    int z_min = 10;
    int z_max = 64;
    char tokens[]= "XYF+-B[]f";
    //int iterations = 20;
    int iterations = 9;
    double degrees = 90;
    double step = 1;
    //double xstep = randrange(1,2);
    //double ystep = randrange(1,2);
    double xstep = 1;
    double ystep = 1;
    L->set_tokens(tokens);
    L->set_iterations(iterations);
    L->set_degrees(degrees);
    L->set_step_size(step);
    L->set_step_size(xstep, ystep);
    L->set_starting_degrees(0);
    float start_x = randf() * map_dim.x;
    float start_y = randf() * map_dim.y;
    L->set_starting_position(start_x, start_y);
    L->set_map_tile(tile);
    L->set_map_z_level(z_min, z_max);
    L->set_dimensions(map_dim.x,map_dim.y);
    L->set_draw_width(.1);
    L->tile = tile;
    float p = randf();
    p /= 5.0f;
    p += 0.5f;
    L->set_probability(p);

    // rules
    L->add_rule((char*) "B > B");
    L->add_rule((char*) "X > X]YF");
    L->add_rule((char*) "Y > FX[Y");
    L->add_rule((char*) "F > F");
    //L->add_rule((char*) "+ > ]");
    //L->add_rule((char*) "- > [");
    //L->add_rule((char*) "] > +");
    //L->add_rule((char*) "[ > -");
    L->add_rule((char*) "+ > +");
    L->add_rule((char*) "- > -");
    L->add_rule((char*) "] > ]");
    L->add_rule((char*) "[ > [");
    L->add_rule((char*) "f > f");

    // start condition
    L->set_init_condition((char*) "FX");

    L->run();

    return L;
}

void caves()
{
    int n = 20;
    for (int i=0; i<n; i++)
        delete _caves();
}

void generate_caves() {
    int x = width;
    int y = height;
    int z = 0;
    int h = 64;
    int tile = 3;
    map_gen::floor(x,y, z, h, tile);
    caves();
    map_gen::box(x,y,z,h,tile);
}

bool block_is_edge(int i, int j, int k, int limit) {
    int tile = _get(i,j,k);
    int ct = 0;

    if (_get(i+1, j, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i-1, j, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i, j+1, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i, j-1, k) != tile) ct++;
    if (ct == limit) return true;

    if (_get(i+1, j+1, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i+1, j-1, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i-1, j-1, k) != tile) ct++;
    if (ct == limit) return true;
    if (_get(i-1, j+1, k) != tile) ct++;
    if (ct == limit) return true;
    
    return false;
}

void segment_caves(int z, int tile, int limit) {
    int i,j;
    Location* blocks = (Location*)malloc(sizeof(Location)*width*height);
    int index = 0;
    // check for a cave block
    // look at all 8 surrounding blocks
    // if >1 is non cave, convert to orange (blocked)

    Location* loc = NULL;
    int new_tile = 7; // lava
    
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            if (_get(i,j,z) != tile) continue;
            if (block_is_edge(i,j,z, limit)) {
                loc = &blocks[index];
                loc->x = i;
                loc->y = j;
                loc->z = z;
                index++;
            }
        }
    }

    for (i=0; i<index; i++) {
        loc = &blocks[i];
        _set(loc->x, loc->y, loc->z, new_tile);
    }

    free(blocks);
}

void generate(int z)
{
    int x = width;
    int y = height;
    int tile = 3;
    map_gen::floor(x,y, z, 1, tile);
    init(z);

    segment_caves(z, 101, 2);
    segment_caves(z, 101, 7);
    segment_caves(z, 101, 8);

    // post processing for good visual of cave isolation
    int i,j;
    // convert blue_tron to solar panel (inner caves)
    for (i=0;i<width;i++) {
        for (j=0;j<height;j++) {
            if (_get(i,j,z) == 101) _set(i,j,z, 103);
        }
    }
    // convert lava (outline) to blue_tron
    for (i=0;i<width;i++) {
        for (j=0;j<height;j++) {
            //if (_get(i,j,z) == 7) _set(i,j,z, 101);
            if (_get(i,j,z) == 7) _set(i,j,z, 3);
        }
    }
}


/* Data filters */

struct iPoint* convert_to_int_points(struct Point* points, int n_points)
{
    struct iPoint* ipoints = (struct iPoint*)malloc(n_points * sizeof(struct iPoint));
    for (int i=0; i<n_points; i++)
    {
        ipoints[i].x = (int)points[i].x;
        ipoints[i].y = (int)points[i].y;
    }
    return ipoints;
}

template <typename ThePoints>
ThePoints* remove_duplicate_points(ThePoints* points, int n_points, int* n_new_points)
{
    ThePoints* start_points = points;
    ThePoints* current;
    ThePoints* end = points + n_points - 1;
    for (current = points + 1; points < end; points++, current = points + 1)
        while (current < end)
        {
            if ((*current).x == (*points).x && (*current).y == (*points).y)
            {
                *current = *end--;
                n_points--;
            }
            else
                current++;
        }
    points = (ThePoints*)realloc(start_points, n_points*sizeof(ThePoints));
    *n_new_points = n_points;
    return points;
}

// removes points with <2 adjacent points
// assumes no duplicate points in inputs
int remove_stranded_points(struct iPoint* &points, int n_points, int adj_dist)
{
    int n_new_points = 0;
    struct iPoint* new_points = (struct iPoint*)malloc(n_points*sizeof(struct iPoint));
    for (int i=0; i<n_points; i++)
    {
        // look in +/- x,y for another point in the array
        // if >=2 found, copy to new points
        int n_adj = 0;
        bool xpos = false;
        bool xneg = false;
        bool ypos = false;
        bool yneg = false;
        for (int j=0; j<n_points; j++)
        {
            if (i == j) continue;
            for (int k=1; k<=adj_dist; k++)
            {
                // +x
                if (!xpos && points[i].x+k == points[j].x && points[i].y == points[j].y)
                {
                    xpos = true;
                    n_adj += 1;
                }
                // -x
                else if (!xneg && points[i].x-k == points[j].x && points[i].y == points[j].y)
                {
                    xneg = true;
                    n_adj += 1;
                }
                // +y
                else if (!ypos && points[i].x == points[j].x && points[i].y+k == points[j].y)
                {
                    ypos = true;
                    n_adj += 1;
                }
                // -y
                else if (!yneg && points[i].x == points[j].x && points[i].y-k == points[j].y)
                {
                    yneg = true;
                    n_adj += 1;
                }
            }
            if (n_adj >= 2) break;
        }
        if (n_adj >= 2) new_points[n_new_points++] = points[i];
    }

    // copy new points to old points
    for (int i=0; i<n_new_points; i++) points[i] = new_points[i];

    // release tmp array
    free(new_points);

    // resize
    points = (struct iPoint*)realloc(points, n_new_points*sizeof(struct iPoint));
    
    return n_new_points;
}

struct iPoint*  remove_all_stranded_points(struct iPoint* points, int n_points, int* n_new_points, int adj_dist)
{
    int last_n_points = n_points;
    while (last_n_points != (n_points = remove_stranded_points(points, n_points, adj_dist)))
        last_n_points = n_points;
    points = (struct iPoint*)realloc(points, n_points*sizeof(struct iPoint));
    *n_new_points = n_points;
    return points;
}

void surface_veins()
{
    int n_tiles = 1;
    int tiles[n_tiles];
    tiles[0] = t_map::get_cube_id((char*)"methane_ice");
    //tiles[0] = t_map::get_cube_id((char*)"methane_1");
    //tiles[1] = t_map::get_cube_id((char*)"methane_2");
    //tiles[2] = t_map::get_cube_id((char*)"methane_3");
    //tiles[3] = t_map::get_cube_id((char*)"methane_4");
    //tiles[4] = t_map::get_cube_id((char*)"methane_5");
    
    const int n = 40;
    for (int i=0; i<n; i++)
    {
        L_System* cave = _caves();

        struct iPoint* points = convert_to_int_points(cave->points, cave->n_points);
        int n_points;
        points = remove_duplicate_points(points, cave->n_points, &n_points);

        const int adjacency_distance = 3;
        points = remove_all_stranded_points(points, n_points, &n_points, adjacency_distance);

        int h_max = 10;
        int h_min = 5;

        int h_step = (int)(((float)n_points)/((float)(h_max-h_min)));

        int tile = tiles[randrange(0,n_tiles-1)];
        for (int i=0; i<n_points; i++)
        {
            int h = h_max - (i/h_step);
            h=1;
            int x = points[i].x;
            int y = points[i].y;
            //int z = t_map::get_highest_open_block(x,y);
            int z = t_map::get_highest_solid_block(x,y);
            if (t_map::get(x,y,z) == tile) continue;
            for (int i=z; i<z+h; i++)
                t_map::set(x,y,i, tile);
            //printf("h %d\n", h);
        }

        delete cave;
        free(points);
    }
}

void flat_veins()
{
    int n_tiles = 1;
    int tiles[n_tiles];
    tiles[0] = t_map::get_cube_id((char*)"methane_ice");
    
    const int n = 200;
    for (int i=0; i<n; i++)
    {
        L_System* cave = _caves();

        struct iPoint* points = convert_to_int_points(cave->points, cave->n_points);
        int n_points;
        points = remove_duplicate_points(points, cave->n_points, &n_points);

        const int adjacency_distance = 3;
        points = remove_all_stranded_points(points, n_points, &n_points, adjacency_distance);

        int tile = tiles[randrange(0,n_tiles-1)];
        int start_z = 0;
        for (int i=0; i<n_points; i++)
        {
            int x = points[i].x;
            int y = points[i].y;
            int z = t_map::get_highest_solid_block(x,y);
            if (z < 1) z = 1;
            if (i == 0) start_z = randrange(0,z-1);
            if (z > start_z) z = start_z;
            t_map::set(x,y,z,tile);
        }

        delete cave;
        free(points);
    }
}

}   // Dragon
