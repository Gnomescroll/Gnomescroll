#include "dragon.hpp"

#include <c_lib/t_map/t_map.hpp>
//#include <c_lib/map_gen/features.h>
#include <math.h>
//#include <c_lib/common/random.h>

#include <c_lib/SDL/draw_functions.h>

#include "hk2.cpp"

namespace Dragon {

//const int width = 128;
//const int height = 128;

//static Building_list buildings;

//class L_System {
    //private:
        //int char_in_string(char c, char* str) {
            //int i;
            //for (i=0; str[i] != '\0'; i++) {
                //if (str[i] == c) return i;
            //}
            //return -1;
        //}

        //void set_n_rules(int n) {
            //n_rules = n;
            //this->rules = (char**)malloc(n * sizeof(char*));
            //this->rule_lengths = (int*)malloc(n * sizeof(int));
        //}

    //public:
        //int z_min;
        //int z_max;
        //int tile;

        //int width,height;

        //int n_tokens;
        //char* tokens;

        //char* start;
        //int n_rules;
        //char** rules;
        //int* rule_lengths;

        //int iterations;

        //// save initial state
        //double ogx,ogy;
        //double ogdegrees;

        //double x,y;
        //double degrees;
        //double dx,dy;
        //double xstep,ystep;

        //double p;
        //void set_probability(double p) {
            //this->p = p;
        //}

        //double draw_width;
        //void set_draw_width(double draw_width) {
            //this->draw_width = draw_width;
        //}

        //void reset_state() {
            //x = ogx;
            //y = ogy;
            //degrees = ogdegrees;
        //}
        
        //void turn_left() {
            //degrees += 90;
            //compute_delta(degrees);
        //};
        //void turn_right() {
            //degrees -= 90;
            //compute_delta(degrees);
        //}
        //void turn_left_p() {
            //if (randf() < this->p) turn_left();
        //};
        //void turn_right_p() {
            //if (randf() < this->p) turn_right();
        //}
        //void forward() {
            //double _x = x;
            //double _y = y;
            //_x += dx*xstep;
            //_y += dy*ystep;
            //trace_set_map(x, y, _x, _y);
            //x = _x;
            //y = _y;
        //}
        //void forward_p() {
            //if (randf() < this->p) forward();
        //}

        //void building() {
            //if (randrange(0,7)!=0) {
            ////int xmi = min(4,(_x-x-2));
            //int xmi = (4 > ((dx*xstep)-2)) ? 4 : ((dx*xstep)-2);
            //int xma = (8 < ((dx*xstep)+2)) ? 8 : ((dx*xstep)+2);
            //int ymi = (4 > ((dy*ystep)-2)) ? 4 : ((dy*ystep)-2);
            //int yma = (8 < ((dy*ystep)+2)) ? 8 : ((dy*ystep)+2);
            //int xo = randrange(-1,1);
            //int yo = randrange(-1,1);
            //buildings.add(x+xo, y+yo, 0, randrange(xmi,xma), randrange(ymi,yma), randrange(12,20), randrange(101,102));
            //}
        //}
        
        //inline void rotate90(double* x, double* y) {
            //double _x = *x;
            //*x = -*y;
            //*y = _x;
        //}

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

            //int i,j,k;
            //int ix,iy;
            //double mmx,mmy;
            //mmx=mx;mmy=my;
            //for (j=0; j<wsteps; j++) {
                //mmx = mx;
                //mmy = my;
                //for (i=0; i<steps; i++) {
                    //mmx += xstep;
                    //mmy += ystep;
                    //ix = (int)mmx;
                    //iy = (int)mmy;
                    //if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                    //for (k=this->z_min; k<this->z_max+1;k++) {
                        //_set(ix, iy, k, this->tile);
                    //}
                //}
                //mx += wxstep;
                //my += wystep;
            //}
        //}
        ////void trace_set_map(double x, double y, double x1, double y1) {
            ////// step along path to set map
            ////double step = 0.1;
            ////double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            ////int steps = ceil(len/step);
            ////step = len/steps;   // resize
            ////double xstep = step * ((x1-x)/len);
            ////double ystep = step * ((y1-y)/len);
            ////int i;
            ////int ix,iy;
            ////double mx=x,my=y;

            ////double wstep = 0.1;
            ////int wsteps = ceil(draw_width/wstep);
            ////wstep = draw_width / wsteps; // resize
            ////double wx=x,wy=y,wx1=x1,wy1=y1;
            ////rotate90(&wx, &wy);
            ////rotate90(&wx1, &wy1);
            ////double wxstep = wstep * ((wx1-wx)/len);
            ////double wystep = wstep * ((wy1-wy)/len);
            ////mx -= wxstep*((wsteps+1)/2);
            ////my -= wystep*((wsteps+1)/2);

            ////int j;
            ////double mmx,mmy;
            ////mmx=mx;mmy=my;
            ////for (j=0; j<wsteps; j++) {
                ////mmx = mx;
                ////mmy = my;
                ////for (i=0; i<steps; i++) {
                    ////mmx += i*xstep;
                    ////mmy += i*ystep;
                    ////ix = (int)mmx;
                    ////iy = (int)mmy;
                    ////if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                    ////_set(ix, iy, this->z, this->tile);
                ////}
                ////mx += wxstep;
                ////my += wystep;
            ////}
        ////}

        ////void trace_set_map(double x, double y, double x1, double y1) {
            ////// step along path to set map
            ////double step = 0.1;
            ////double len = sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y));
            ////int steps = ceil(len/step);
            ////step = len/steps;   // resize
            ////double xstep = step * ((x1-x)/len);
            ////double ystep = step * ((y1-y)/len);
            ////int i;
            ////int ix,iy;
            ////double mx=x,my=y;

            ////for (i=0; i<steps; i++) {
                ////mx += xstep;
                ////my += ystep;
                ////ix = (int)mx;
                ////iy = (int)my;
                ////if (ix < 0 || ix >= this->width || iy < 0 || iy >= this->height) continue;
                ////_set(ix, iy, this->z, this->tile);
            ////}
        ////}

        //void compute_delta(double degrees) {
            //const double pi = 3.14159;
            //double radians = degrees * (pi / 180);
            //dx = cos(radians);
            //dy = sin(radians);
        //}

        //void set_starting_position(double x, double y) {
            //printf("Set starting position to %0.2f %0.2f\n", x,y);
            //this->x = x;
            //this->y = y;
            //this->ogx = x;
            //this->ogy = y;
        //}

        //void set_starting_degrees(double degrees) {
            //compute_delta(degrees);
        //}

        //void set_dimensions(int w, int h) {
            //this->width = w;
            //this->height = h;
        //}

        //void set_degrees(double degrees) {
            //this->degrees = degrees;
            //this->ogdegrees = degrees;
        //}

        //void set_step_size(double step) {
            //this->xstep = step;
            //this->ystep = step;
        //}
        
        //void set_step_size(double xstep, double ystep) {
            //this->xstep = xstep;
            //this->ystep = ystep;
        //}

        //void set_iterations(int i) {
            //this->iterations = i;
        //}

        //void set_map_tile(int tile) {
            //this->tile = tile;
        //}

        //void set_map_z_level(int z_min, int z_max) {
            //this->z_min = z_min;
            //this->z_max = z_max;
        //}
        //void set_map_z_level(int z) {
            //this->z_min = z;
            //this->z_max = z;
        //}

        //void set_init_condition(char* cond) {
            //this->start = (char*)malloc((strlen(cond)+1) * sizeof(char));
            //strcpy(this->start, cond);
        //}

        //void set_tokens(char* tokens) {
            //this->tokens = (char*)malloc((strlen(tokens)+1) * sizeof(char));
            //this->n_tokens = strlen(tokens);
            //strcpy(this->tokens, tokens);
            //set_n_rules(strlen(this->tokens));
        //}

        //char* parse_rule(char* rule, int* token_id) {
            //int i=0,j=0;
            //char token = rule[i];
            //*token_id = char_in_string(token, this->tokens);
            //if (*token_id < 0) printf("L_System :: parse_rule WARNING -- Token %c not in tokens\n", token);
            //char* parsed = (char*)malloc((strlen(rule)+1) * sizeof(char));
            //for (i=1; rule[i] != '\0'; i++) {
                //if (char_in_string(rule[i], tokens) < 0) continue;
                //parsed[j] = rule[i];
                //j++;
            //}
            //parsed[j] = '\0';
            //char* _parsed = (char*)malloc(j * sizeof(char));
            //strcpy(_parsed, parsed);
            //free(parsed);
            //parsed = _parsed;
            //return parsed;
        //}

        //void add_rule(char* rule) {
            //int token_id = -1;
            //rule = parse_rule(rule, &token_id);
            //this->rules[token_id] = (char*)malloc((strlen(rule)+1) * sizeof(char));
            //strcpy(rules[token_id], rule);
            //this->rule_lengths[token_id] = strlen(rule);
        //}

        ////returns length of next rule
        //int apply_rule(char* rule) {
            //int i;
            //int len=0;
            //int token_id;
            //char token;
            //for (i=0; rule[i] != '\0'; i++) {
                //token = rule[i];
                //token_id = char_in_string(token, this->tokens);
                //if (token_id < 0) {
                    //printf("apply_rule WARNING unrecognized token %c. should not occur.\n", token);
                    //continue;
                //}
                //switch(token) {
                    //case '+':
                        //turn_right();
                        //break;
                    //case ']':
                        //turn_right_p();
                        //break;
                    //case '-':
                        //turn_left();
                        //break;
                    //case '[':
                        //turn_left_p();
                        //break;
                    //case 'F':
                        //forward();
                        //break;
                    //case 'f':
                        //forward_p();
                        //break;
                    //case 'B':
                        //building();
                        //break;
                    //default:
                        //break;
                //}
                //len += rule_lengths[token_id];
            //}
            //return len;
        //}

        //void get_next_rule(char* rule, char* next) {
            //int i,j;
            //int rule_len;
            //int next_index = 0;
            //char token;
            //int token_id;
            //char* insert_rule;
            //for (i=0; rule[i] != '\0'; i++) {
                //token = rule[i];
                //token_id = char_in_string(token, this->tokens);
                //if (token_id < 0) {
                    //printf("get_next_rule WARNING unrecognized token %c. should not occur\n", token);
                    //continue;
                //}
                //rule_len = rule_lengths[token_id];
                //insert_rule = rules[token_id];
                //for (j=0; j<rule_len; j++) {
                    //next[next_index] = insert_rule[j];
                    //next_index++;
                //}
            //}
            //next[next_index] = '\0';
        //}

        //void run() {
            //// apply start
            ////printf("%s\n", start);
            //char* curr = start;
            //char* next;
            //int next_len;
            //next_len = apply_rule(curr);
            //next = (char*)malloc((next_len+1) * sizeof(char));
            //get_next_rule(curr, next);
            //curr = next;
            //int i;
            //for (i=1; i<iterations-1; i++) {
                //next_len = apply_rule(curr);
                //next = (char*)malloc((next_len+1) * sizeof(char));
                //get_next_rule(curr, next);
                //free(curr);
                //curr = next;
                ////printf("%s\n", curr);
            //}
            //next_len = apply_rule(curr);
            //free(curr);
        //}

    //L_System() :
    //n_rules(0), iterations(0), p(1.0), draw_width(0.1)
    //{}
    
    //~L_System() {
        //if (start != NULL) {
            //free(start);
        //}
        //int i;
        //if (rules != NULL) {
            //for (i=0; i<n_rules; i++) {
                //if (rules[i] != NULL) free(rules[i]);
            //}
            //free(rules);
        //}
        //if (tokens != NULL) {
            //free(tokens);
        //}
    //}
//};

//void init() {

    //L_System* L = new L_System();

    //int tile;
    //tile = 0;
    //tile = 101;
    //int z_min = 12;
    //int z_max = 13;
    //char tokens[]= "XYF+-B[]f";
    //int iterations = 20;
    //double degrees = 90;
    ////double degrees = 60;
    //double step = 4;
    //double xstep = 1;
    //double ystep = 1;
    //L->set_tokens(tokens);
    //L->set_iterations(iterations);
    //L->set_degrees(degrees);
    //L->set_step_size(step);
    //L->set_step_size(xstep, ystep);
    //L->set_starting_degrees(0);
    ////L->set_starting_position(width/2+0.5,height/2+0.5);
    //L->set_starting_position(64.5,64.5);
    //L->set_map_tile(tile);
    //L->set_map_z_level(z_min, z_max);
    //L->set_dimensions(width,height);
    //L->set_draw_width(.1);
    //L->set_probability(0.7);

    //// rules
    //L->add_rule("B > B");
    //L->add_rule("X > X]YF");
    //L->add_rule("Y > FX[Y");
    ////L->add_rule("X > X+YF");
    ////L->add_rule("Y > FX-Y");
    //L->add_rule("F > F");
    ////L->add_rule("+ > ]");
    ////L->add_rule("- > [");
    ////L->add_rule("] > +");
    ////L->add_rule("[ > -");
    //L->add_rule("+ > +");
    //L->add_rule("- > -");
    //L->add_rule("] > ]");
    //L->add_rule("[ > [");
    //L->add_rule("f > f");

    //// start condition
    //L->set_init_condition("FX");

    //L->run();

    ////L->reset_state();
    //////L->set_starting_degrees(90);
    ////L->set_step_size(15,7);
    ////L->run();

    //delete L;
//}

//void caves() {

    //L_System* L = new L_System();

    //int tile = 0;
    //int z_min = 32-10;
    //int z_max = 32+5;
    //char tokens[]= "XYF+-B[]f";
    //int iterations = 20;
    //double degrees = 90;
    //double step = 4;
    //double xstep = 1;
    //double ystep = 1;
    //L->set_tokens(tokens);
    //L->set_iterations(iterations);
    //L->set_degrees(degrees);
    //L->set_step_size(step);
    //L->set_step_size(xstep, ystep);
    //L->set_starting_degrees(0);
    //L->set_starting_position(width/2+0.5,height/2+0.5);
    //L->set_map_tile(tile);
    //L->set_map_z_level(z_min, z_max);
    //L->set_dimensions(width,height);
    //L->set_draw_width(.1);
    //L->set_probability(0.7);

    //// rules
    //L->add_rule("B > B");
    //L->add_rule("X > X]YF");
    //L->add_rule("Y > FX[Y");
    //L->add_rule("F > F");
    ////L->add_rule("+ > ]");
    ////L->add_rule("- > [");
    ////L->add_rule("] > +");
    ////L->add_rule("[ > -");
    //L->add_rule("+ > +");
    //L->add_rule("- > -");
    //L->add_rule("] > ]");
    //L->add_rule("[ > [");
    //L->add_rule("f > f");

    //// start condition
    //L->set_init_condition("FX");

    //L->run();

    //delete L;
//}

//bool pattern_match(int x, int y, int z, int pattern_w, int pattern_h) {
    //int edge_tile = 101;
    ////int inside_tile = 11;
    ////int width = 4;
    ////int height = 5;
    //int width = pattern_w;
    //int height = pattern_h;
    //int i,j;
    //bool match = true;

    //// check sides
    //for (j=y; j<y+height; j++) {
        //if (_get(x,j,z) != edge_tile) {
            //match = false;
            //return match;
        //}
        //if (_get(x+width,j,z) != edge_tile) {
            //match = false;
            //return match;
        //}
    //}

    //for (i=x; i<x+width; i++) {
        //if (_get(i,y,z) != edge_tile) {
            //match = false;
            //return match;
        //}
        //if (_get(i,y+height,z) != edge_tile) {
            //match = false;
            //return match;
        //}
    //}
    //return match;
//}

//inline void create_building(int x, int y, int z, int w, int h, int l, int tile) {
    //buildings.add(x,y,z,w,h,l,tile);
//}

//void raster(int pattern_w, int pattern_h) {
    //int z = 0;
    //bool matched;
    //int i,j;
    //for (i=0; i<width; i++) {
        //for (j=0; j<height; j++) {
            //matched = pattern_match(i,j,z,pattern_w, pattern_h);
            //if (matched) {
                //if (randrange(0,1) == 0) {
                    //create_building(i+1,j+1,z,
                        //randrange((int)(((float)pattern_w)*0.8),(int)(((float)pattern_w)*1.2)),
                        //randrange((int)(((float)pattern_h)*0.8),(int)(((float)pattern_h)*1.2)),
                        //randrange(15,45),
                        //randrange(100,101)
                    //);
                //}
            //}
        //}
    //}
//}

//static Road_list roads;

//void make_roads() {
    //L_System* L = new L_System();

    //int tile = 2;
    //int z = 0;
    //char tokens[]= "XYF+-";
    //int iterations = 9;
    //double degrees = 90;
    ////double step = 2.5;
    ////double xstep = 60;
    ////double ystep = 20;
    //double xstep = 5;
    //double ystep = 5;
    ////double draw_width = 8;
    //double draw_width = 0.1;
    //L->set_draw_width(draw_width);
    //L->set_tokens(tokens);
    //L->set_iterations(iterations);
    //L->set_degrees(degrees);
    ////L->set_step_size(step);
    //L->set_step_size(xstep, ystep);
    //L->set_starting_degrees(randrange(0,3)*90);
    ////L->set_starting_degrees(0);
    //L->set_starting_position(randrange(width/4, (3*width)/4)+0.5, randrange(height/4, (3*height)/4)+0.5);
    ////L->set_starting_position(width/2+0.5, height/2+0.5);
    //L->set_map_tile(tile);
    //L->set_map_z_level(z);
    //L->set_dimensions(width,height);

    //// rules
    //L->add_rule("X > X+YF");
    //L->add_rule("Y > FX-Y");
    //L->add_rule("F > F");
    //L->add_rule("+ > +");
    //L->add_rule("- > -");

    //// start condition
    //L->set_init_condition("FX");

    //L->run();

    //// sidewalks
    ////xstep = 50;
    ////ystep = 30;
    ////int qw1 = randrange(1,3),qw2;
    ////while ((qw2=randrange(1,3)) == qw1);
    ////int qh1 = randrange(1,3),qh2;
    ////while ((qh2=randrange(1,3)) == qh1);
    
    ////double sidewalk_draw_width = 4;
    ////L->reset_state();
    ////L->set_iterations(5);
    ////L->set_draw_width(sidewalk_draw_width);
    ////L->set_step_size(xstep, ystep);
    ////L->set_starting_position((qw1*width)/4+0.5, (qh1*height)/4+0.5);
    ////L->set_starting_degrees(randrange(0,3)*90);
    ////L->run();

    ////L->reset_state();
    ////L->set_step_size(ystep, xstep); // flip
    ////L->set_starting_position((qw2*width)/4+0.5, (qh2*height)/4+0.5);
    ////L->set_starting_degrees(randrange(0,3)*90);
    ////L->run();

    //delete L;

//}

//bool is_corner(int x, int y, int z, int inner_tile) {
    //if (_get(x,y,z) == inner_tile
     //&& _get(x-1,y,z) != inner_tile
     //&& _get(x,y-1,z) != inner_tile
     ////&& _get(x-1,y,z) == 2
     ////&& _get(x,y-1,z) == 2
     //) return true;
     //return false;
//}

//void turn90(int* x, int *y) {
    //int _x=*x;
    //*x = -*y;
    //*y = _x;
    ////printf("turned\n");
//}

////class LineSegment {
    ////public:
        ////int z;
        ////int x1,y1;
        ////int x2,y2;
        ////int tile;

        ////void set() {
            ////int i,j;
            ////for (i=x1; i<=x2; i++) {
                ////for (j=y1; j<=y2; j++) {
                    ////_set(i,j,z, tile);
                ////}
            ////}
        ////}
////};

//struct Corner {
    //int i,j,k;
//};

//int corner_index=0;
//struct Corner corners[50];

//void outline_box(int x, int y, int z, int inner_tile, int outline_tile) {
    //printf("outlining box at %d %d %d\n", x,y,z);
    ////move in the x direction
    //int ox=x,oy=y,oz=z; //save beginning
    //int dx=1,dy=0;  // motion

    //int cx=x+1,cy=y,cz=z; // current pos, start one step ahead

    //if (_get(cx,cy,cz) != inner_tile) {
        //printf("special case at %d %d %d\n", cx,cy,cz);
        //return;
        //turn90(&dx,&dy);  // special case where corner is 1 high
        //cx = x;
        //cy = y+1;
    //}
    
    ////while (cx!=ox || cy!=oy || cz!=oz) {
    //while (1) {
        //if (cx==ox && cy==oy && cz==oz) break;
        ////printf("settings %d %d %d to %d\n", cx,cy,cz, outline_tile);
        ////_set_broadcast(cx,cy,cz, outline_tile);
        //_set(cx,cy,cz, outline_tile);
        //turn90(&dx,&dy);
        //turn90(&dx,&dy);
        //turn90(&dx,&dy);
        //if (_get(cx+dx, cy+dy, cz) != inner_tile) turn90(&dx,&dy);
        //cx += dx;
        //cy += dy;
        //if (_get(cx,cy,cz) != inner_tile) {
            ////if (_get(cx+1, cy,cz) == outline_tile) return;
            //cx -= dx;
            //cy -= dy;
            //turn90(&dx,&dy);
            //cx += dx;
            //cy += dy;
        //}
    //}
    //_set(ox,oy,oz,outline_tile);
//}

//void outline_boxes(int inner_tile, int outline_tile) {
    //// look for a corner where the inside tile is in the top left
    //int z = 0;
    //int i,j;
    //struct Corner* c = NULL;
    //for (i=0; i<width; i++) {
        //for (j=0; j<height; j++) {
            ////if (is_corner(i,j,z,inner_tile)) outline_box(i,j,z,inner_tile,outline_tile);
            //if (is_corner(i,j,z,inner_tile)) {
                //c = &corners[corner_index];
                //c->i = i;
                //c->j = j;
                //c->k = z;
                //corner_index++;
            //}
        //}
    //}

    //for (i=0;i<corner_index;i++) {
        //c = &corners[i];
        //outline_box(c->i, c->j, c->k, inner_tile, outline_tile);
    //}
    
//}

//void generate_caves() {
    //int x = width;
    //int y = height;
    //int z = 0;
    //int h = 64;
    //int tile = 3;
    //_floor(x,y, z, h, tile);
    //caves();
    //_box(x,y,z,h,tile);
//}

//bool block_is_edge(int i, int j, int k, int limit) {
    //int tile = _get(i,j,k);
    //int ct = 0;

    //if (_get(i+1, j, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i-1, j, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i, j+1, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i, j-1, k) != tile) ct++;
    //if (ct == limit) return true;

    //if (_get(i+1, j+1, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i+1, j-1, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i-1, j-1, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i-1, j+1, k) != tile) ct++;
    //if (ct == limit) return true;
    
    //return false;
//}

//bool block_is_edge_4(int i, int j, int k, int limit) {
    //int tile = _get(i,j,k);
    //int ct = 0;

    //if (_get(i+1, j, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i-1, j, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i, j+1, k) != tile) ct++;
    //if (ct == limit) return true;
    //if (_get(i, j-1, k) != tile) ct++;
    //if (ct == limit) return true;

    //return false;
//}

//bool block_is_edge_2(int i, int j, int k) {
    //int tile = _get(i,j,k);

    //if (_get(i+1, j, k) != tile
     //&& _get(i-1, j, k) != tile)
     //return true;

    //if (_get(i, j+1, k) != tile
     //&& _get(i, j-1, k) != tile)
     //return true;

    //return false;
//}

//struct Location {
    //unsigned char x,y,z;
//};
//void segment_caves(int z, int tile, int limit) {
    //int i,j;
    //Location* blocks = (Location*)malloc(sizeof(Location)*width*height);
    //int index = 0;
    //// check for a cave block
    //// look at all 8 surrounding blocks
    //// if >1 is non cave, convert to orange (blocked)

    //Location* loc = NULL;
    //int new_tile = 7; // lava
    
    //for (i=0; i<width; i++) {
        //for (j=0; j<height; j++) {
            //if (_get(i,j,z) != tile) continue;
            //if (block_is_edge(i,j,z, limit)) {
                //loc = &blocks[index];
                //loc->x = i;
                //loc->y = j;
                //loc->z = z;
                //index++;
            //}
        //}
    //}

    //for (i=0; i<index; i++) {
        //loc = &blocks[i];
        //_set(loc->x, loc->y, loc->z, new_tile);
    //}

    //free(blocks);
//}
//void segment_caves_4(int z, int tile, int limit) {
    //int i,j;
    //Location* blocks = (Location*)malloc(sizeof(Location)*width*height);
    //int index = 0;
    //// check for a cave block
    //// look at all 8 surrounding blocks
    //// if >1 is non cave, convert to orange (blocked)

    //Location* loc = NULL;
    //int new_tile = 7; // lava
    
    //for (i=0; i<width; i++) {
        //for (j=0; j<height; j++) {
            //if (_get(i,j,z) != tile) continue;
            //if (block_is_edge_4(i,j,z, limit)) {
                //loc = &blocks[index];
                //loc->x = i;
                //loc->y = j;
                //loc->z = z;
                //index++;
            //}
        //}
    //}

    //for (i=0; i<index; i++) {
        //loc = &blocks[i];
        //_set(loc->x, loc->y, loc->z, new_tile);
    //}

    //free(blocks);
//}
//void segment_caves_2(int z, int tile) {
    //int i,j;
    //Location* blocks = (Location*)malloc(sizeof(Location)*width*height);
    //int index = 0;
    //// check for a cave block
    //// look at all 8 surrounding blocks
    //// if >1 is non cave, convert to orange (blocked)

    //Location* loc = NULL;
    //int new_tile = 7; // lava
    
    //for (i=0; i<width; i++) {
        //for (j=0; j<height; j++) {
            //if (_get(i,j,z) != tile) continue;
            //if (block_is_edge_2(i,j,z)) {
                //loc = &blocks[index];
                //loc->x = i;
                //loc->y = j;
                //loc->z = z;
                //index++;
            //}
        //}
    //}

    //for (i=0; i<index; i++) {
        //loc = &blocks[i];
        //_set(loc->x, loc->y, loc->z, new_tile);
    //}

    //free(blocks);
//}

class Hole {
    public:
    unsigned char h,w;
    int x,y;
    int tile;
    void restore(int z) {
        int i,j;
        for (i=x; i<x+w; i++) {
            for(j=y; j<y+h; j++) {
                _set(i,j,z,this->tile);
            }
        }
    }
};

int max_holes = 256;
Hole* holes;
int n_holes = 0;

void save_and_remove_holes(int z, int tile) {
    Hole* h = (Hole*)malloc(sizeof(Hole)*max_holes);
    int n=0;

    // raster for holes
    int i,j;
    int tmp_tile;
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            tmp_tile = _get(i,j,z);
            if (tmp_tile != tile) {
                if (_get(i+1, j,z)==tile
                 && _get(i-1, j,z)==tile
                 && _get(i, j+1,z)==tile
                 && _get(i, j-1,z)==tile)
                {
                    h[n].w = 1;
                    h[n].h = 1;
                    h[n].x = i;
                    h[n].y = j;
                    h[n].tile = tmp_tile;
                    n++;
                    _set(i,j,z,tile);
                }
            }
        }
    }

    n_holes = n;
    h = (Hole*)realloc(h, sizeof(Hole)*n);
    holes = h;
}

void restore_saved_holes(int z) {
    int i;
    for (i=0; i<n_holes;i++) {
        holes[i].restore(z);
    }
}

inline bool point_in_set(Point p, Diagonal* loose_ends, int n_ends, Point* pts) {
    int i;
    Diagonal* diag;
    for (i=0; i<n_ends; i++) {
        diag = &loose_ends[i];
        if (pts[diag->p] == p || pts[diag->q] == p) return true;
    }
    return false;
}

inline int point_in_points(Point p, Point* pts, int points) {
    int i;
    for (i=0; i<points; i++) {
        if (pts[i] == p) return i;
    }
    return -1;
}
inline int point_in_points_both(Point p, Point* pts, int points) {
    int i;
    for (i=0; i<points; i++) {
        if (pts[i] == p && pts[i].bx == p.bx && pts[i].by == p.by) return i;
    }
    return -1;
}

inline bool point_intersects_diagonal(int x, int y, Diagonal* loose_ends, int n_ends, Point* pts) {
    int i;
    Diagonal* d;
    Point *p,*q;
    int start_x, end_x, start_y, end_y;
    for (i=0; i<n_ends; i++) {
        d = &loose_ends[i];
        p = &pts[d->p];
        q = &pts[d->q];
        start_x = (p->x >= q->x) ? q->x : p->x;
        end_x = (p->x < q->x) ? q->x : p->x;
        start_y = (p->y >= q->y) ? q->y : p->y;
        end_y = (p->y < q->y) ? q->y : p->y;
        if (x >= start_x && x <= end_x && y >= start_y && y <= end_y) return true;
    }
    return false;
}

Point get_nearest_endpoint(Point _p, Diagonal* loose_ends, int n_ends, Point* pts, int z, int tile) {
    Point *p = &_p;
    Point q;

    int dx,dy;
    int x=0,y=0;
    dx = p->x - 2*p->bx;
    dy = p->y - 2*p->by;
    while(1) {
        x += dx;
        y += dy;
        if (_get((p->x + x)/2, (p->y+1)/2, z) != tile || _get((p->x + x)/2, (p->y-1)/2, z) != tile) // another vertex
        {
            q.x = p->x+x;
            if (x>0) q.x-=1;
            q.y = p->y;
            break;
        }
        if (point_intersects_diagonal(p->x+x,p->y, loose_ends, n_ends, pts))      // intersection with existing diagonal
        {
            q.x = p->x+x;
            q.y = p->y;
            break;
        }
        if (_get((p->x+1)/2, (p->y+y)/2, z) != tile || _get((p->x-1)/2, (p->y+y)/2, z) != tile) // another vertex
        {
            q.x = p->x;
            q.y = p->y+y;
            if (y>0) q.y-=1;
            break;
        }
        if (point_intersects_diagonal(p->x,p->y+y, loose_ends, n_ends, pts))      // intersection with existing diagonal
        {
            q.x = p->x;
            q.y = p->y+y;
            break;
        }
        if (((p->x+x)/2 > width || p->x+x < 0) && ((p->y+y)/2 > height || p->y+y < 0))
        {
            printf("ERROR get_nearest_endpoint left the map\n");
            break;
        }
    }
    return q;
}

void renumber_v_diagonals(Diagonal *v_diagonals, int v_diag_ct, Point* pts2, Point* pts, int points) {
    bool p_found,q_found;
    int i,j;
    Diagonal *d;
    Point p;
    for (i=0; i<v_diag_ct; i++) {
        p_found = false;
        q_found = false;
        d = &v_diagonals[i];
        
        p = pts2[d->p];
        for (j=0; j<points; j++) {
            if (p == pts[j]) {
                d->p = j;
                p_found = true;
                break;
            }
        }

        p = pts2[d->q];
        for (j=0; j<points; j++) {
            if (p == pts[j]) {
                d->q = j;
                q_found = true;
                break;
            }
        }

        if (!p_found) printf("d->p not found in pts.\n");
        if (!q_found) printf("d->p not found in pts.\n");
    }
}

void swap_diag(Diagonal *a, Diagonal *b)
{
  Diagonal t=*a; *a=*b; *b=t;
}
void quicksort_diagonals_HV(Diagonal* d, Point* p, int beg, int end)
{
    bool is_h = false;
  if (end > beg + 1)
  {
    Diagonal diag = d[beg];
    if (p[diag.p].y == p[diag.q].y) is_h = true;
    int l = beg + 1, r = end;
    while (l < r)
    {
      if (p[d[l].p].y == p[d[l].q].y && !is_h)
        l++;
      else
        swap_diag(&d[l], &d[--r]);
    }
    swap_diag(&d[--l], &d[beg]);
    quicksort_diagonals_HV(d, p, beg, l);
    quicksort_diagonals_HV(d, p, r, end);
  }
}

class Room {
    public:
    int x,y,w,h;
    bool placed;

    int area() {
        return w*h;
    }

    bool collides(Room* r) {
        return !(x+w > r->x || x < r->x+r->w ||
            y+h > r->y || y < r->y+r->h);
    }

    void draw(int z, int tile) {
        int i,j;
        for (i=x-w; i<x; i++) {
            for (j=y; j<y+h; j++) {
                _set(i,j,z,tile);
            }
        }
        //printf("drew ");
        //print();
    }

    void print() {
        printf("%d %d %d %d\n", x,y,w,h);
    }

    Room() :
    x(0), y(0), w(0), h(0),
    placed(false)
    {}
};

inline bool is_topleft_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x+1,y,z) == tile) return false;
    if (_get(x,y-1,z) == tile) return false;
    if (_get(x+1,y-1,z) == tile) return false;
    return true;
}
inline bool is_topright_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x-1,y,z) == tile) return false;
    if (_get(x,y-1,z) == tile) return false;
    if (_get(x-1,y-1,z) == tile) return false;
    return true;
}
inline bool is_bottomleft_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x+1,y,z) == tile) return false;
    if (_get(x,y+1,z) == tile) return false;
    if (_get(x+1,y+1,z) == tile) return false;
    return true;
}
inline bool is_bottomright_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    if (_get(x-1,y,z) == tile) return false;
    if (_get(x,y+1,z) == tile) return false;
    if (_get(x-1,y+1,z) == tile) return false;
    return true;
}
inline bool is_corner(int x, int y, int z, int tile) {
    if (_get(x,y,z) != tile) return false;
    
    if(is_topleft_corner(x,y,z,tile)) return true;
    if(is_topright_corner(x,y,z,tile)) return true;
    if(is_bottomleft_corner(x,y,z,tile)) return true;
    if(is_bottomright_corner(x,y,z,tile)) return true;
    return false;
}

inline bool point_is_leftmost_horizontal_endpoint(Point p, Point* points, Diagonal* diagonals, int n_diagonals, int n_horizontals) {
    int i;
    Diagonal *d;
    Point smaller;
    Point r,s;
    for (i=0; i<n_horizontals; i++) {
        d = &diagonals[i];
        r = points[d->p];
        s = points[d->q];
        smaller = (r.x > s.x) ? r : s;
        if (p == smaller) return true;
    }
    return false;
}

inline bool point_is_bottommost_vertical_endpoint(Point p, Point* points, Diagonal* diagonals, int n_diagonals, int n_horizontals) {
    int i;
    Diagonal *d;
    Point higher;
    Point r,s;
    for (i=n_horizontals; i<n_diagonals; i++) {
        d = &diagonals[i];
        r = points[d->p];
        s = points[d->q];
        higher = (r.y < s.y) ? r : s;
        if (p == higher) return true;
    }
    return false;
}

inline bool is_vertical_line_segment(int tx, int ty, int bx, int by, Point* points, Diagonal *diagonals, int n_diagonals,  int n_horizontals) {
    // tx,ty must be >= bx,by
    int i;
    Point *p,*q;
    Diagonal* d;
    Point *top,*bottom;
    for (i=n_horizontals; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];
        if (p->x != tx) continue;
        top = (p->y > q->y) ? p : q;
        bottom = (p->y < q->y) ? p : q;
        if (ty <= top->y && by >= bottom->y) return true;
    }
    return false;
}


inline bool is_horizontal_line_segment(int lx, int ly, int rx, int ry, Point* points, Diagonal *diagonals, int n_diagonals,  int n_horizontals) {
    // lx,ly must be >= rx,ry
    int i;
    Point *p,*q;
    Diagonal* d;
    Point *left,*right;
    for (i=0; i<n_horizontals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];
        if (p->y != ly) continue;
        left = (p->x > q->x) ? p : q;
        right = (p->x < q->x) ? p : q;
        if (lx <= left->x && rx >= right->x) return true;
    }
    return false;
}

Point *points = NULL;
int n_points = 0;
Diagonal* diagonals = NULL;
int n_diagonals = 0;
int n_horizontals = 0;

Point *corners = NULL;
int n_bottom_corners = 0;
int n_corners = 0;

Room* rooms = NULL;
int n_rooms = 0;

void rect_solver() {
    if (points != NULL) {
        free(points);
        points = NULL;
        n_points = 0;
    }
    if (diagonals != NULL) {
        free(diagonals);
        diagonals = NULL;
        n_diagonals = 0;
        n_horizontals = 0;
    }
    
    int z = 13;
    int tile = 103; // solar panel

    // temporarily remove 2x2 or smaller "holes" from map
    save_and_remove_holes(z, tile);

    int vertex_max = 1024*4;
    Point* pts = (Point*)malloc(sizeof(Point)*vertex_max);

    int i;
    for (i=0; i<vertex_max; i++) {
        pts[i].x = 0;
        pts[i].y = 0;
        pts[i].bx = 0;
        pts[i].by = 0;
    }

    n_points=0;
    get_convex_vertices(z, tile, pts, vertex_max, &n_points);
    if (n_points > vertex_max) printf("ERROR: n_points %d exceeds vertex_max %d\n", n_points, vertex_max);
    printf("Got %d vertices\n", n_points);

    if (!n_points) {
        free(pts);
        return;
    }

    pts = (Point*)realloc(pts, sizeof(Point)*n_points);
    Point* pts2 = (Point*)malloc(sizeof(Point)*n_points);
    memcpy(pts2, pts, sizeof(Point)*n_points);
    for (i=0; i<n_points;i++) {
        if (!(pts[i].x%2) || !(pts[i].y%2)) printf("WARNING Even point: %d :: %d %d\n", i, pts[i].x, pts[i].y);
    }

    int h_diag_ct,v_diag_ct;
    int diagonals_max = 2048*4;
    Diagonal* h_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    get_horizontal_diagonals(pts, n_points, h_diagonals, diagonals_max, &h_diag_ct, z, tile);
    h_diagonals = (Diagonal*)realloc(h_diagonals, sizeof(Diagonal)*h_diag_ct);

    Diagonal* v_diagonals = (Diagonal*)malloc(sizeof(Diagonal)*diagonals_max);
    get_vertical_diagonals(pts2, n_points, v_diagonals, diagonals_max, &v_diag_ct, z, tile);
    v_diagonals = (Diagonal*)realloc(v_diagonals, sizeof(Diagonal)*v_diag_ct);

    n_diagonals = h_diag_ct + v_diag_ct;
    renumber_v_diagonals(v_diagonals, v_diag_ct, pts2, pts, n_points);
    free(pts2);

    // find intersections
    int i_ct = 0;
    Intersection* intersections = (Intersection*)malloc(sizeof(Intersection)*v_diag_ct*h_diag_ct);
    find_intersections(pts, h_diagonals, h_diag_ct, v_diagonals, v_diag_ct, intersections, &i_ct);
    intersections = (Intersection*)realloc(intersections, sizeof(Intersection)*i_ct);

    // apply Hopcropt-Karp to the bipartite graph (Horizontals, Verticals, Intersections)
    int n_matches = hk(h_diag_ct, v_diag_ct, i_ct, intersections);

    // get a maximum independent vertex (diagonals) set from hk results
    int* independent_set = (int*)malloc(sizeof(int)*(n_diagonals));
    int n_ind = 0;
    int k;
    int h_len,v_len;
    for (i=0; i<h_diag_ct; i++) {
        if (!HK::map[i+1]) {  // free h_diag vertex
            independent_set[n_ind++] = i;
        } else {
            h_len = h_diagonals[i].length(pts);
            v_len = v_diagonals[HK::map[i+1] - h_diag_ct - 1].length(pts);
            if (v_len < h_len) k = HK::map[i+1] - h_diag_ct;
            else k = i;
            //k = choose_shortest_diagonal(i, intersections, i_ct, h_diag_ct, h_diagonals, v_diagonals, pts, HK::map);
            independent_set[n_ind++] = k;
        }
    }

    for (i=h_diag_ct; i<n_diagonals; i++) {
        if (!HK::map[i+1]) {
            independent_set[n_ind++] = i;
        }
    }

    printf("Independent Diagonals: %d\n", n_ind);
    if (n_ind != n_diagonals-n_matches) {
        printf("ERROR: independent diagonals processed wrong. Should be %d\n", (n_diagonals)-n_matches);
    }

    /*
     *  Connect remaining vertices
     */

    // copy diagonals into one array, with room for new diagonals
    int n_ends_max = n_points-n_ind;
    int index;
    diagonals = (Diagonal*)malloc(sizeof(Diagonal)*(n_ends_max+n_ind));
    for (i=0; i<n_ind; i++) {
        index = independent_set[i];
        if (index < h_diag_ct) {
            diagonals[i] = h_diagonals[index];
        } else {
            diagonals[i] = v_diagonals[index-h_diag_ct];
        }
    }

    free(independent_set);
    
    pts = (Point*)realloc(pts, sizeof(Point)*(n_points+n_ends_max));
    int n_ends = 0;
    Point p;
    Diagonal* d;
    Point new_p;
    int new_p_slot;
    for (i=0; i<n_points; i++) {
        p = pts[i];
        if (point_in_set(p, diagonals, n_ends+n_ind, pts)) continue;
        d = &diagonals[n_ind+n_ends];
        d->p = i;
        new_p = get_nearest_endpoint(p, diagonals, n_ind+n_ends, pts, z, tile);   // find new endpoint
        new_p_slot = point_in_points(new_p, pts, n_points+n_ends);
        if (new_p_slot < 0) {   // point not in pts
            new_p_slot = n_points+n_ends;
            pts[new_p_slot] = new_p;     // add new point
        }
        d->q = new_p_slot;
        n_ends++;
        if (n_ends >= n_ends_max) {
            printf("n_ends_max %d reached.\n", n_ends_max);
            break;
        }
    }

    pts = (Point*)realloc(pts, sizeof(Point)*(n_points+n_ends));
    points = pts;

    n_diagonals = n_ind + n_ends;
    diagonals = (Diagonal*)realloc(diagonals, sizeof(Diagonal)*n_diagonals);

    // sort diagonals by horizontal-vertical
    // save h_diag_ct;
    quicksort_diagonals_HV(diagonals, points, 0, n_diagonals);
    n_horizontals = 0;
    Point *pp, *qq;
    for (i=0; i<n_diagonals; i++) {
        pp = &points[diagonals[i].p];
        qq = &points[diagonals[i].q];
        if (pp->x == qq->x) // reached verticals
        {
            n_horizontals = i-1;
            break;
        }
    }

    restore_saved_holes(z);
}

void resolve_rooms(int z, int tile) {

    if (corners != NULL) {
        free(corners);
        corners = NULL;
        n_bottom_corners = 0;
        n_corners = 0;
    }

    if (rooms != NULL) {
        free(rooms);
        rooms = NULL;
        n_rooms = 0;
    }

    //if (!n_points) return;

    int corners_max = 2048;
    corners = (Point*)malloc(sizeof(Point)*corners_max);
    n_corners = 0;
    n_bottom_corners = 0;

    int i,j;
    Point *p,*q;
    Diagonal *d;

    int bx,by;
    int dx,dy;
    // iterate diagonals
    // look for bottom or left point intersection with block
    for (i=0; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];

        if (p->y == q->y)        // left
        {
            p = (p->x > q->x) ? p : q;
            bx = (p->x - 1)/2;
            by = (p->y - 1)/2;
            if (_get(bx, by+1, z) == tile
             && _get(bx+1, by+1, z) != tile)            // corner
            {
                q = &corners[n_corners++];
                q->bx = bx;
                q->by = by+1;
                q->x = p->x;
                q->y = p->y;
            }
        }
        else if (p->x == q->x)  // bottom
        {
            p = (p->y < q->y) ? p : q;
            bx = (p->x - 1)/2;
            by = (p->y - 1)/2;
            if (_get(bx, by+1, z) == tile
             && _get(bx, by, z) != tile)            // corner
            {
                q = &corners[n_corners++];
                q->bx = bx;
                q->by = by+1;
                q->x = p->x;
                q->y = p->y;
            }
        }
        
    }

    // iterate for tile patterns
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            if (!is_topleft_corner(i,j,z, tile)) continue;
            // save corner
            p = &corners[n_corners++];
            p->bx = i;
            p->by = j;
            p->x = 2*i+1;
            p->y = 2*j-1;
        }
    }

    // find all intersections between diagonals (endpoint inclusive)
    int intersect_max = (n_horizontals)*(n_diagonals-n_horizontals);
    int i_ct = 0;
    Intersection* intersections = (Intersection*)malloc(sizeof(Intersection)*intersect_max);    
    find_intersections(points, diagonals, n_diagonals, n_horizontals, intersections, &i_ct);
    intersections = (Intersection*)realloc(intersections, sizeof(Intersection)*i_ct);

    Intersection* in;
    Diagonal *e;
    Point *top,*left;
    Point *right,*bottom;
    int x,y;
    for (i=0; i<i_ct; i++) {
        in = &intersections[i];

        d = &diagonals[in->dh];
        e = &diagonals[in->dv];

        //left = (points[d->p].x > points[d->q].x) ? &points[d->p] : &points[d->q];
        top = (points[e->p].y > points[e->q].y) ? &points[e->p] : &points[e->q];
        right = (points[d->p].x < points[d->q].x) ? &points[d->p] : &points[d->q];
        //bottom = (points[e->p].y < points[e->q].y) ? &points[e->p] : &points[e->q];

        if (top->y - right->y < 2) continue;
        if (top->x - right->x < 2) continue;

        x = top->x;
        //y = left->y;
        y = right->y;

        p = &corners[n_corners++];
        p->x = x;
        p->y = y;
        //p->bx = (x-1)/2 + 1;
        p->bx = (x-1)/2;
        p->by = (y-1)/2 + 1;
    }

    n_bottom_corners = n_corners;

    if (n_corners > corners_max) printf("ERROR: n_corners %d exceeds corners_max %d\n", n_corners, corners_max);

    // get the rest of the corners

    // all the intersection points are corners
    Point qq;
    for (i=0; i<i_ct; i++) {
        in = &intersections[i];

        d = &diagonals[in->dh];
        e = &diagonals[in->dv];

        qq.x = points[e->p].x;
        qq.y = points[d->p].y;

        // intersection, but not endpoint inclusive. 4way intersection
        if (orthogonal_intersection(&points[e->p], &points[e->q], &points[d->p], &points[d->q])) {
            
            qq.bx = (qq.x-1)/2;
            qq.by = (qq.y-1)/2;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;

            qq.bx = (qq.x-1)/2+1;
            qq.by = (qq.y-1)/2;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;

            qq.bx = (qq.x-1)/2+1;
            qq.by = (qq.y-1)/2+1;
            p = &corners[n_corners++];
            p->x = qq.x;
            p->y = qq.y;
            p->bx = qq.bx;
            p->by = qq.by;
            
        } else {    // endpoint intersects

            right = (points[d->p].x > points[d->q].x) ? &points[d->p] : &points[d->q];
            left = (points[d->p].x < points[d->q].x) ? &points[d->p] : &points[d->q];
            top = (points[e->p].y > points[e->q].y) ? &points[e->p] : &points[e->q];
            bottom = (points[e->p].y < points[e->q].y) ? &points[e->p] : &points[e->q];

            bool right_edge = false;
            bool left_edge = false;
            bool top_edge = false;
            bool bottom_edge = false;

            if (right->x == top->x) left_edge = true;   // -|
            else
            if (left->x == top->x) right_edge = true;   // |-
            else
            if (top->y == left->y) bottom_edge = true;  // T
            else
            if (bottom->y == left->y) top_edge = true;  // ⟂

            if (!(left_edge || right_edge || top_edge || bottom_edge)) {
                left->print();
                right->print();
                top->print();
                bottom->print();
                printf("WTF\n");
            }

            if (left_edge) { // -|
                //printf("left edge\n");
                qq.bx = (qq.x-1)/2;
                qq.by = (qq.y-1)/2;
                //_set(qq.bx, qq.by, z, 1);
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (right_edge) { // |-
                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2+1;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;

                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (bottom_edge) {  // T
                qq.bx = (qq.x-1)/2;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;

                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }
            else
            if (top_edge) {     // ⟂
                qq.bx = (qq.x-1)/2+1;
                qq.by = (qq.y-1)/2+1;
                p = &corners[n_corners++];
                p->x = qq.x;
                p->y = qq.y;
                p->bx = qq.bx;
                p->by = qq.by;
            }

        }

        
    }

    free(intersections);

    // tile patterns
    for (i=0; i<width; i++) {
        for (j=0; j<height; j++) {
            if (_get(i,j,z) != tile) continue;

            if (is_topright_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i-1;
                p->y = 2*j-1;
            }
            if (is_bottomright_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i-1;
                p->y = 2*j+1;
            }
            if (is_bottomleft_corner(i,j,z,tile)) {
                p = &corners[n_corners++];
                p->bx = i;
                p->by = j;
                p->x = 2*i+1;
                p->y = 2*j+1;
            }
        }
    }

    //// diagonal-wall patterns
    Point *pp;
    for (i=0; i<n_diagonals; i++) {
        d = &diagonals[i];
        p = &points[d->p];
        q = &points[d->q];

        if (i < n_horizontals) {
            right = (p->x > q->x) ? p : q;
            left = (p->x < q->x) ? p : q;

            //      |-
            bx = (left->x - 1)/2;
            by = (left->y - 1)/2;
            if (_get(bx+1, by, z) == tile
             && _get(bx, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by;
                pp->x = left->x;
                pp->y = left->y;
            }

            if (_get(bx+1, by+1, z) == tile
             && _get(bx, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by+1;
                pp->x = left->x;
                pp->y = left->y;
            }

            //      -|
            bx = (right->x - 1)/2;
            by = (right->y - 1)/2;
            if (_get(bx, by, z) == tile
             && _get(bx+1, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx;
                pp->by = by;
                pp->x = right->x;
                pp->y = right->y;
            }

        } else {
            top = (p->y > q->y) ? p : q;
            bottom = (p->y < q->y) ? p : q;

            //      T
            bx = (top->x - 1)/2;
            by = (top->y - 1)/2;
            if (_get(bx, by, z) == tile
             && _get(bx, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx;
                pp->by = by;
                pp->x = top->x;
                pp->y = top->y;
            }
            
            if (_get(bx+1, by, z) == tile
             && _get(bx+1, by+1, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by;
                pp->x = top->x;
                pp->y = top->y;
            }

            //      ⟂
            bx = (bottom->x - 1)/2;
            by = (bottom->y - 1)/2;
            if (_get(bx+1, by+1, z) == tile
             && _get(bx+1, by, z) != tile)
            {
                pp = &corners[n_corners++];
                pp->bx = bx+1;
                pp->by = by+1;
                pp->x = bottom->x;
                pp->y = bottom->y;
            }

        }
    }

    if (n_corners > corners_max) printf("ERROR: n_corners %d exceeds corners_max %d\n", n_corners, corners_max);

    corners = (Point*)realloc(corners, sizeof(Point)*n_corners);

    rooms = (Room*)malloc(sizeof(Room) * n_bottom_corners);

    Room *r;
    n_rooms = 0;
    int k;
    int p_index;
    int pp_p_index = 0;
    for (i=0; i<n_bottom_corners;i++) {
        r = &rooms[n_rooms++];
        p = &corners[i];
        r->x = (p->x)/2 + 1;
        r->y = (p->y)/2 + 1;
        qq = *p;
        // trace from here
        dx = 0;
        dy = 1;
        k = 0;
        do {
            qq.x += dx;
            qq.y += dy;
            if (dx) {
                qq.bx = qq.x/2 + 1;
                qq.by = qq.y/2;
            }
            if (dy) {
                qq.bx = qq.x/2;
                qq.by = qq.y/2;
            }

            if (qq.x<0||qq.x>=width*2||qq.y<0||qq.y>=height*2) {
                printf("ERROR room trace out of bounds\n");
                if (pp == NULL) {
                    printf("Never found a point\n");
                } else {
                    pp->print();
                }
                n_rooms--;
                break;
            }
            p_index = point_in_points_both(qq, corners, n_corners);
            if (p_index >= 0) {
                // check that point is actual vertex
                // if ++ isnt a wall
                // and pt is the smaller of a diag
                pp = &corners[p_index];
                pp_p_index = p_index;
                //pp->print();
                if (dx) {
                    //if (_get((qq.x-1)/2 + 1, (qq.y-1)/2 + 1, z) == tile
                     //&& !(is_vertical_line_segment(qq.x, qq.y+1, qq.x, qq.y, points, diagonals, n_diagonals, n_horizontals))
                    //) {
                    //if (pp->bx*2 - qq.x != 1 && pp->by*2 - qq.y != -1) {
                    if (pp->bx+1 != (qq.x)/2 && pp->by != (qq.y)/2) {
                        continue;
                    }
                    r->w = (p->x - qq.x)/2;
                }
                if (dy) {
                    //if (_get((qq.x-1)/2, (qq.y-1)/2+1, z) == tile
                     //&& !(is_horizontal_line_segment(qq.x, qq.y, qq.x-1, qq.y, points, diagonals, n_diagonals, n_horizontals))
                    //) {
                    //if (pp->bx*2 - qq.x != -1 && pp->by*2 - qq.y != -1) {
                    if (pp->bx != (qq.x)/2 && pp->by != (qq.y)/2) {
                        continue;
                    }
                    r->h = (qq.y - p->y)/2;
                }
                rotate90(&dx, &dy);
                //printf("turned\n");
                k++;
            }
        } while (k < 2);
    }

    rooms = (Room*)realloc(rooms, sizeof(Room)*n_rooms);

    int _tile = 0;
    int tiles[7] = { 1, 4,5,7,11,100,102};
    for (i=0; i<n_rooms; i++) {
        rooms[i].draw(z, tiles[_tile]);
        _tile++;
        _tile%=7;
    }

}

void draw() {
    int i;
    Point *p, *q;
    Diagonal d;
    int z = 14;
    if (diagonals != NULL) {
        for (i=0; i<n_diagonals; i++) {
            d = diagonals[i];
            p = &points[d.p];
            q = &points[d.q];
            _draw_line(255,10,10, ((float)p->x)/2+0.5, ((float)p->y)/2+0.5, z+0.1, ((float)q->x)/2+0.5, ((float)q->y)/2+0.5, z+0.1);
        }
    }

    //for (i=0; i<n_corners; i++) {
        //p = &corners[i];
        //_draw_line(255,10,10, ((float)p->x)/2+0.5+0.25, ((float)p->y)/2+0.5+0.25, z+0.1, ((float)p->x)/2+0.5-0.25, ((float)p->y)/2+0.5-0.25, z+0.1);
        //_draw_line(255,10,10, ((float)p->x)/2+0.5+0.25, ((float)p->y)/2+0.5-0.25, z+0.1, ((float)p->x)/2+0.5-0.25, ((float)p->y)/2+0.5+0.25, z+0.1);
    //}
    if (corners != NULL) {
        for (i=0; i<n_corners; i++) {
        //for (i=0; i<n_bottom_corners; i++) {
            p = &corners[i];
            _draw_line(255,10,10, ((float)p->x)/2+0.5, ((float)p->y)/2+0.5, z+0.1, (float)p->bx + 0.5, (float)p->by + 0.5, z+0.1);
        }
    }
}

void generate() {
    //int x = width;
    //int y = height;
    //int z = 0;
    ////int h = 64;
    //int h = 14;
    //int tile = 3;
    //_floor(x,y, z, h, tile);
    //init();
    ////caves();
    //_walls(x,y,z+1,h-1,tile);

    //segment_caves(13, 101, 2);
    //segment_caves(13, 101, 7);
    //segment_caves(13, 101, 8);
    //segment_caves_4(13, 101, 2);
    //segment_caves_4(13, 101, 2);
    //segment_caves_4(13, 101, 2);
    //segment_caves_4(13, 101, 2);
    //segment_caves_2(13,101);
    //segment_caves_2(13,101);

    //// post processing for good visual of cave isolation
    //int i,j;
    //// convert blue_tron to solar panel (inner caves)
    //for (i=0;i<width;i++) {
        //for (j=0;j<height;j++) {
            //if (_get(i,j,13) == 101) _set(i,j,13, 103);
        //}
    //}
    //// convert lava (outline) to blue_tron
    //for (i=0;i<width;i++) {
        //for (j=0;j<height;j++) {
            ////if (_get(i,j,13) == 7) _set(i,j,13, 101);
            //if (_get(i,j,13) == 7) _set(i,j,13, 3);
        //}
    //}

    int z = 13;
    int tile = 103;
    rect_solver();

    resolve_rooms(z, tile);
    //build_adjacency_graph();

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
    //static int i = 0;
    //if (!i) 
        generate();
    //i++;
}
void draw_dragon() {
    draw();
}
//void outline() {
    //outline_boxes(11,7);
//}


}
