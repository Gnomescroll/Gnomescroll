'''
File change monitor
Detects changes to files and responds as directed
'''

from hashlib import md5
from os.path import exists

class FileMonitor(object):

    def __init__(self, files=None, callbacks=None, interval=2000):
        if files is None:
            files = []
        self.files = files
        
        if callbacks is None:
            callbacks = [lambda: None] * len(files)
        self.callbacks = dict(zip(files, callbacks))

        self.interval = interval
        
        self.build_hashes()
        self.set_hash_map()

    def build_hashes(self):
        hashes = []
        for f in self.files:
            hash = self.get_hash(f)
            hashes.append(hash)
        self.hashes = hashes

    def get_hash(self, file):
        return md5(open(file).read())

    def set_hash_map(self):
        self.hash_map = dict(zip(self.files, self.hashes))

    def track(self, file):
        if file in self.files:
            return
        if not exists(file):
            print 'WARNING: attempting to monitor changes to nonexistant file %s' % (file,)
            return
        self.files.append(file)
        hash = self.get_hash(file)
        self.hashes.append(hash)
        self.hash_map[file] = hash

    def on_change(self, file, fn):
        self.callbacks[file] = fn

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
            hash = self.get_hash(file)
            if h != hash:
                self.hash_map[f] = hash
                if f in self.callbacks:
                    self.callbacks[f](f)
    
