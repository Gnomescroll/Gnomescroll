#include "dragon.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/map_gen/features.h>
#include <math.h>
//#include <c_lib/common/random.h>

#include "hk2.cpp"

namespace Dragon {

const int width = 128;
const int height = 128;

static Building_list buildings;

class L_System {
    private:
        int char_in_string(char c, char* str) {
            int i;
            for (i=0; str[i] != '\0'; i++) {
                if (str[i] == c) return i;
            }
            return -1;
        }

        void set_n_rules(int n) {
            n_rules = n;
            this->rules = (char**)malloc(n * sizeof(char*));
            this->rule_lengths = (int*)malloc(n * sizeof(int));
        }

    public:
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

        double p;
        void set_probability(double p) {
            this->p = p;
        }

        double draw_width;
        void set_draw_width(double draw_width) {
            this->draw_width = draw_width;
        }

        void reset_state() {
            x = ogx;
            y = ogy;
            degrees = ogdegrees;
        }
        
        void turn_left() {
            degrees += 90;
            compute_delta(degrees);
        };
        void turn_right() {
            degrees -= 90;
            compute_delta(degrees);
        }
        void turn_left_p() {
            if (randf() < this->p) turn_left();
        };
        void turn_right_p() {
            if (randf() < this->p) turn_right();
        }
        void forward() {
            double _x = x;
            double _y = y;
            _x += dx*xstep;
            _y += dy*ystep;
            trace_set_map(x, y, _x, _y);
            x = _x;
            y = _y;
        }
        void forward_p() {
            if (randf() < this->p) forward();
        }

        void building() {
            if (randrange(0,7)!=0) {
            //int xmi = min(4,(_x-x-2));
            int xmi = (4 > ((dx*xstep)-2)) ? 4 : ((dx*xstep)-2);
            int xma = (8 < ((dx*xstep)+2)) ? 8 : ((dx*xstep)+2);
            int ymi = (4 > ((dy*ystep)-2)) ? 4 : ((dy*ystep)-2);
            int yma = (8 < ((dy*ystep)+2)) ? 8 : ((dy*ystep)+2);
            int xo = randrange(-1,1);
            int yo = randrange(-1,1);
            buildings.add(x+xo, y+yo, 0, randrange(xmi,xma), randrange(ymi,yma), randrange(12,20), randrange(101,102));
            }
        }
        
        inline void rotate90(double* x, double* y) {
            double _x = *x;
            *x = -*y;
            *y = _x;
        }

        void trace_set_map(double x, double y, double x1, double y1) {
            // step along path to set map
            double step = 0.1;
            double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            int steps = ceil(len/step);
            step = len/steps;   // resize
            double xstep = step * ((x1-x)/len);
            double ystep = step * ((y1-y)/len);
            double mx=x,my=y;

            double wstep = 0.1;
            int wsteps = ceil(draw_width/wstep);
            wstep = draw_width / wsteps; // resize
            double wx=x,wy=y,wx1=x1,wy1=y1;
            rotate90(&wx, &wy);
            rotate90(&wx1, &wy1);
            double wxstep = wstep * ((wx1-wx)/len);
            double wystep = wstep * ((wy1-wy)/len);
            mx -= wxstep*((wsteps+1)/2);
            my -= wystep*((wsteps+1)/2);

            int i,j,k;
            int ix,iy;
            double mmx,mmy;
            mmx=mx;mmy=my;
            for (j=0; j<wsteps; j++) {
                mmx = mx;
                mmy = my;
                for (i=0; i<steps; i++) {
                    mmx += xstep;
                    mmy += ystep;
                    ix = (int)mmx;
                    iy = (int)mmy;
                    if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                    for (k=this->z_min; k<this->z_max+1;k++) {
                        _set(ix, iy, k, this->tile);
                    }
                }
                mx += wxstep;
                my += wystep;
            }
        }
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

        void compute_delta(double degrees) {
            const double pi = 3.14159;
            double radians = degrees * (pi / 180);
            dx = cos(radians);
            dy = sin(radians);
        }

        void set_starting_position(double x, double y) {
            printf("Set starting position to %0.2f %0.2f\n", x,y);
            this->x = x;
            this->y = y;
            this->ogx = x;
            this->ogy = y;
        }

        void set_starting_degrees(double degrees) {
            compute_delta(degrees);
        }

        void set_dimensions(int w, int h) {
            this->width = w;
            this->height = h;
        }

        void set_degrees(double degrees) {
            this->degrees = degrees;
            this->ogdegrees = degrees;
        }

        void set_step_size(double step) {
            this->xstep = step;
            this->ystep = step;
        }
        
        void set_step_size(double xstep, double ystep) {
            this->xstep = xstep;
            this->ystep = ystep;
        }

        void set_iterations(int i) {
            this->iterations = i;
        }

        void set_map_tile(int tile) {
            this->tile = tile;
        }

        void set_map_z_level(int z_min, int z_max) {
            this->z_min = z_min;
            this->z_max = z_max;
        }
        void set_map_z_level(int z) {
            this->z_min = z;
            this->z_max = z;
        }

        void set_init_condition(char* cond) {
            this->start = (char*)malloc((strlen(cond)+1) * sizeof(char));
            strcpy(this->start, cond);
        }

        void set_tokens(char* tokens) {
            this->tokens = (char*)malloc((strlen(tokens)+1) * sizeof(char));
            this->n_tokens = strlen(tokens);
            strcpy(this->tokens, tokens);
            set_n_rules(strlen(this->tokens));
        }

        char* parse_rule(char* rule, int* token_id) {
            int i=0,j=0;
            char token = rule[i];
            *token_id = char_in_string(token, this->tokens);
            if (*token_id < 0) printf("L_System :: parse_rule WARNING -- Token %c not in tokens\n", token);
            char* parsed = (char*)malloc((strlen(rule)+1) * sizeof(char));
            for (i=1; rule[i] != '\0'; i++) {
                if (char_in_string(rule[i], tokens) < 0) continue;
                parsed[j] = rule[i];
                j++;
            }
            parsed[j] = '\0';
            char* _parsed = (char*)malloc(j * sizeof(char));
            strcpy(_parsed, parsed);
            free(parsed);
            parsed = _parsed;
            return parsed;
        }

        void add_rule(char* rule) {
            int token_id = -1;
            rule = parse_rule(rule, &token_id);
            this->rules[token_id] = (char*)malloc((strlen(rule)+1) * sizeof(char));
            strcpy(rules[token_id], rule);
            this->rule_lengths[token_id] = strlen(rule);
        }

        //returns length of next rule
        int apply_rule(char* rule) {
            int i;
            int len=0;
            int token_id;
            char token;
            for (i=0; rule[i] != '\0'; i++) {
                token = rule[i];
                token_id = char_in_string(token, this->tokens);
                if (token_id < 0) {
                    printf("apply_rule WARNING unrecognized token %c. should not occur.\n", token);
                    continue;
                }
                switch(token) {
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
                    case 'B':
                        building();
                        break;
                    default:
                        break;
                }
                len += rule_lengths[token_id];
            }
            return len;
        }

        void get_next_rule(char* rule, char* next) {
            int i,j;
            int rule_len;
            int next_index = 0;
            char token;
            int token_id;
            char* insert_rule;
            for (i=0; rule[i] != '\0'; i++) {
                token = rule[i];
                token_id = char_in_string(token, this->tokens);
                if (token_id < 0) {
                    printf("get_next_rule WARNING unrecognized token %c. should not occur\n", token);
                    continue;
                }
                rule_len = rule_lengths[token_id];
                insert_rule = rules[token_id];
                for (j=0; j<rule_len; j++) {
                    next[next_index] = insert_rule[j];
                    next_index++;
                }
            }
            next[next_index] = '\0';
        }

        void run() {
            // apply start
            //printf("%s\n", start);
            char* curr = start;
            char* next;
            int next_len;
            next_len = apply_rule(curr);
            next = (char*)malloc((next_len+1) * sizeof(char));
            get_next_rule(curr, next);
            curr = next;
            int i;
            for (i=1; i<iterations-1; i++) {
                next_len = apply_rule(curr);
                next = (char*)malloc((next_len+1) * sizeof(char));
                get_next_rule(curr, next);
                free(curr);
                curr = next;
                //printf("%s\n", curr);
            }
            next_len = apply_rule(curr);
            free(curr);
        }

    L_System() :
    n_rules(0), iterations(0), p(1.0), draw_width(0.1)
    {}
    
    ~L_System() {
        if (start != NULL) {
            free(start);
        }
        int i;
        if (rules != NULL) {
            for (i=0; i<n_rules; i++) {
                if (rules[i] != NULL) free(rules[i]);
            }
            free(rules);
        }
        if (tokens != NULL) {
            free(tokens);
        }
    }
};

void init() {

    L_System* L = new L_System();

    int tile;
    tile = 0;
    tile = 101;
    int z_min = 12;
    int z_max = 13;
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
    L->set_map_z_level(z_min, z_max);
    L->set_dimensions(width,height);
    L->set_draw_width(.1);
    L->set_probability(0.7);

    // rules
    L->add_rule("B > B");
    L->add_rule("X > X]YF");
    L->add_rule("Y > FX[Y");
    //L->add_rule("X > X+YF");
    //L->add_rule("Y > FX-Y");
    L->add_rule("F > F");
    //L->add_rule("+ > ]");
    //L->add_rule("- > [");
    //L->add_rule("] > +");
    //L->add_rule("[ > -");
    L->add_rule("+ > +");
    L->add_rule("- > -");
    L->add_rule("] > ]");
    L->add_rule("[ > [");
    L->add_rule("f > f");

    // start condition
    L->set_init_condition("FX");

    L->run();

    //L->reset_state();
    ////L->set_starting_degrees(90);
    //L->set_step_size(15,7);
    //L->run();

    delete L;
}

void caves() {

    L_System* L = new L_System();

    int tile = 0;
    int z_min = 32-10;
    int z_max = 32+5;
    char tokens[]= "XYF+-B[]f";
    int iterations = 20;
    double degrees = 90;
    double step = 4;
    double xstep = 1;
    double ystep = 1;
    L->set_tokens(tokens);
    L->set_iterations(iterations);
    L->set_degrees(degrees);
    L->set_step_size(step);
    L->set_step_size(xstep, ystep);
    L->set_starting_degrees(0);
    L->set_starting_position(width/2+0.5,height/2+0.5);
    L->set_map_tile(tile);
    L->set_map_z_level(z_min, z_max);
    L->set_dimensions(width,height);
    L->set_draw_width(.1);
    L->set_probability(0.7);

    // rules
    L->add_rule("B > B");
    L->add_rule("X > X]YF");
    L->add_rule("Y > FX[Y");
    L->add_rule("F > F");
    //L->add_rule("+ > ]");
    //L->add_rule("- > [");
    //L->add_rule("] > +");
    //L->add_rule("[ > -");
    L->add_rule("+ > +");
    L->add_rule("- > -");
    L->add_rule("] > ]");
    L->add_rule("[ > [");
    L->add_rule("f > f");

    // start condition
    L->set_init_condition("FX");

    L->run();

    delete L;
}

bool pattern_match(int x, int y, int z, int pattern_w, int pattern_h) {
    int edge_tile = 101;
    //int inside_tile = 11;
    //int width = 4;
    //int height = 5;
    int width = pattern_w;
    int height = pattern_h;
    int i,j;
    bool match = true;

    // check sides
    for (j=y; j<y+height; j++) {
        if (_get(x,j,z) != edge_tile) {
            match = false;
            return match;
        }
        if (_get(x+width,j,z) != edge_tile) {
            match = false;
            return match;
        }
    }

    for (i=x; i<x+width; i++) {
        if (_get(i,y,z) != edge_tile) {
            match = false;
            return match;
        }
        if (_get(i,y+height,z) != edge_tile) {
            match = false;
            return match;
        }
    }
    return match;
}

inline void create_building(int x, int y, int z, int w, int h, int l, int tile) {
    buildings.add(x,y,z,w,h,l,tile);
}

void raster(int pattern_w, int pattern_h) {
    int z = 0;
    bool matched;
    int i,j;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            matched = pattern_match(i,j,z,pattern_w, pattern_h);
            if (matched) {
                if (randrange(0,1) == 0) {
                    create_building(i+1,j+1,z,
                        randrange((int)(((float)pattern_w)*0.8),(int)(((float)pattern_w)*1.2)),
                        randrange((int)(((float)pattern_h)*0.8),(int)(((float)pattern_h)*1.2)),
                        randrange(15,45),
                        randrange(100,101)
                    );
                }
            }
        }
    }
}

static Road_list roads;

void make_roads() {
    L_System* L = new L_System();

    int tile = 2;
    int z = 0;
    char tokens[]= "XYF+-";
    int iterations = 9;
    double degrees = 90;
    //double step = 2.5;
    //double xstep = 60;
    //double ystep = 20;
    double xstep = 5;
    double ystep = 5;
    //double draw_width = 8;
    double draw_width = 0.1;
    L->set_draw_width(draw_width);
    L->set_tokens(tokens);
    L->set_iterations(iterations);
    L->set_degrees(degrees);
    //L->set_step_size(step);
    L->set_step_size(xstep, ystep);
    L->set_starting_degrees(randrange(0,3)*90);
    //L->set_starting_degrees(0);
    L->set_starting_position(randrange(width/4, (3*width)/4)+0.5, randrange(height/4, (3*height)/4)+0.5);
    //L->set_starting_position(width/2+0.5, height/2+0.5);
    L->set_map_tile(tile);
    L->set_map_z_level(z);
    L->set_dimensions(width,height);

    // rules
    L->add_rule("X > X+YF");
    L->add_rule("Y > FX-Y");
    L->add_rule("F > F");
    L->add_rule("+ > +");
    L->add_rule("- > -");

    // start condition
    L->set_init_condition("FX");

    L->run();

    // sidewalks
    //xstep = 50;
    //ystep = 30;
    //int qw1 = randrange(1,3),qw2;
    //while ((qw2=randrange(1,3)) == qw1);
    //int qh1 = randrange(1,3),qh2;
    //while ((qh2=randrange(1,3)) == qh1);
    
    //double sidewalk_draw_width = 4;
    //L->reset_state();
    //L->set_iterations(5);
    //L->set_draw_width(sidewalk_draw_width);
    //L->set_step_size(xstep, ystep);
    //L->set_starting_position((qw1*width)/4+0.5, (qh1*height)/4+0.5);
    //L->set_starting_degrees(randrange(0,3)*90);
    //L->run();

    //L->reset_state();
    //L->set_step_size(ystep, xstep); // flip
    //L->set_starting_position((qw2*width)/4+0.5, (qh2*height)/4+0.5);
    //L->set_starting_degrees(randrange(0,3)*90);
    //L->run();

    delete L;

}

bool is_corner(int x, int y, int z, int inner_tile) {
    if (_get(x,y,z) == inner_tile
     && _get(x-1,y,z) != inner_tile
     && _get(x,y-1,z) != inner_tile
     //&& _get(x-1,y,z) == 2
     //&& _get(x,y-1,z) == 2
     ) return true;
     return false;
}

void turn90(int* x, int *y) {
    int _x=*x;
    *x = -*y;
    *y = _x;
    //printf("turned\n");
}

//class LineSegment {
    //public:
        //int z;
        //int x1,y1;
        //int x2,y2;
        //int tile;

        //void set() {
            //int i,j;
            //for (i=x1; i<=x2; i++) {
                //for (j=y1; j<=y2; j++) {
                    //_set(i,j,z, tile);
                //}
            //}
        //}
//};

struct Corner {
    int i,j,k;
};

int corner_index=0;
struct Corner corners[50];

void outline_box(int x, int y, int z, int inner_tile, int outline_tile) {
    printf("outlining box at %d %d %d\n", x,y,z);
    //move in the x direction
    int ox=x,oy=y,oz=z; //save beginning
    int dx=1,dy=0;  // motion

    int cx=x+1,cy=y,cz=z; // current pos, start one step ahead

    if (_get(cx,cy,cz) != inner_tile) {
        printf("special case at %d %d %d\n", cx,cy,cz);
        return;
        turn90(&dx,&dy);  // special case where corner is 1 high
        cx = x;
        cy = y+1;
    }
    
    //while (cx!=ox || cy!=oy || cz!=oz) {
    while (1) {
        if (cx==ox && cy==oy && cz==oz) break;
        //printf("settings %d %d %d to %d\n", cx,cy,cz, outline_tile);
        //_set_broadcast(cx,cy,cz, outline_tile);
        _set(cx,cy,cz, outline_tile);
        turn90(&dx,&dy);
        turn90(&dx,&dy);
        turn90(&dx,&dy);
        if (_get(cx+dx, cy+dy, cz) != inner_tile) turn90(&dx,&dy);
        cx += dx;
        cy += dy;
        if (_get(cx,cy,cz) != inner_tile) {
            //if (_get(cx+1, cy,cz) == outline_tile) return;
            cx -= dx;
            cy -= dy;
            turn90(&dx,&dy);
            cx += dx;
            cy += dy;
        }
    }
    _set(ox,oy,oz,outline_tile);
}

void outline_boxes(int inner_tile, int outline_tile) {
    // look for a corner where the inside tile is in the top left
    int z = 0;
    int i,j;
    struct Corner* c = NULL;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            //if (is_corner(i,j,z,inner_tile)) outline_box(i,j,z,inner_tile,outline_tile);
            if (is_corner(i,j,z,inner_tile)) {
                c = &corners[corner_index];
                c->i = i;
                c->j = j;
                c->k = z;
                corner_index++;
            }
        }
    }

    for (i=0;i<corner_index;i++) {
        c = &corners[i];
        outline_box(c->i, c->j, c->k, inner_tile, outline_tile);
    }
    
}

void generate_caves() {
    int x = width;
    int y = height;
    int z = 0;
    int h = 64;
    int tile = 3;
    _floor(x,y, z, h, tile);
    caves();
    _box(x,y,z,h,tile);
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

bool block_is_edge_4(int i, int j, int k, int limit) {
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

    return false;
}

bool block_is_edge_2(int i, int j, int k) {
    int tile = _get(i,j,k);

    if (_get(i+1, j, k) != tile
     && _get(i-1, j, k) != tile)
     return true;

    if (_get(i, j+1, k) != tile
     && _get(i, j-1, k) != tile)
     return true;

    return false;
}

struct Location {
    unsigned char x,y,z;
};
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
void segment_caves_4(int z, int tile, int limit) {
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
            if (block_is_edge_4(i,j,z, limit)) {
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
void segment_caves_2(int z, int tile) {
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
            if (block_is_edge_2(i,j,z)) {
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



void rect_solver() {
    int vertex_max = 1024;
    Point* pts = (Point*)malloc(sizeof(Point)*vertex_max);

    int i;
    int z = 13;
    int tile = 103; // solar panel
    int points=0;
    get_convex_vertices(z, tile, pts, vertex_max, &points);
    printf("Got %d vertices\n", points);

    pts = (Point*)realloc(pts, sizeof(Point)*points);
    Point* pts2 = (Point*)malloc(sizeof(Point)*points);
    memcpy(pts2, pts, sizeof(Point)*points);
    for (i=0; i<points;i++) {
        if (!(pts[i].x%2) || !(pts[i].y%2)) printf("WARNING Even point: %d :: %d %d\n", i, pts[i].x, pts[i].y);
    }

    int diagonals_max = 2048;
    Diagonal* h_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    int h_diag_ct = 0;
    get_horizontal_diagonals(pts, points, h_diagonals, diagonals_max, &h_diag_ct, z, tile);
    h_diagonals = (Diagonal*)realloc(h_diagonals, sizeof(Diagonal)*h_diag_ct);

    Diagonal* v_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    int v_diag_ct = 0;
    get_vertical_diagonals(pts2, points, v_diagonals, diagonals_max, &v_diag_ct, z, tile);
    v_diagonals = (Diagonal*)realloc(v_diagonals, sizeof(Diagonal)*v_diag_ct);

    Point *p,*q;
    int j;
    int start,end;
    //printf("Horizontals:\n");
    //for (i=0; i<h_diag_ct; i++) {
        //p = &pts[h_diagonals[i].p];
        //q = &pts[h_diagonals[i].q];
        //start = (p->x > q->x) ? q->x : p->x;
        //end = (p->x < q->x) ? q->x : p->x;
        //for (j=start+1; j<end; j++) {
            //_set(j/2,p->y/2,z, 7);
        //}
        //printf("%d,%d -> %d,%d\n", pts[h_diagonals[i].p].x, pts[h_diagonals[i].p].y, pts[h_diagonals[i].q].x, pts[h_diagonals[i].q].y);
    //}
    //printf("Verticals:\n");
    //for (i=0; i<v_diag_ct; i++) {
        //p = &pts2[v_diagonals[i].p];
        //q = &pts2[v_diagonals[i].q];
        //start = (p->y > q->y) ? q->y : p->y;
        //end = (p->y < q->y) ? q->y : p->y;
        //for (j=start+1; j<end; j++) {
            //_set(p->x/2,j/2,z, 1);
        //}

        //printf("%d,%d -> %d,%d\n", pts2[v_diagonals[i].p].x, pts2[v_diagonals[i].p].y, pts2[v_diagonals[i].q].x, pts2[v_diagonals[i].q].y);
    //}

    // TODO: double the point so that corners of blocks can be used, instead of treating blocks as single points

    // find intersections
    int i_ct = 0;
    Intersection* intersections = (Intersection*)malloc(sizeof(Intersection)*v_diag_ct*h_diag_ct);
    find_intersections(pts, h_diagonals, h_diag_ct, pts2, v_diagonals, v_diag_ct, intersections, &i_ct);
    intersections = (Intersection*)realloc(intersections, sizeof(Intersection)*i_ct);

    // apply Hopcropt-Karp to the bipartite graph (Horizontals, Verticals, Intersections)
    int n_matches = hk(h_diag_ct, v_diag_ct, i_ct, intersections);

    // get a maximum independent vertex (diagonals) set from hk results
    int* independent_set = (int*)malloc(sizeof(int)*(h_diag_ct+v_diag_ct));
    int n_ind = 0;
    int use_h=1;
    int k;
    for (i=0; i<h_diag_ct; i++) {
        if (!HK::map[i+1]) {  // free h_diag vertex
            independent_set[n_ind++] = i;
        } else {
            //if (_i >= h_diag_ct) {
                //continue;   // should have processed all these by now
            //}
            if (use_h) {    // use h_diag endpoint
                independent_set[n_ind++] = i;
            } else {        // use v_diag endpoint
                k = find_matching_diagonal(i, intersections, i_ct, h_diag_ct, HK::map);
                independent_set[n_ind++] = h_diag_ct + k;
            }
            use_h++;
            use_h%=2;
            //use_h = randrange(0,1);
        }
    }

    for (i=h_diag_ct; i<v_diag_ct+h_diag_ct; i++) {
        if (!HK::map[i+1]) {
            independent_set[n_ind++] = i;
        }
    }

    printf("Independent Diagonals: %d\n", n_ind);
    if (n_ind != (h_diag_ct+v_diag_ct)-n_matches) {
        printf("ERROR: independent diagonals processed wrong. Should be %d\n", (h_diag_ct+v_diag_ct)-n_matches);
    }
    int index;
    for (i=0; i<n_ind; i++) {
        index = independent_set[i];
        if (index < h_diag_ct) {
            p = &pts[h_diagonals[index].p];
            q = &pts[h_diagonals[index].q];
            start = (p->x > q->x) ? q->x : p->x;
            end = (p->x < q->x) ? q->x : p->x;
            for (j=start+1; j<end; j++) {
                _set(j/2,p->y/2,z, 7);
            }
        } else {
            index -= h_diag_ct;
            p = &pts2[v_diagonals[index].p];
            q = &pts2[v_diagonals[index].q];
            start = (p->y > q->y) ? q->y : p->y;
            end = (p->y < q->y) ? q->y : p->y;
            for (j=start+1; j<end; j++) {
                _set(p->x/2,j/2,z, 1);
            }
        }
    }

    // last step: connecting remaining free vertices

    
    free(independent_set);
}



void generate() {
    int x = width;
    int y = height;
    int z = 0;
    //int h = 64;
    int h = 14;
    int tile = 3;
    _floor(x,y, z, h, tile);
    init();
    //caves();
    _walls(x,y,z+1,h-1,tile);

    segment_caves(13, 101, 2);
    segment_caves(13, 101, 7);
    segment_caves(13, 101, 8);
    segment_caves_4(13, 101, 2);
    segment_caves_4(13, 101, 2);
    segment_caves_4(13, 101, 2);
    segment_caves_4(13, 101, 2);
    segment_caves_2(13,101);
    segment_caves_2(13,101);

    // post processing for good visual of cave isolation
    int i,j;
    // convert blue_tron to solar panel (inner caves)
    for (i=0;i<width;i++) {
        for (j=0;j<height;j++) {
            if (_get(i,j,13) == 101) _set(i,j,13, 103);
        }
    }
    // convert lava (outline) to blue_tron
    for (i=0;i<width;i++) {
        for (j=0;j<height;j++) {
            //if (_get(i,j,13) == 7) _set(i,j,13, 101);
            if (_get(i,j,13) == 7) _set(i,j,13, 3);
        }
    }

    rect_solver();

    //int pattern_w = 7;
    //int pattern_h = 15;
    //raster(pattern_w, pattern_h);
    //pattern_w = 15;
    //pattern_h = 7;
    //raster(pattern_w, pattern_h);
    //buildings.set();
    //_floor(x,y, z, h, tile);
    //make_roads();
    //roads.set();

    // test box
    //int i,j;
    //for (i=10;i<20;i++) {
        //for (j=10;j<30;j++) {
            //if (i != 10 && i != 19 && j!=10 && j!=29) continue;
            //_set(i,j,0,101);
        //}
    //}

    //outline_boxes(tile, 7);
}

//cython
void generate_dragon() {
    generate();
}

void outline() {
    outline_boxes(11,7);
}


}
