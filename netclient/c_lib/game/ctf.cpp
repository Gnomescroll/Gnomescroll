#include "ctf.hpp"

CTF::CTF() {

    none.init(0);
    one.init(1);
    two.init(2);

    one.set_color(10,10,10);    //black-ish
    two.set_color(10,210,10);   // green-ish

    char team_name_one[] = "Raiders";
    one.set_name(team_name_one);

    char team_name_two[] = "Ex-military";
    two.set_name(team_name_two);
}
