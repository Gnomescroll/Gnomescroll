
import c_lib.c_lib_sdl

p_on = False
verbrose = True #only print states that take more than 1 ms

def get_ticks():
    return c_lib.c_lib_sdl.get_ticks()

class Profiler:

    def __init__(self,):
        self.frame_id = 0

    def start_frame(self):
        self.frame_id += 1
        self.id = 0
        self.times = []
        self.label = []

    def event(self, description):
        global p_on
        if p_on:
            self.times.insert(self.id, get_ticks())
            self.label.insert(self.id, description)
            self.id +=1

    def finish_frame(self):
        global p_on,verbose
        if p_on:
            self.times.insert(self.id, get_ticks())
            print "Frame number: %i, total time= %i" % (self.frame_id, self.times[self.id] - self.times[0])
            for i in range(0, self.id):
                self.times[i] = self.times[i+1] -self.times[i]
            for i in range(0, self.id):
                pass
                if verbrose == True:
                    print "%s: %i ms" % (self.label[i], self.times[i])
                else:
                    if self.times[i] > 1:
                        print "%s: %i ms" % (self.label[i], self.times[i])
            print ""

    def flush_to_disc(self):
        pass

P = Profiler()
