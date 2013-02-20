#include "pathfinding.hpp"

#include <stdlib.h>
#include <t_map/_interface.hpp>
#include <common/common.hpp>
#if DC_CLIENT
# include <SDL/draw_functions.hpp>
#endif

namespace Path
{

#define PATHFINDING_DEBUG 1
#if PATHFINDING_DEBUG
# define PATH_ASSERT(x) GS_ASSERT(x)
#else
# define PATH_ASSERT(x)
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

// max distance between 2 nodes (manhattan)
static const int MAX_PATH_DISTANCE = 128;
static const size_t OPEN_START_SIZE = 128;
static const size_t CLOSED_START_SIZE = 128;

struct Adjacency
{
    int x,y,z;
    int cost;
};

static const struct Adjacency adj[8+9+9] = {
    // xy
    {  1,  0,  0, 100 },
    { -1,  0,  0, 100 },
    {  0,  1,  0, 100 },
    {  0, -1,  0, 100 },
    {  1,  1,  0, 141 },
    {  1, -1,  0, 141 },
    { -1,  1,  0, 141 },
    { -1, -1,  0, 141 },
    // xy +z
    {  1,  0,  1, 141 },
    { -1,  0,  1, 141 },
    {  0,  1,  1, 141 },
    {  0, -1,  1, 141 },
    {  1,  1,  1, 173 },
    {  1, -1,  1, 173 },
    { -1,  1,  1, 173 },
    { -1, -1,  1, 173 },
    {  0,  0,  1, 100 },
    // xy -z
    {  1,  0, -1, 141 },
    { -1,  0, -1, 141 },
    {  0,  1, -1, 141 },
    {  0, -1, -1, 141 },
    {  1,  1, -1, 173 },
    {  1, -1, -1, 173 },
    { -1,  1, -1, 173 },
    { -1, -1, -1, 173 },
    {  0,  0, -1, 100 },
};

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
    struct MapPos pos;
    struct Score score;
};

static inline void _node_update_score(struct Node& node, int g)
{
    PATH_ASSERT(g != node.score.g);
    node.score.g = g;
    node.score.f = node.score.g + node.score.h;
}

static inline void _node_set_position(struct Node& node, const struct MapPos& pos, const struct MapPos& end)
{
    node.pos = pos;
    node.score.h = euclidean_distance_2d(node.pos, end) * 100;
}

static inline void node_first(struct Node& node, const struct MapPos& start, const struct MapPos& end)
{   // init first node in the sequence
    node.parent = -1;
    node.id = 0;
    node.score.g = -1;
    _node_set_position(node, start, end);
    _node_update_score(node, 0);
}

static inline void node_update(struct Node& node, const struct Adjacency& adj, const struct MapPos& end)
{
    node.pos.x = translate_point(node.pos.x + adj.x);
    node.pos.y = translate_point(node.pos.y + adj.y);
    node.pos.z = translate_point(node.pos.z + adj.z);
    _node_set_position(node, node.pos, end);
    _node_update_score(node, node.score.g + adj.cost);
}

static inline bool node_reassign(struct Node& node, int g, int parent)
{   // if the new g score is better, take that parent
    if (g >= node.score.g)
        return false;
    _node_update_score(node, g);
    node.parent = parent;
    return true;
}

static inline void node_print(const struct Node& node)
{
    printf("ID: %03d; Parent: %03d; Score: f=%04d; g=%04d, h=%04d; ", node.id, node.parent,
           node.score.f, node.score.g, node.score.h);
    print_pos(node.pos);
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
    if (d == 0) d = b->score.h - a->score.h;
    return (d == 0) ? 0 : ((d < 0) ? -1 : 1);
}

static ALWAYS_INLINE void sort_nodes(struct Node* nodes, size_t len)
{
    qsort(nodes, len, sizeof(nodes[0]), &compare_node_score);
    for (int i=0; i<int(len); i++) nodes[i].id = i;
}

static inline int add_node(struct Node& node, struct Node*& nodes,
                            size_t& max_len, size_t& len)
{
    if (len >= max_len)
    {
        struct Node* _nodes = (struct Node*)realloc(nodes, 2*max_len*sizeof(struct Node));
        IF_ASSERT(_nodes == NULL) return -1;
        max_len *= 2;
        printf("Reallocing nodelist from %d to %d\n", int(len), int(max_len));
        nodes = _nodes;
    }
    node.id = len;
    nodes[len++] = node;
    return len-1;
}

static inline int get_node_pos_index(const struct MapPos& pos,
                                     const Node* nodes, size_t len)
{
    for (size_t i=0; i<len; i++)
        if (is_equal(pos, nodes[i].pos))
            return int(i);
    return -1;
}

static struct MapPos add_pos_adj(const struct MapPos& pos, const struct Adjacency& adj)
{
    struct MapPos p;
    p.x = pos.x + adj.x;
    p.y = pos.y + adj.y;
    p.z = pos.z + adj.z;
    return p;
}

struct MapPos* get_path(const struct MapPos& start,
                        const struct MapPos& end, size_t& len)
{
    IF_ASSERT(!is_boxed_position(start) || !is_boxed_position(end)) return NULL;
    len = 0;
    // hacks to prevent lockup until 3D is implemented
    if (start.z != end.z) return NULL;
    if (t_map::isSolid(end)) return NULL;
    //if (manhattan_distance(start, end) > MAX_PATH_DISTANCE) return NULL;

    // TODO --
    // 3d
    // Global open/closed buffer, init/teardown (minimize allocations)
    // sqrtf lookups
    // different sized bounding boxes
    // handle corner blocks properly (cant cut through open diagonal, must walk around corners)
    // fix zig zag paths when start.x or start.y < end.x or end.y
    // hierarchical paths over 16x16 chunk nodes
    // terminating condition within loop (in case area is unreachable)

    printf("Finding path from:\n");
    printf("\t");
    print_pos(start);
    printf("to\n");
    printf("\t");
    print_pos(end);

    size_t iopen = 0;
    size_t iclosed = 0;
    size_t mopen = OPEN_START_SIZE;
    size_t mclosed = CLOSED_START_SIZE;
    struct Node* open = (struct Node*)malloc(mopen * sizeof(struct Node));
    struct Node* closed = (struct Node*)malloc(mclosed * sizeof(struct Node));
    node_first(open[iopen], start, end);
    iopen++;
    bool needs_sort = false;

    while (1)
    {
        // if the open list is empty, then we failed to find a path at all
        if (iopen == 0)
            break;
        if (needs_sort)
        {
            sort_nodes(open, iopen);
            needs_sort = false;
        }

        // get lowest cost node list
        struct Node current = open[--iopen];

        // move to closed list
        add_node(current, closed, mclosed, iclosed);
        // if we found the target, break
        if (is_equal(current.pos, end))
            break;

        for (int i=0; i<8; i++)
        {   // iterate adjacent squares
            struct Node node = current;
            node_update(node, adj[i], end);

            // if blocked or in closed list, skip
            if (t_map::isSolid(node.pos) ||
                get_node_pos_index(node.pos, closed, iclosed) >= 0)
            {
                continue;
            }

            int in_open = get_node_pos_index(node.pos, open, iopen);
            if (in_open < 0)
            {   // not found; add to open list
                node.parent = current.id;
                add_node(node, open, mopen, iopen);
                needs_sort = true;
            }
            else
            {   // update if g is better
                bool changed = node_reassign(open[in_open], node.score.g, current.id);
                if (changed)
                    needs_sort = true;
            }
        }
    }

    len = 0;
    struct MapPos* path = NULL;
    if (iopen != 0)
    {   // walk path backwards, from last item on closed list
        // count length of path, allocate
        len = 1;
        closed[0].parent = -1;
        int first = closed[iclosed-1].parent;
        while (first >= 0)
        {
            first = closed[first].parent;
            len++;
        }
        path = (struct MapPos*)malloc(len * sizeof(struct MapPos));

        // add to path array
        int i = len;
        first = closed[iclosed-1].id;
        while (first >= 0)
        {
            path[--i] = closed[first].pos;
            first = closed[first].parent;
        }
    }

    // teardown
    free(open);
    free(closed);
    return path;
}

void print_path(const struct MapPos* path, size_t len)
{
    if (path == NULL) return;
    for (int i=0; i<int(len); i++)
    {
        printf("%d: ", i);
        print_pos(path[i]);
    }
}

#if DC_CLIENT
void draw_path(const struct MapPos* path, size_t len)
{
    if (path == NULL) return;
    if (len < 2) return;
    const Color color = Color(0, 220, 0);
    const struct Vec3 offset = vec3_init(0.5f, 0.5f, 0.05f);
    for (size_t i=0; i<len-1; i++)
    {
        struct Vec3 a = vec3_add(vec3_init(path[i]), offset);
        struct Vec3 b = vec3_add(vec3_init(path[i+1]), offset);
        a.z = 128.05f;
        b.z = 128.05f;
        a = quadrant_translate_position(current_camera_position, a);
        b = quadrant_translate_position(current_camera_position, b);
        draw_line(color, a, b);
    }
}
#endif

}   // Path
