#include "frame_graph.hpp"

namespace Profiling
{

FrameGraph* frame_graph = NULL;

void init_frame_graph()
{
    GS_ASSERT(frame_graph == NULL);
    frame_graph = new Profiling::FrameGraph;
}

void teardown_frame_graph()
{
    delete frame_graph;
}

}   // Profiling
