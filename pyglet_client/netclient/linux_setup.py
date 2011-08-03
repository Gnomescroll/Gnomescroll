from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

debug = True

if debug == True:
    extra_compile_args=["-g"]
    extra_link_args=["-g"]
else:
    extra_compile_args=[]
    extra_link_args=[]

SDL_gl = Extension('SDL.gl',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',
                                        ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT']+extra_compile_args, # $(shell sdl-config --cflags)
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
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image',], # "Chrome"], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib',"./"],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT']+extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

SDL_hud = Extension('SDL.hud',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image', 'SDL_ttf'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT']+extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/hud.pyx',
                                'SDL/SDL_text.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',]
                                )

cube_lib_terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    'gl_lib',
                                    '/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image'], #SDL_image ?
                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT']+extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            'cube_lib/t_map.c',
                            'cube_lib/t_properties.c',
                            'cube_lib/t_vbo.c',]
                    )

vox_lib = Extension('vox_lib',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT']+extra_compile_args, # $(shell sdl-config --cflags)
                    extra_link_args = extra_link_args,
                    sources = [
                    'vox_lib/vox_functions.c',
                     'vox_lib/vox_lib.pyx',]
                                )


setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [vox_lib, SDL_gl, SDL_input, SDL_hud, cube_lib_terrain_map, ], #+ cythonize("*.pyx")
)
