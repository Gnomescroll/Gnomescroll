#include "frame_graph.hpp"

namespace Profiling
{

FrameGraph* frame_graph = NULL;

void init_frame_graph()
{
    if (frame_graph == NULL)
        frame_graph = new Profiling::FrameGraph;
}

void teardown_frame_graph()
{
    if (frame_graph != NULL)
        delete frame_graph;
}

}
