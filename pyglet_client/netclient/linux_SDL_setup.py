from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext

module1 = Extension('SDL',
                    #define_macros = [('MAJOR_VERSION', '1'),
                    #                 ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include',
                                    ' /usr/include/X11/extensions/',
                                    '/usr/include/SDL',
                                    '/usr/lib',     ],
                    libraries = ['SDL','GL','SDL','GLU'],

                    library_dirs = ['/usr/X11R6/lib','usr/lib'],
                    extra_compile_args = ['-I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT'], # $(shell sdl-config --cflags)
                    #extra_link_args = '',
                    sources = ['gl_lib/SDL_functions.c', 'gl_lib/SDL_interface.pyx'])

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [module1]
    #ext_modules = [module1, Extension("test2", ["test2.pyx"]),]#+ cythonize("*.pyx")
)
