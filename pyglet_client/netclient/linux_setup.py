from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

SDL_gl = Extension('SDL.gl',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT'], # $(shell sdl-config --cflags)
                    #extra_link_args = '',
                    sources = ['SDL/SDL_functions.c',
                                'SDL/camera.c',
                                'SDL/draw_functions.c',
                                'SDL/vbo_manager.c',
                                'SDL/texture_loader.c',
                                'SDL/gl.pyx'])

SDL_input = Extension('SDL.input',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT'], # $(shell sdl-config --cflags)
                    #extra_link_args = '',
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

cube_lib_VBO = Extension('cube_lib.VBO',
                    include_dirs = ['/usr/local/include',
                    'gl_lib'],
                    libraries = [],
                    library_dirs = [],
                    extra_compile_args = [],
                    extra_link_args = [],
                    sources = ['cube_lib/VBO.pyx'])

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [SDL_gl, SDL_input, cube_lib_VBO] + cythonize("*.pyx")
    #ext_modules = [module1, Extension("test2", ["test2.pyx"]),]#+ cythonize("*.pyx")
)
