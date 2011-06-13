from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

module1 = Extension('SDL',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU', 'SDL_image'], #SDL_image ?

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT'], # $(shell sdl-config --cflags)
                    #extra_link_args = '',
                    sources = ['gl_lib/SDL_functions.c',
                                'gl_lib/camera.c',
                                'gl_lib/draw.c',
                                'gl_lib/input.c',
                                'gl_lib/vbo_manager.c',
                                'gl_lib/texture_loader.c',
                                'gl_lib/SDL_interface.pyx'])

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [module1]
    #ext_modules = [module1, Extension("test2", ["test2.pyx"]),]#+ cythonize("*.pyx")
)
