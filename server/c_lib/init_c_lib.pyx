from libcpp cimport bool

"""
Options
"""

cdef extern from "./options.hpp" namespace "Options":
    void set_server_name(char* server_name)
    void set_ip_address(char* ip_address)
    void set_port(int port)
    void set_game_mode(char* game_mode)
    void set_team_kills(bool team_kills)
    void set_victory_points(int victory_points) 
    void set_team_name_one(char* team_name_one)
    void set_team_name_two(char* team_name_two)
    void set_map(char* map)
    void set_seed(unsigned int seed)
    void set_logger(bool logger)

def load_options(opts):
    set_server_name(opts.server_name)
    set_ip_address(opts.ip_address)
    set_port(opts.port)
    set_game_mode(opts.game_mode)
    set_team_kills(opts.team_kills)
    set_victory_points(opts.victory_points)
    set_team_name_one(opts.team_name_one)
    set_team_name_two(opts.team_name_two)
    set_map(opts.map)
    set_seed(opts.seed)
    reset_seed(opts.seed)
    set_logger(opts.logger)
