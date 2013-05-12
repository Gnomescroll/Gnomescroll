#include "pathfinding.hpp"

#include <stdlib.h>
#include <t_map/_interface.hpp>
#include <common/common.hpp>
#include <t_map/node_hashmap.hpp>

#if DC_CLIENT
# include <SDL/draw_functions.hpp>
#endif

namespace Path
{

#if PRODUCTION
# define PATHFINDING_DEBUG 0
# define PATHFINDING_VERBOSE 0
#elif DC_SERVER
# define PATHFINDING_DEBUG 1
# define PATHFINDING_VERBOSE 0
#else
# define PATHFINDING_DEBUG 1
# define PATHFINDING_VERBOSE 1
#endif

#if PATHFINDING_DEBUG
# define PATH_ASSERT(x) GS_ASSERT(x)
#else
# define PATH_ASSERT(x)
#endif

#if PATHFINDING_VERBOSE
# define PATH_PRINT(...) printf(__VA_ARGS__)
#else
# define PATH_PRINT(...)
#endif


/*
 * References:
 *
 * http://www.policyalmanac.org/games/aStarTutorial.htm
 * http://www-cs-students.stanford.edu/~amitp/gameprog.html#paths
 * http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html
 * http://aigamedev.com/open/tutorial/symmetry-in-pathfinding/
 * http://grastien.net/ban/articles/hg-aaai11.pdf
 *
 */

// F = G + H
// F: final cost
// G: movement cost (10 for lateral, 14 for diagonal)
// H: heuristic (manhattan)

HashMap* open_map;
HashMap* closed_map;

void init()
{
    open_map = new HashMap;
    closed_map = new HashMap;
}

void teardown()
{
    delete open_map;
    delete closed_map;
}


// max distance between 2 nodes (manhattan)
static const int MAX_PATH_DISTANCE = 128;
static const size_t OPEN_START_SIZE = 128;
static const size_t CLOSED_START_SIZE = 128;
static const size_t OPEN_NODES_MAX = 2048;
static const size_t CLOSED_NODES_MAX = 2048;

typedef enum
{
    DIR_LATERAL_PLANAR,
    DIR_DIAGONAL_PLANAR,
    DIR_LATERAL_NONPLANAR,
    DIR_DIAGONAL_NONPLANAR,
} DirectionType;

struct PathAdjacency
{
    struct Vec3i adj;
    int cost;
    DirectionType dir;
};

// per-block of distance
static const int fall_cost = 50;
static const int jump_cost = 100;

static const struct PathAdjacency adj[8+9+9] = {
    // 2d
    {{{{  1,  0,  0 }}}, 100, DIR_LATERAL_PLANAR },
    {{{{ -1,  0,  0 }}}, 100, DIR_LATERAL_PLANAR },
    {{{{  0,  1,  0 }}}, 100, DIR_LATERAL_PLANAR },
    {{{{  0, -1,  0 }}}, 100, DIR_LATERAL_PLANAR },
    {{{{  1,  1,  0 }}}, 141, DIR_DIAGONAL_PLANAR },
    {{{{  1, -1,  0 }}}, 141, DIR_DIAGONAL_PLANAR },
    {{{{ -1,  1,  0 }}}, 141, DIR_DIAGONAL_PLANAR },
    {{{{ -1, -1,  0 }}}, 141, DIR_DIAGONAL_PLANAR },
//8
    {{{{  0,  0, -1 }}}, 100, DIR_LATERAL_PLANAR },
    {{{{  0,  0,  1 }}}, 100, DIR_LATERAL_PLANAR },
//10
    {{{{  1,  0,  1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{ -1,  0,  1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{  0,  1,  1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{  0, -1,  1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{  1,  0, -1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{ -1,  0, -1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{  0,  1, -1 }}}, 141, DIR_LATERAL_NONPLANAR },
    {{{{  0, -1, -1 }}}, 141, DIR_LATERAL_NONPLANAR },
//18
    {{{{  1,  1,  1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{  1, -1,  1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{ -1,  1,  1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{ -1, -1,  1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{  1,  1, -1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{  1, -1, -1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{ -1,  1, -1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
    {{{{ -1, -1, -1 }}}, 173, DIR_DIAGONAL_NONPLANAR },
//26
};

static const struct Vec3i anchor[6] = {
    {{{  0,  0, -1 }}},
    {{{  0,  0,  1 }}},
    {{{  1,  0,  0 }}},
    {{{ -1,  0,  0 }}},
    {{{  0,  1,  0 }}},
    {{{  0, -1,  0 }}},
};

static inline struct Vec3i add_pos_adj(const struct Vec3i& pos, int iadj)
{   // NOTE: make sure to check that the final z position is in bounds
    return vec3i_add(pos, adj[iadj].adj);
}

struct Score
{
    int f;
    int g;
    int h;
};

struct Node
{
    int id;
    int parent;
    int anchor;
    struct Vec3i pos;
    struct Score score;
};

static inline void _node_update_score(struct Node& node, int g)
{
    PATH_ASSERT(g != node.score.g);
    node.score.g = g;
    node.score.f = node.score.g + node.score.h;
}

static inline void _node_set_position(struct Node& node, const struct Vec3i& pos, const struct Vec3i& end)
{
    node.pos = pos;
    node.score.h = euclidean_distance(node.pos, end) * 100;
}

static inline void node_first(HashMap* map, struct Node& node, const struct Vec3i& start, const struct Vec3i& end)
{   // init first node in the sequence
    node.parent = -1;
    node.id = 0;
    node.score.g = -1;
    node.anchor = 0;
    _node_set_position(node, start, end);
    _node_update_score(node, 0);
    map->set(node.pos, node.id);
}

static inline void node_update(struct Node& node, const struct Vec3i& end, int cost)
{
    _node_set_position(node, node.pos, end);
    _node_update_score(node, node.score.g + cost);
}

static inline bool node_reassign(struct Node& node, int new_g, int parent)
{   // if the new g score is better, take that parent
    if (node.score.g >= new_g)
        return false;
    _node_update_score(node, new_g);
    node.parent = parent;
    return true;
}

static inline void node_print(const struct Node& node)
{
    printf("ID: %03d; Parent: %03d; Score: f=%04d; g=%04d, h=%04d; ", node.id,
           node.parent, node.score.f, node.score.g, node.score.h);
    vec3i_print(node.pos);
}

static inline void nodes_print(const struct Node* nodes, size_t len)
{
    for (size_t i=0; i<len; i++)
    {
        printf("Node %03d: ", int(i));
        node_print(nodes[i]);
    }
}

static int compare_node_score(const void* _a, const void* _b)
{   // we want descending order, so b comes first
    const struct Node* a = static_cast<const struct Node*>(_a);
    const struct Node* b = static_cast<const struct Node*>(_b);
    int d = b->score.f - a->score.f;
    // we do a tiebreaker here, to reduce the search space. if the point is closer,
    // according to our heuristic h, then it wins in the tie
    if (d == 0)
        d = b->score.h - a->score.h;
    return d;
}

static inline void sort_nodes(HashMap* map, struct Node* nodes, size_t len)
{
    qsort(nodes, len, sizeof(nodes[0]), &compare_node_score);
    for (int i=0; i<int(len); i++)
    {
        // We have to update the hashmap for the changed values
        if (nodes[i].id != i)
            map->set(nodes[i].pos, i);
        // Update the ids of the nodes:
        //  (we need to do this with or without the hashmap)
        nodes[i].id = i;
    }
}

static inline int add_node(HashMap* map, struct Node& node,
                           struct Node*& nodes, size_t& max_len, size_t& len)
{
    #if !PATHFINDING_DEBUG
    IF_ASSERT(max_len == 0) return -1;
    #endif
    if (len >= max_len)
    {
        PATH_PRINT("Reallocing nodelist from %d to %d...", int(len), int(max_len*2));
        struct Node* _nodes = (struct Node*)realloc(nodes, 2*max_len*sizeof(struct Node));
        IF_ASSERT(_nodes == NULL)
        {
            PATH_PRINT("\n");
            return -1;
        }
        PATH_PRINT("done\n");
        max_len *= 2;
        nodes = _nodes;
    }
    node.id = len;
    nodes[len] = node;
    map->set(node.pos, node.id);
    len++;
    return len-1;
}

static inline struct Node get_lowest_cost_node(HashMap* open_map,
                                               const struct Node* open,
                                               size_t& iopen)
{
    PATH_ASSERT(iopen > 0);
    struct Node node = open[--iopen];
    #if PATHFINDING_DEBUG
    int ret =
    #endif
    open_map->pop(node.pos);
    #if PATHFINDING_DEBUG
    PATH_ASSERT(ret == node.id);
    if (ret != node.id)
        printf("popped position doesn't match: %d != %d\n", ret, node.id);
    #endif
    return node;
}

//static inline int get_node_pos_index(const struct Vec3i& pos,
                                     //const Node* nodes, size_t len)
//{
    //for (size_t i=0; i<len; i++)
        //if (is_equal(pos, nodes[i].pos))
            //return int(i);
    //return -1;
//}

struct Vec3i* construct_path(const struct Node* open, size_t iopen,
                              struct Node* closed, size_t iclosed,
                              size_t& len)
{
    len = 0;
    if (iopen == 0 || iclosed == 0) return NULL;
    // walk path backwards, from last item on closed list
    len = 1;
    closed[0].parent = -1;
    int first = closed[iclosed-1].parent;
    while (first >= 0)
    {
        first = closed[first].parent;
        len++;
    }
    struct Vec3i* path = (struct Vec3i*)malloc(len * sizeof(struct Vec3i));

    // add to path array, starting with the last element
    int i = len;
    first = closed[iclosed-1].id;
    while (first >= 0)
    {
        path[--i] = closed[first].pos;
        first = closed[first].parent;
    }

    // TODO -- condense path
    // if ai-aj == aj-ak, remove aj

    return path;
}

struct Passable2D
{
    static bool is_passable(const struct Node& node, int iadj,
                            struct Node& exit, int& cost)
    {
        struct Vec3i cur = node.pos;
        struct Vec3i dst = add_pos_adj(cur, iadj);
        if (dst.z <= 0) return false;
        if (t_map::isSolid(dst))
            return false;
        exit.pos = dst;
        cost = adj[iadj].cost;
        if (adj[iadj].dir == DIR_LATERAL_PLANAR)
            return true;
        PATH_ASSERT(adj[iadj].dir == DIR_DIAGONAL_PLANAR);
        return (!t_map::isSolid(dst.x, cur.y, cur.z) &&
                !t_map::isSolid(cur.x, dst.y, cur.z));
    }
};

struct Passable3DAir
{
    static bool is_passable(const struct Node& node, int iadj,
                            struct Node& exit, int& cost)
    {
        struct Vec3i cur = node.pos;
        struct Vec3i dst = add_pos_adj(cur, iadj);
        if (dst.z <= 0 || dst.z > map_dim.z) return false;
        if (t_map::isSolid(dst))
            return false;
        exit.pos = dst;
        cost = adj[iadj].cost;
        if (adj[iadj].dir == DIR_LATERAL_PLANAR)
            return true;

        bool lat_ok = (!t_map::isSolid(dst.x, cur.y, cur.z) &&
                       !t_map::isSolid(cur.x, dst.y, cur.z));
        if (adj[iadj].dir == DIR_DIAGONAL_PLANAR)
            return lat_ok;

        bool zlat_ok = (!t_map::isSolid(cur.x, cur.y, dst.z) &&
                        !t_map::isSolid(dst.x, dst.y, cur.z));
        if (adj[iadj].dir == DIR_LATERAL_NONPLANAR)
            return zlat_ok;

        PATH_ASSERT(adj[iadj].dir == DIR_DIAGONAL_NONPLANAR);
        return (lat_ok && zlat_ok &&
                !t_map::isSolid(dst.x, cur.y, dst.z) &&
                !t_map::isSolid(cur.x, dst.y, dst.z));
    }
};

struct Passable3DSurface
{
    static bool is_passable(const struct Node& node, int iadj,
                            struct Node& exit, int& cost)
    {
        struct Vec3i cur = node.pos;
        struct Vec3i dst = add_pos_adj(cur, iadj);
        if (dst.z <= 0 || dst.z > map_dim.z) return false;
        if (t_map::isSolid(dst))
            return false;
        exit.pos = dst;
        cost = adj[iadj].cost;
        if (adj[iadj].dir == DIR_LATERAL_PLANAR)
            return true;

        bool lat_ok = (!t_map::isSolid(dst.x, cur.y, cur.z) &&
                       !t_map::isSolid(cur.x, dst.y, cur.z));
        //if (adj[iadj].dir == DIR_DIAGONAL_PLANAR)
            return lat_ok;

        // ANCHOR WORK:

        // from start position, latch to adjacent solid
            // priority: -z, x, -x, y, -y, z
            // if not attached, move down until we hit solid -z

        // now that we have start and anchor...
            // need to handle direction?
        // we are either travelling on the xy plane or the z plane, so keep track of that
        // when the plane switches, we need to get a new anchor point
        // edge case:
            // traveling on the inside of a box
            // traveling on the outside of a box
            // moving into a corner


        // only allow up/down in z, no diagonals for z
        // position must always be on ground except:
            // when going up a wall, up to 1 block past the wall height
            // when going off a ledge
            // this means we need to know the direction of the wall we're latched to

            // QUESTION ?? Ceilings allowed?
                // Yes - this routine would be for surface huggers
                // Jumpers will use a different routine
    }
};

struct Passable3DJump
{
    static bool can_jump_to(const struct Vec3i& start, const struct Vec3i& end,
                            int clearance, int jump_max)
    {   // make sure there is vertical clearance to jump to next level (both down and up)
        int d = end.z - start.z;
        if (d > jump_max || d < 0) return false;
        clearance--;    // a clearance of 1 is accounted for
        // we start at 1 above start,
        // because its assumed start.z has already been checked to be clear
        for (int i=start.z+1; i<=end.z+clearance; i++)
            if (t_map::isSolid(start.x, start.y, i))
                return false;
        for (int i=end.z+1; i<=end.z+clearance; i++)
            if (t_map::isSolid(end.x, end.y, end.z+clearance))
                return false;
        return true;
    }

    static bool can_fall_to(const struct Vec3i& start, const struct Vec3i& end,
                            int clearance, int fall_max)
    {
        clearance--;    // a clearance of 1 is accounted for
        int d = start.z - end.z;
        if (d > fall_max || d < 0) return false;
        for (int i=start.z + clearance; i>=end.z; i--)
            if (t_map::isSolid(end.x, end.y, i))
                return false;
        return true;
    }

    static bool is_passable(const struct Node& node, int iadj,
                            struct Node& exit, int& cost)
    {
        const int clearance = 1;    // TODO -- parameterized
        const int max_down = 9;
        const int max_up = 3;
        struct Vec3i cur = node.pos;
        struct Vec3i dst = add_pos_adj(cur, iadj);
        if (dst.z <= 0 || dst.z > map_dim.z)
            return false;

        if (adj[iadj].dir == DIR_LATERAL_PLANAR)
        {   // attempt a jump
            struct Vec3i up = vec3i_init(dst);
            struct Vec3i down = vec3i_init(dst);
            // Note: _below returns <= dst.z; _above returns > dst.z
            down.z = t_map::get_nearest_surface_block_below(dst, clearance, max_down);
            up.z = t_map::get_nearest_surface_block_above(dst, clearance, max_up);

            PATH_ASSERT(cur.z - down.z >= 0);
            PATH_ASSERT(up.z - cur.z >= 1);

            if (down.z >= 0 && cur.z - down.z < up.z - cur.z)
            {
                exit.pos = down;
                cost = adj[iadj].cost * (cur.z - down.z + 1);// + fall_cost * (cur.z - down.z);
                return Passable3DJump::can_fall_to(cur, down, clearance, max_down);
            }

            if (up.z > map_dim.z) return false;
            exit.pos = up;
            cost = adj[iadj].cost * (up.z - cur.z + 1);// + jump_cost * (up.z - cur.z);
            return Passable3DJump::can_jump_to(cur, up, clearance, max_up);
        }

        cost = adj[iadj].cost;
        exit.pos = dst;
        if (!t_map::isSolid(dst.x, dst.y, dst.z - 1))
            return false;
        for (int i=0; i<clearance; i++)
        {
            if (t_map::isSolid(dst.x, dst.y, dst.z + i) ||
                t_map::isSolid(dst.x, cur.y, cur.z + i) ||
                t_map::isSolid(cur.x, dst.y, cur.z + i))
            {
                return false;
            }
        }
        return true;
    }
};

template<class Passable, int adj_size>
Vec3i* get_path(const struct Vec3i& start, const struct Vec3i& end,
                 size_t& len)
{
    len = 0;
    IF_ASSERT(!is_boxed_position(start) || !is_boxed_position(end))
        return NULL;
    if (t_map::isSolid(end))
    {
        PATH_PRINT("Not pathing; endpoint is solid (unreachable)\n");
        return NULL;
    }
    if (manhattan_distance(start, end) > MAX_PATH_DISTANCE)
    {
        PATH_PRINT("Not pathing, too far away\n");
        return NULL;
    }

    // TODO --
    // 3d
    // Global open/closed buffer, init/teardown (minimize allocations)
    // sqrtf lookups
    // different sized bounding boxes
    // handle corner blocks properly (cant cut through open diagonal, must walk around corners)
    // fix zig zag paths when start.x or start.y < end.x or end.y
    // hierarchical paths over 16x16 chunk nodes
    // terminating condition within loop (in case area is unreachable)

    #if PATHFINDING_VERBOSE
    PATH_PRINT("Finding path from:\n");
    PATH_PRINT("\t");
    vec3i_print(start);
    PATH_PRINT("to\n");
    PATH_PRINT("\t");
    vec3i_print(end);
    #endif

    size_t iopen = 0;
    size_t iclosed = 0;
    size_t mopen = OPEN_START_SIZE;
    size_t mclosed = CLOSED_START_SIZE;
    struct Node* open = (struct Node*)malloc(mopen * sizeof(struct Node));
    struct Node* closed = (struct Node*)malloc(mclosed * sizeof(struct Node));

    // setup the start node
    node_first(open_map, open[iopen++], start, end);

    bool needs_sort = false;
    while (1)
    {
        if (iopen >= OPEN_NODES_MAX || iclosed >= CLOSED_NODES_MAX)
        {   // the path is taking too long, give up
            iopen = 0;
            iclosed = 0;
            break;
        }
        // if the open list is empty, then we failed to find a path at all
        if (iopen == 0)
            break;
        if (needs_sort)
        {
            sort_nodes(open_map, open, iopen);
            needs_sort = false;
        }
        // get lowest cost node
        struct Node current = get_lowest_cost_node(open_map, open, iopen);

        // move to closed list
        #if PATHFINDING_DEBUG
        int ret =
        #endif
        add_node(closed_map, current, closed, mclosed, iclosed);
        #if PATHFINDING_DEBUG
        PATH_ASSERT(ret >= 0);
        #endif
        // if we found the target, break
        if (is_equal(current.pos, end))
            break;

        for (int i=0; i<adj_size; i++)
        {
            struct Node node = current;
            node.parent = current.id;
            int cost = 0;
            if (!Passable::is_passable(current, i, node, cost))
                continue;
            // update position, scores of node
            node_update(node, end, cost);
            // if in closed list, skip
            if (closed_map->contains(node.pos))
                continue;
            //if (get_node_pos_index(node.pos, closed, iclosed) >= 0)
                //continue;

            //int in_open = get_node_pos_index(node.pos, open, iopen);
            //if (in_open < 0)
            int in_open = open_map->get(node.pos);
            if (in_open < 0)
            {   // not found; add to open list
                #if PATHFINDING_DEBUG
                int ret =
                #endif
                add_node(open_map, node, open, mopen, iopen);
                #if PATHFINDING_DEBUG
                PATH_ASSERT(ret >= 0);
                #endif
                needs_sort = true;
            }
            else if (node_reassign(open[in_open], node.score.g, current.id))
            {   // update if g is better
                needs_sort = true;
            }
            #if PATHFINDING_DEBUG
            PATH_ASSERT(in_open < int(iopen));
            //if (in_open >= int(iopen))
                //printf("in_open out of bounds: %d >= %d\n", in_open, int(iopen));
            #endif
        }

    }

    struct Vec3i* path = construct_path(open, iopen, closed, iclosed, len);
    free(open);
    free(closed);
    closed_map->erase();
    open_map->erase();
    return path;
}

struct Vec3i* get_path_2d(const struct Vec3i& start, const struct Vec3i& end, size_t& len)
{
    if (start.z != end.z) return NULL;
    return get_path<Passable2D, 8>(start, end, len);
}

struct Vec3i* get_path_3d_air(const struct Vec3i& start, const struct Vec3i& end, size_t& len)
{
    return get_path<Passable3DAir, 26>(start, end, len);
}

struct Vec3i* get_path_3d_surface(const struct Vec3i& start, const struct Vec3i& end, size_t& len)
{   // stick to surfaces
    return get_path<Passable3DSurface, 10>(start, end, len);
}

struct Vec3i* get_path_3d_jump(const struct Vec3i& start, const struct Vec3i& end, size_t& len)
{   // stick to upper surfaces, allowing jumps
    if (!t_map::is_surface_block(start) ||
        !t_map::is_surface_block(end)) return NULL;
    return get_path<Passable3DJump, 8>(start, end, len);
}

void print_path(const struct Vec3i* path, size_t len)
{
    if (path == NULL)
    {
        printf("Empty path\n");
        return;
    }
    for (int i=0; i<int(len); i++)
    {
        printf("%d: ", i);
        vec3i_print(path[i]);
    }
}

#if DC_CLIENT
void draw_path(const struct Vec3i* path, size_t len)
{
    if (path == NULL) return;
    if (len < 2) return;
    const Color color = Color(0, 220, 0);
    const struct Vec3 offset = vec3_init(0.5f);
    for (size_t i=0; i<len-1; i++)
    {
        struct Vec3 a = vec3_add(vec3_init(path[i]), offset);
        struct Vec3 b = vec3_add(vec3_init(path[i+1]), offset);
        a = quadrant_translate_position(current_camera_position, a);
        b = quadrant_translate_position(current_camera_position, b);
        draw_line(color, a, b);
    }
}
#endif

#undef PATHFINDING_DEBUG
#undef PATH_ASSERT
}   // Path
