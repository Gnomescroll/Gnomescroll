cdef extern from "./map_gen/dragon.hpp" namespace "Dragon":
    void generate_dragon()

class Dragon(object):
    @classmethod
    def generate(cls):
        generate_dragon()
