#!/bin/env python

import os
import shutil
import subprocess
import pipes

def ensure_dir(f):
    d = os.path.dirname(f)
    if not os.path.exists(d):
        os.makedirs(d)

def copyfile(s,d):
    shutil.copy2(s,d)
    
def get_version():
    with open('../src/c_lib/common/version.h') as f:
        r = f.readlines()
        r = [line.strip() for line in r if 'GS_VERSION' in line]
        assert len(r) == 1
        r = r[0]
        pieces = r.split(' ')
        assert len(pieces) == 3
        version = int(pieces[-1])
        return version
        
def run():
    # configure and compile build
    subprocess.call('python ./waf clean', shell=True)
    subprocess.call('python ./waf configure', shell=True)
    subprocess.call('python ./waf', shell=True)
    
    # create base dir for storing all versions
    base_path = 'C:\\gnomescroll_server\\' 
    ensure_dir(base_path)

    version = get_version()
    build_name = "gnomescroll_server_windows_%d" % (version,)

    # create source directory
    build_path = os.path.join(base_path, build_name + '\\')
    if os.path.exists(build_path):
        shutil.rmtree(build_path)
    ensure_dir(build_path)
    
    # copy exe
    copyfile('./build/run.exe', os.path.join(build_path, 'gnomescroll_server.exe'))
    
    # copy settings
    settings_path = os.path.join(build_path, 'settings\\')
    ensure_dir(settings_path)
    copyfile('./settings/lua_library.lua', os.path.join(settings_path, 'lua_library.lua'))
    copyfile('./settings/localhost.lua', os.path.join(settings_path, 'localhost.lua'))

    # create screenshot folder
    ensure_dir(os.path.join(build_path, 'screenshot\\'))
    
    # copy media folder
    shutil.copytree('./media/', os.path.join(build_path, 'media\\'))
    
    #create log folder
    ensure_dir(os.path.join(build_path, 'log\\'))
    
    #copy DLLs
    dlls = [
        'lua51.dll',
        'zlib1.dll',
        'libpng12-0.dll'
        ]
        
    for dll in dlls:
        copyfile(os.path.join('../lib/win32/dll/', dll), os.path.join(build_path, dll))
        
        
if __name__ == '__main__':
    run()
