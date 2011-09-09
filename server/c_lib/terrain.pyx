cdef extern from "./t_map/t_map.h":
    int _get_highest_open_block(int x, int y, int n)
    int _get_lowest_open_block(int x, int y, int n)

# n defines how many continuous open spaces must exist
def get_highest_open_block(int x, int y, int n=1):
    return _get_highest_open_block(x,y,n)

def get_lowest_open_block(int x, int y, int n=1):
    return _get_lowest_open_block(x,y,n)
