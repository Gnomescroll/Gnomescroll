#!/bin/env python

import os
import shutil
import subprocess
import pipes
import sys

def ensure_dir(f):
    d = os.path.dirname(f)
    if not os.path.exists(d):
        os.makedirs(d)

def copyfile(s,d):
    shutil.copy2(s,d)
    
def copyfolder(s,d):
    shutil.copytree(s,d)
    
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
        
def run(conf):
    # configure and compile build
    subprocess.call('python ./waf clean', shell=True)
    subprocess.call('python ./waf %s' % conf, shell=True)
    subprocess.call('python ./waf', shell=True)
    
    # create base dir for storing all versions
    base_path = 'C:\\gnomescroll\\%s\\' % conf 
    ensure_dir(base_path)

    version = get_version()
    build_name = "gnomescroll_windows_%d" % (version,)

    # create source directory
    build_path = os.path.join(base_path, build_name + '\\')
    if os.path.exists(build_path):
        shutil.rmtree(build_path)
    ensure_dir(build_path)
    
    # copy exe
    copyfile('./run.exe', os.path.join(build_path, 'gnomescroll.exe'))
    
    # copy settings
    settings_path = os.path.join(build_path, 'settings\\')
    ensure_dir(settings_path)
    copyfile('./settings/lua_library.lua', os.path.join(settings_path, 'lua_library.lua'))
    if conf == 'production':
    	settings_file = os.path.join(settings_path, 'settings.lua')
        copyfile('./settings/production.lua', settings_file)
        subprocess.call('unix2dos %s' % (settings_file,), shell=True)
    elif conf == 'configure':
    	settings_file = os.path.join(settings_path, 'dev.lua')
        copyfile('./settings/dev.lua', settings_file)
        subprocess.call('unix2dos %s' % (settings_file,), shell=True)

    # TODO -- call unix2dos or "/usr/bin/perl -pi s/\n/\r\n/" to convert the settings file
    # Check that the lua interpreter handles crlf properly
        
    # create screenshot folder
    ensure_dir(os.path.join(build_path, 'screenshot\\'))
    
    # copy media folder
    shutil.copytree('./media/', os.path.join(build_path, 'media\\'))
    
    #create log folder
    ensure_dir(os.path.join(build_path, 'log\\'))
    
    #copy DLLs
    dlls = [
        'glew32.dll',
        'lua51.dll',
        'OpenAL32.dll',
        'wrap_oal.dll',
        'zlib1.dll',
        'SDL.dll',
        'SDL_image.dll',
        'libpng12-0.dll',
        'libgcc_s_dw2-1.dll',
        'libstdc++-6.dll',
        'msvcr100.dll',
        'Assimp32.dll',
        ]
        
    for dll in dlls:
        dll_src_path = os.path.join('../lib/win32/dll/', dll)
        assert os.path.exists(dll_src_path)
        copyfile(dll_src_path, os.path.join(build_path, dll))
    
    awesomium_dll_path = '../lib/win32/awesomium/bin/release'
    for dll in os.listdir(awesomium_dll_path):
        dll_src_path = os.path.join(awesomium_dll_path, dll)
        assert os.path.exists(dll_src_path)
        if os.path.isdir(dll_src_path):
            copyfolder(dll_src_path, os.path.join(build_path, dll))
        else:
            copyfile(dll_src_path, os.path.join(build_path, dll))
        
    # only zip and copy for production builds
    if conf != 'production':
        return
        
    # zip it into the dropbox folder
    dropbox_path = "C:\\Documents and Settings\\Administrator\\My Documents\\Dropbox\\Public\\"
    if not os.path.exists(dropbox_path):
        print "Could not find dropbox path:", dropbox_path
        print "You will have to manually compress and copy the folder to dropbox"
        return
        
    zip_path = os.path.join(dropbox_path, build_name + '.zip')
    if os.path.exists(zip_path):
        os.remove(zip_path)
    
    # set working directory to where the build folder is
    curdir = os.path.abspath(os.curdir)
    os.chdir(build_path)
    # execute zip
    subprocess.call('zip -r %s *' % (pipes.quote(zip_path),), shell=True)
    os.chdir(curdir)
        
if __name__ == '__main__':
    conf = 'production'
    if len(sys.argv) > 1:
        conf = sys.argv[1]
    if conf not in ['production', 'configure']:
        print "Error, unrecognized configuration %s" % conf
        sys.exit(1)
    run(conf)
