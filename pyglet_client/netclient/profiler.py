
import SDL.gl

def get_ticks():
    return SDL.gl.get_ticks()

class Profiler:

    def __init__(self,):
        self.frame_id = 0

    def start_frame(self):
        self.frame_id += 1
        self.id = 0
        self.times = []
        self.label = []

    def event(self, description):
        self.times.insert(self.id, get_ticks())
        self.label.insert(self.id, description)
        self.id +=1

    def finish_frame(self):
        self.times.insert(self.id, get_ticks())
        #print "Frame number: %i, total time= %i" % (self.frame_id, self.times[self.id] - self.times[0])
        for i in range(0, self.id):
            self.times[i] = self.times[i+1] -self.times[i]
        for i in range(0, self.id):
            pass
            #print "%s: %i ms" % (self.label[i], self.times[i])
        #print ""

    def flush_to_disc(self):
        pass

P = Profiler()
