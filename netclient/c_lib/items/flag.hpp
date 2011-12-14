#pragma once

class Flag {
    public:
        float x,y,z;
        //float vx,vy,vz;

        int id;
        int team;

        void set_position(float x, float y, float z);
        void draw();

        Flag(int id, int team) : id(id), team(team) {}
};
