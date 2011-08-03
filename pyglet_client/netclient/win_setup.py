from distutils.core import setup, Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

import os

os.chdir(r"C:\cygwin\home\Administrator\dc_mmo\pyglet_client\netclient\ ")
print os.getcwd()

compiler = 'VC6'
if compiler == 'VC6':
    extra_compile_args=[]
    extra_link_args=["-L."]

    SDL_compile_args = []
    include_dirs = ["C:\SDL-1.2.14\include",
                    "C:\Program Files\Microsoft Visual Studio 9.0\VC\include",
                    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include",
                    "C:\SDL_image-1.2.10\include",]
                    #"C:\glee5.4\include",]
    library_dirs =  ['./',
    'C:\Program Files\Microsoft Visual Studio 9.0\VC\lib',
    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib",
    "C:\SDL-1.2.14\lib",
    "C:\SDL_image-1.2.10\lib",
    "C:\glee5.4\lib",]
    #libraries = ['SDL', 'SDL_image'] #'glut',
    libraries = ['glu32','Opengl32', 'GLee'] + ['SDL', 'SDL_image'],
    extra_link_args=[]
else:
    pass ## put your directory stuff heres
    extra_compile_args=[]
    extra_link_args=["-L."]

    SDL_compile_args = []

    ##EDIT DIRECTORIES BELOW FOR MINGE
    include_dirs = ["C:\SDL-1.2.14\include",
                    "C:\Program Files\Microsoft Visual Studio 9.0\VC\include",
                    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include",
                    "C:\SDL_image-1.2.10\include",]
                    #"C:\glee5.4\include",]  #may need to do something with this

    ##EDIT DIRECTORIES BELONG FOR MINGW
    library_dirs =  ['./',
    'C:\Program Files\Microsoft Visual Studio 9.0\VC\lib',
    "C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib",
    "C:\SDL-1.2.14\lib",
    "C:\SDL_image-1.2.10\lib",
    "C:\glee5.4\lib",]
    libraries = ['glu32','Opengl32', 'GLee'] + ['SDL', 'SDL_image'],

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
                                'SDL/particle_functions.c',
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
                                'SDL/SDL_text.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',]
                                )

terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/lib'],
                    libraries = [],
                    library_dirs = library_dirs,
                    extra_compile_args = SDL_compile_args + extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            'cube_lib/t_map.c',
                            'cube_lib/t_properties.c',
                            'cube_lib/t_vbo.c',]
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
    ext_modules = [loader, vox_lib, SDL_gl, SDL_input, SDL_hud, terrain_map] # + cythonize("*.pyx")
)

print "Done"
raw_input()
