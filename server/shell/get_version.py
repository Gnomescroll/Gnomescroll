#!/usr/bin/env python

def get_version():
    with open('../src/c_lib/common/version.h') as f:
        r = f.readlines()
        r = [g for g in r if 'GS_VERSION' in g]
        assert len(r) == 1
        v = r[0].strip()
        version = v.split(' ')[-1]
        return int(version)

if __name__ == '__main__':
    import sys
    sys.stdout.write(str(get_version()))
