'''
f change monitor
Detects changes to files and responds as directed
'''

from hashlib import md5
from os.path import exists

class FileMonitor(object):

    def __init__(self, files=None, callbacks=None, interval=2000):
        self.files = []
        self.interval = interval

        self.hash_map = {}
        self.build_hashes(files)

        if callbacks is None:
            callbacks = [lambda: None] * len(files)
        self.callbacks = dict(zip(files, callbacks))


    def __call__(self):
        self.check()

    def build_hashes(self, files):
        for f in files:
            self.track(f)

    def get_hash(self, f):
        return md5(open(f).read()).digest()

    def track(self, f):
        if not exists(f):
            print 'WARNING: attempting to monitor changes to nonexistant file %s' % (f,)
            return
        self.files.append(f)
        hash = self.get_hash(f)
        self.hash_map[f] = hash

    def on_change(self, f, fn):
        self.callbacks[f] = fn

    def teardown(self, f):
        i = self.files.index(f)
        self.files.remove(f)
        self.hashes.pop(i)
        del self.hash_map[f]
        del self.callbacks[f]

    def check(self):
        for f, h in self.hash_map.items():
            if not exists(f):
                self.teardown(f)
                continue
            hash = self.get_hash(f)
            if h != hash:
                self.hash_map[f] = hash
                if f in self.callbacks:
                    self.callbacks[f](f)
    
