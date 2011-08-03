from distutils.core import setup, Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

import os

os.chdir(r"C:\cygwin\home\Administrator\test2\netclient\ ")
print os.getcwd()

extra_compile_args=[]
extra_link_args=["-L."]


    SDL_compile_args = []
    include_dirs = ["C:\SDL-1.2.14\include",
                    "C:\Program Files\Microsoft Visual Studio 9.0\VC\include",
                    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include",
                    "C:\SDL_image-1.2.10\include",]
                    #"C:\glee5.4\include",]
    library_dirs = ['./',
    './clib/',
    'C:\Program Files\Microsoft Visual Studio 9.0\VC\lib',
    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib",
    "C:\SDL-1.2.14\lib",
    "C:\SDL_image-1.2.10\lib",
    "C:\glee5.4\lib",]
    #libraries = ['SDL', 'SDL_image'] #'glut',
    libraries = ['glu32','Opengl32', 'GLee'] + ['SDL', 'SDL_image'] #, 'GLee' #'glut',
    extra_link_args=[]

SDL_gl = Extension('SDL.gl',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = libraries,

                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args + extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = ['SDL/SDL_functions.c',
                                'SDL/camera.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                'SDL/gl.pyx'],
                    )

SDL_input = Extension('SDL.input',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = libraries,

                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args + extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

SDL_hud = Extension('SDL.hud',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = libraries,

                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args + extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/hud.pyx',
                                'SDL/SDL_text.c',]
                                )

cube_lib_VBO = Extension('cube_lib.VBO',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = libraries,

                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args+extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/VBO.pyx',
                    'SDL/draw_functions.c',
                    'cube_lib/draw_terrain.c']
                    )

terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/lib'],
                    libraries = [],
                    library_dirs = ['usr/lib'],
                    extra_compile_args = []+extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx']
                    )

map_chunk_manager = Extension('cube_lib.map_chunk_manager',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/lib'],
                    libraries = [],
                    library_dirs = ['usr/lib'],
                    extra_compile_args = []+extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/map_chunk_manager.pyx']
                    )


vox_lib = Extension('vox_lib',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = libraries,

                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args+extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [
                    'vox_lib/vox_functions.c',
                     'vox_lib/vox_lib.pyx',]
                                )


loader = Extension('loader2', sources = ['loader.pyx',])


setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [loader, vox_lib, SDL_gl, SDL_input, SDL_hud, cube_lib_VBO, terrain_map, map_chunk_manager] # + cythonize("*.pyx")
    #ext_modules = [module1, Extension("test2", ["test2.pyx"]),]#+ cythonize("*.pyx")
)

print "Done"
raw_input()
