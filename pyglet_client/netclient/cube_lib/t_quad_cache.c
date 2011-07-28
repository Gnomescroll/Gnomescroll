cdef Chunk_scratch* chunk_scratch
chunk_scratch = <Chunk_scratch *>malloc(sizeof(Chunk_scratch))

cdef Quad* quad_cache
quad_cache = <Quad *>malloc(max_cubes*6 * sizeof(Quad))
#quad_cache = <Quad *>malloc(1024*6 * sizeof(Quad))

#cdef Quad_cache quad_cache2

#print "size = " + str(sizeof(max_cubes*6))
#quad_cache = <Quad *>malloc(max_cubes*6*sizeof(Quad))

cdef float * v_index
v_index = <float*> malloc(72*sizeof(float))

l = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    ]

for i in range(0, 72):
    v_index[i] = float(l[i])

cpdef test():
    global v_index, quad_cache
    cdef int i
    print str(max_cubes)
    for i in range(0, max_cubes*6):
        print str(i)
        #print str(quad_cache[i].vertex.r)
        quad_cache[i].vertex[0].y = 5

def init_quad_cache():
    global v_index, quad_cache
    #quad_cache = <Quad *>malloc(max_cubes*6 * sizeof(Quad))
    #print "max cubes= " + str(max_cubes)
    #print "Size of Quad=" + str(sizeof(Quad))
    cdef Quad* quad
    cdef Vertex* vertex
    cdef int i,j,k,index
    for k in range(0, max_cubes):
        for i in range(0,6):
            quad = &quad_cache[6*k+i]
            for j in range(0,4):
                #print "(%i,%i,%i)" % (k,i,j)
                index = 12*i + 3*j
                vi = 6*k+i
                vertex = &quad.vertex[j]
                #vertices
                quad_cache[6*k+i].vertex[j].x = v_index[index + 0]
                quad_cache[6*k+i].vertex[j].y = v_index[index + 1]
                quad_cache[6*k+i].vertex[j].z = v_index[index + 2]
                ##vertex.x = v_index[index + 0]
                ##vertex.y = v_index[index + 1]
                ##vertex.z = v_index[index + 2]
                #colors
                vertex.r = 255
                vertex.g = 255
                vertex.b = 255
                vertex.a = 255
                #tex
                #set_tex(j, vertex, 0, 0)

                tx,ty = get_cube_texture(k, i, j) #tile_id, side, vert_num

                vertex.tx = tx
                vertex.ty = ty
                #print "(%i,%i,%i)" % (k,i,j)
                #print "(tx= %f,ty= %f)" %(quad_cache[6*k+i].vertex[j].tx,quad_cache[6*k+i].vertex[j].ty)
    print "done"
