
#from distutils.core import setup
#from distutils.extension import Extension
#from Cython.Distutils import build_ext

#setup(
    #cmdclass = {'build_ext': build_ext},
    #ext_modules = [Extension("world", ["world.pyx"])]
#)

print "Building Admin Client Modules"
import os
OS = os.uname()[0]
print "Platform: %s" %(OS)

from distutils.core import setup, Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext
import subprocess
import platform

SYSTEM=platform.system()
libraries=['SDL', 'SDL_image']

SDL_CFLAGS=""
SDL_LDFLAGS=""

try:
    SDL_CFLAGS=subprocess.Popen(['sdl-config', '--cflags'], stdout=subprocess.PIPE).communicate()[0]
    SDL_LDFLAGS=subprocess.Popen(['sdl-config', '--libs'], stdout=subprocess.PIPE).communicate()[0]
    SDLFLAGS = subprocess.Popen(['sdl-config', '--cflags', '--libs', ], stdout=subprocess.PIPE).communicate()[0]
    print "SDL_CFLAGS= %s" % SDL_CFLAGS
    print "SDL_LDFLAGS= %s" % SDL_LDFLAGS
    print "SDLFLAGS= %s" % SDLFLAGS
except WindowsError:
    # sdl-config is a shell script, windows users will have to provide the path.
    pass

extra_compile_args=[SDL_CFLAGS]
extra_link_args=[SDL_LDFLAGS]


if SYSTEM == 'Windows':
    #libraries+=['GLee','opengl32','glu32', 'GLEW',] # 'mega']
    libraries+=['opengl32','glu32', 'GLEW',]
    include_dirs = ['/usr/include/SDL']
    runtime_library_dirs = ["./"]
    library_dirs = ["./"]
elif OS == "Darwin":
    libraries =["GLEW"]#, 'SDL_image', 'SDL', 'SDLmain'] #, "SDLmain"] # 'GL','GLU',
    #extra_link_args += ["-framework OpenGL", "-framework SDL"]
    extra_link_args = [] #["-framework OpenGL"] #["-framework Cocoa","-framework SDL", "-framework SDL_image"]#  "-framework OpenGL" #"-framework Carbon", "-framework SDL", "-framework SDL_image",
    include_dirs =  ["./"]#["/usr/local/include/SDL"] #['/usr/include/SDL'], #"/usr/local/Cellar/sdl/include",
    runtime_library_dirs = ["./"]
    library_dirs = [] #, "/usr/local/lib"]
    #extra_compile_args += ["-arch i386 -arch ppc"]
    #extra_link_args += ["-arch i386 -arch ppc"]
elif OS == "Linux":
    libraries+=['GL','GLU', 'GLEW',] # 'mega']
    include_dirs = ['/usr/include/SDL']
    runtime_library_dirs = ["./"]
    library_dirs = ["./"]
else:
    print "Platform unknown: %s" %(OS)
    print "Error!"
    exit()

extra_compile_args+=["-g"]
extra_link_args+=["-g"]

alt_lib = ['rt']
if OS == "Darwin":
    alt_lib = []

SDL_gl = Extension('SDL.gl',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['SDL/SDL_functions.c',
                                'SDL/gl.pyx'],
                    )

setup(
    name = "dc_mmo admin client",
    cmdclass = {'build_ext': build_ext},
    ext_modules = [SDL_gl]+ cythonize("*.pyx")
)
