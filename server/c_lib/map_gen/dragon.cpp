#include "dragon.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/map_gen/features.h>
#include <math.h>
#include <c_lib/common/random.h>

namespace Dragon {

const int width = 128;
const int height = 128;

class L_System {
    private:
        int char_in_string(char c, char* str) {
            int i;
            for (i=0; str[i] != '\0'; i++) {
                if (str[i] == c) return i;
            }
            return -1;
        }
        
    public:
        int z;
        int tile;

        int width,height;

        int n_tokens;
        char* tokens;

        char* start;
        int n_rules;
        char** rules;
        int* rule_lengths;

        char* current_rule;
        int iterations;

        double x,y;
        double degrees;
        double dx,dy;
        double xstep,ystep;
        void turn_left() {
            degrees += 90;
            compute_delta(degrees);
        };
        void turn_right() {
            degrees -= 90;
            compute_delta(degrees);
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

        void trace_set_map(double x, double y, double x1, double y1) {
            // step along path to set map
            double step = 0.1;
            double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            double xstep = step * ((x1-x)/len);
            double ystep = step * ((y1-y)/len);
            int steps = ceil(len/step);
            int i;
            int ix,iy;
            double mx=x,my=y;
            for (i=0; i<steps; i++) {
                mx += xstep;
                my += ystep;
                ix = (int)mx;
                iy = (int)my;
                if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                _set(ix, iy, this->z, this->tile);
            }
        }

        void compute_delta(double degrees) {
            const double pi = 3.14159;
            double radians = degrees * (pi / 180);
            dx = cos(radians);
            dy = sin(radians);
        }

        void set_starting_position(double x, double y) {
            this->x = x;
            this->y = y;
        }

        void set_starting_degrees(double degrees) {
            compute_delta(degrees);
            compute_delta(degrees);
        }

        void set_dimensions(int w, int h) {
            this->width = w;
            this->height = h;
        }

        void set_degrees(double degrees) {
            this->degrees = degrees;
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

        void set_map_z_level(int z) {
            this->z = z;
        }

        void set_init_condition(char* cond) {
            this->start = (char*)malloc((strlen(cond)+1) * sizeof(char));
            strcpy(this->start, cond);
        }

        void set_tokens(char* tokens) {
            this->tokens = (char*)malloc((strlen(tokens)+1) * sizeof(char));
            this->n_tokens = strlen(tokens);
            strcpy(this->tokens, tokens);
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

        void set_n_rules(int n) {
            n_rules = n;
            this->rules = (char**)malloc(n * sizeof(char*));
            this->rule_lengths = (int*)malloc(n * sizeof(int));
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
                    case '-':
                        turn_left();
                        break;
                    case 'F':
                        forward();
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
    n_rules(0), iterations(0)
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


    int tile = 101;
    int z = 0;
    char tokens[]= "XYF+-";
    int iterations = 10;
    double degrees = 90;
    //double step = 2.5;
    double xstep = 4;
    double ystep = 5;
    L->set_n_rules(strlen(tokens));
    L->set_tokens(tokens);
    L->set_iterations(iterations);
    L->set_degrees(degrees);
    //L->set_step_size(step);
    L->set_step_size(xstep, ystep);
    L->set_starting_degrees(0);
    L->set_starting_position(width/2+0.5,height/2+0.5);
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

    delete L;
}

bool pattern_match(int x, int y, int z) {
    int edge_tile = 101;
    int inside_tile = 11;
    int width = 4;
    int height = 5;
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

void place_building(int x, int y, int z, int w, int h, int depth) {
    int i,j,k;
    //int tile = 7;
    int tile = 101;
    for (i=x; i<w+x; i++) {
        for (j=y; j<h+y; j++) {
            for (k=z; k<depth+z; k++) {
                if (i > x && i < w+x-1 && j > y && j < h+y-1 && k != depth+z-1) continue; // hollow
                _set(i,j,k,tile);
            }
        }
    }
}

void raster() {
    int x = width;
    int y = height;
    int z = 0;
    bool matched;
    int i,j;
    for (i=0; i<x; i++) {
        for (j=0; j<y; j++) {
            matched = pattern_match(i,j,z);
            if (matched) {
                //printf("%d %d\n", i,j);
                if (randrange(0,2) == 0) {
                    place_building(i+1,j+1,z, randrange(3,6), randrange(3,6), randrange(7,15));
                }
            }
        }
    }
}

void generate() {
    int x = width;
    int y = height;
    int z = 0;
    int h = 1;
    int tile = 11;
    _floor(x,y, z, h, tile);
    init();
    raster();
    _floor(x,y, z, h, tile);

}

//cython
void generate_dragon() {
    generate();
}
}
