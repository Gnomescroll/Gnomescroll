from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext
import subprocess
import platform

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

SYSTEM=platform.system()

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

libraries=['SDL', 'SDL_image']
import os

OS = os.uname()[0]
print "Platform: %s" %(OS)

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
    include_dirs =  []#["/usr/local/include/SDL"] #['/usr/include/SDL'], #"/usr/local/Cellar/sdl/include",
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

include_dirs += ["c_lib"]

debug = True
if debug == True:
    extra_compile_args+=["-g"]
    extra_link_args+=["-g"]


from distutils.unixccompiler import UnixCCompiler

debug = 1

if OS != "Darwin":
    print "Compiling Shared Libraries"
    comp = UnixCCompiler(verbose=True, force=True)
    s_lib=[]

    comp.set_include_dirs(include_dirs)
    comp.set_libraries(libraries)
    comp.set_library_dirs(library_dirs)
    comp.set_runtime_library_dirs(runtime_library_dirs)
    #comp.add_runtime_library_dir("./")

    comp.compile(
        sources = [ 'c_lib/c_lib.c',
                    'c_lib/texture_loader.c',
                    ],
        #output_dir="build",
        include_dirs= include_dirs,
        debug=0,
        extra_preargs= extra_compile_args,
        #extra_postargs= extra_compile_args
        )


    comp.link_shared_lib(
        objects = [ 'c_lib/c_lib.o',
                    'c_lib/texture_loader.o',
                    ],
        output_libname= "_c_lib",
        #output_dir="build",
        libraries=libraries,
        library_dirs=library_dirs,
        #runtime_library_dirs= runtime_library_dirs,
        debug=0,
        extra_preargs= extra_link_args,
        #extra_postargs= extra_link_args,
    )
else:
    print "Compiling Shared Libraries"
    print "Super OSX Build Process"
    comp = UnixCCompiler(verbose=True, force=True)
    s_lib=[]

    comp.set_include_dirs(include_dirs)
    comp.set_libraries(libraries)
    comp.set_library_dirs(library_dirs)
    comp.set_runtime_library_dirs(runtime_library_dirs)
    #comp.add_runtime_library_dir("./")

    comp.compile(
        sources = [ 'c_lib/c_lib.c',
                    'c_lib/texture_loader.c',
                    ],
        #output_dir="build",
        include_dirs= ["/usr/local/include/SDL"], #include_dirs,
        debug=0,
        extra_preargs= ["-D_GNU_SOURCE=1", "-D_THREAD_SAFE",],
        #extra_postargs= extra_compile_args
        )


    comp.link_shared_lib(
        objects = [ 'c_lib/c_lib.o',
                    'c_lib/texture_loader.o',
                    ],
        output_libname= "_c_lib",
        #output_dir="build",
        libraries= ["OpenGL", "SDLmain", "SDL", "GLEW"], #libraries,
        library_dirs= ["/usr/local/lib"], #library_dirs,
        #runtime_library_dirs= runtime_library_dirs,
        debug=0,
        extra_preargs= ["-Wl","-framework,Cocoa","-framework OpenGL"],
        #extra_postargs= extra_link_args,
    )

s_lib += ['_c_lib']

print "Compiling Python Modules"

SDL_gl = Extension('SDL.gl',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['SDL/SDL_functions.c',
                                'SDL/camera.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                'SDL/particle_functions.c',
                                'SDL/gl.pyx'],
                    )

SDL_hud = Extension('SDL.hud',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/hud.pyx',
                                'SDL/SDL_text.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                'SDL/hud/block_selector.c',
                                ]
                                )

SDL_input = Extension('SDL.input',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

terrain_map = Extension('cube_lib.terrain_map',
                    include_dirs = include_dirs,
                    libraries = libraries+s_lib ,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            #'cube_lib/t_map.c',
                            #'cube_lib/t_properties.c',
                            'cube_lib/t_vbo.c',
                            'cube_lib/t_viz.c'
                            ]
                    )

vox_lib = Extension('vox_lib',
                    include_dirs = include_dirs,
                    libraries = libraries,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'vox_lib/vox_functions.c',
                    'vox_lib/vox_lib.pyx',
                    ]
                                )
## c_lib

ray_trace = Extension('c_lib.ray_trace',
                    include_dirs = include_dirs,
                    libraries = libraries+s_lib,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/_ray_trace.pyx',
                    #'c_lib/ray_trace/ray_trace.c',
                    ]
                                )

init_c_lib = Extension('init_c_lib',
                    include_dirs = include_dirs,
                    libraries = libraries+s_lib,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/init_c_lib.pyx',
                    ]
                                )

c_lib_objects = Extension('c_lib.c_lib_objects',
                    include_dirs = include_dirs,
                    libraries = libraries+s_lib,
                    library_dirs = library_dirs,
                    runtime_library_dirs =  runtime_library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/c_lib_objects.pyx',
                    ]
                                )

#x = distutils.ccompiler.CCompiler()

#import distutils.unixcompiler

'''
from distutils.unixccompiler import UnixCCompiler

comp = UnixCCompiler()

for i in libraries:
    comp.add_library(i)
for i in include_dirs:
    comp.add_include_dir(i)

comp.compile(
    sources = ["vox_lib/vox_functions.c"],
    #output_dir="build",
    include_dirs= include_dirs,
    debug=0,
    extra_preargs= extra_compile_args,
    extra_postargs=None)


comp.link_shared_lib(
    ["vox_lib/vox_functions.o"],
    output_libname= "test2",
    #output_dir="build",
    libraries=None,
    library_dirs=None,
    runtime_library_dirs= ["./",],
    debug=0,
    extra_preargs= extra_link_args,
    extra_postargs=None,
)
'''

'''
comp.link("test", ["vox_lib/vox_functions.o"], "libTest",
    #include_dirs = include_dirs,
    #libraries = libraries, #SDL_image ?
    #extra_compile_args = extra_compile_args,
    #extra_link_args = extra_link_args,

    output_dir="test2/",
    libraries= libraries,
    library_dirs=None,

    runtime_library_dirs=None,
    export_symbols=None,
    debug=0,
    extra_preargs=None,
    extra_postargs=None,
    build_temp=None,
    target_lang=None)
'''

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [vox_lib, SDL_gl, SDL_input, SDL_hud, terrain_map, ray_trace, init_c_lib, c_lib_objects ], #+ cythonize("*.pyx")
)
