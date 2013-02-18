#include "pathfinding.hpp"

#include <stdlib.h>
#include <t_map/_interface.hpp>

namespace Path
{

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

static const size_t OPEN_START_SIZE = 128;
static const size_t CLOSED_START_SIZE = 128;

struct Adjacency
{
    int x,y,z;
    int cost;
};

static const struct Adjacency adj[8+9+9] = {
    // xy
    {  1,  0,  0, 10 },
    { -1,  0,  0, 10 },
    {  0,  1,  0, 10 },
    {  0, -1,  0, 10 },
    {  1,  1,  0, 14 },
    {  1, -1,  0, 14 },
    { -1,  1,  0, 14 },
    { -1, -1,  0, 14 },
    // xy +
    {  1,  0,  1, 14 },
    { -1,  0,  1, 14 },
    {  0,  1,  1, 14 },
    {  0, -1,  1, 14 },
    {  1,  1,  1, 18 },
    {  1, -1,  1, 18 },
    { -1,  1,  1, 18 },
    { -1, -1,  1, 18 },
    {  0,  0,  1, 10 },
    // xy -z
    {  1,  0, -1, 14 },
    { -1,  0, -1, 14 },
    {  0,  1, -1, 14 },
    {  0, -1, -1, 14 },
    {  1,  1, -1, 18 },
    {  1, -1, -1, 18 },
    { -1,  1, -1, 18 },
    { -1, -1, -1, 18 },
    {  0,  0, -1, 10 },
};

struct Score
{
    float g;
    float h;
    ALWAYS_INLINE float f()
    {
        return g + h;
    }
};

float euclidean_distance(const struct MapPos& start, const struct MapPos& end)
{
    int x = quadrant_translate_i(end.x, start.x) - end.x;
    int y = quadrant_translate_i(end.y, start.y) - end.y;
    return sqrtf(x*x + y*y);
}

struct Node
{
    struct MapPos pos;
    struct Score score;
    int id;
    int parent;

    void _update_score(float g, const struct MapPos& end)
    {
        this->score.g += g;
        this->score.h = euclidean_distance(this->pos, end);
    }

    void first(const struct MapPos& start, const struct MapPos& end)
    {   // init first node in the sequence
        this->pos = start;
        this->parent = -1;
        this->id = 0;
        this->_update_score(0, end);
    }

    void update(const struct Adjacency& adj, const struct MapPos& end)
    {
        this->pos.x = translate_point(this->pos.x + adj.x);
        this->pos.y = translate_point(this->pos.y + adj.y);
        this->pos.z = translate_point(this->pos.z + adj.z);
        this->_update_score(adj.cost, end);
    }

    bool reassign(float g, int parent)
    {   // if the new g score is better, take that parent
        if (g >= this->score.g)
            return false;
        this->score.g = g;
        this->parent = parent;
        return true;
    }

    void print()
    {
        printf("ID: %d; Parent: %d; Score: g=%0.2f, h=%0.2f; ", this->id,
               this->parent, this->score.g, this->score.h);
        print_pos(this->pos);
    }
};

static int compare_node_score(const void* a, const void* b)
{   // we want descending order, so b comes first
    return ((struct Node*)b)->score.f() - ((struct Node*)a)->score.f();
}

static ALWAYS_INLINE void sort_nodes(struct Node* nodes, size_t len)
{
    qsort(nodes, len, sizeof(struct Node), &compare_node_score);
}

static inline int add_node(struct Node& node, struct Node*& nodes,
                            size_t& max_len, size_t& len)
{
    if (len >= max_len)
    {
        struct Node* _nodes = (struct Node*)realloc(nodes, len*2*sizeof(struct Node));
        IF_ASSERT(_nodes == NULL) return -1;
        nodes = _nodes;
        max_len *= 2;
    }
    nodes[len] = node;
    node.id = len++;
    return len;
}

static inline int get_node_pos_index(const struct MapPos& pos,
                                     const Node* nodes, size_t len)
{
    for (size_t i=0; i<len; i++)
        if (is_equal(pos, nodes[i].pos))
            return int(i);
    return -1;
}

struct MapPos* get_path(const struct MapPos& start,
                        const struct MapPos& end, size_t& len)
{
    len = 0;
    if (start.z != end.z) return NULL;

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
    open[iopen++].first(start, end);
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
            node.update(adj[i], end);

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
                needs_sort = open[in_open].reassign(node.score.g, current.id);
            }
        }
    }

    len = 0;
    struct MapPos* path = NULL;
    if (iopen != 0)
    {   // walk path backwards, from last item on closed list
        // count length of path, allocate
        len = 1;
        int first = closed[iclosed-1].parent;
        while (first >= 0)
        {
            first = closed[first].parent;
            len++;
        }
        path = (struct MapPos*)malloc(len * sizeof(struct MapPos));

        // add to path array
        int i = len;
        first = closed[iclosed-1].parent;
        path[--i] = closed[iclosed-1].pos;
        while (first >= 0)
        {
            first = closed[first].parent;
            path[--i] = closed[first].pos;
        }
        GS_ASSERT(i == 0);
    }

    // teardown
    free(open);
    free(closed);
    return path;
}

void print_path(const struct MapPos* path, size_t len)
{
    for (size_t i=0; i<len; i++)
    {
        printf("%d: ", int(i));
        print_pos(path[i]);
    }
}

}   // Path
