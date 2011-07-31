
#from distutils.core import setup
#from distutils.extension import Extension
#from Cython.Distutils import build_ext

#setup(
    #cmdclass = {'build_ext': build_ext},
    #ext_modules = [Extension("world", ["world.pyx"])]
#)

from distutils.core import setup, Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

extra_compile_args=["-g"]
extra_link_args=["-g"]

terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = ['/usr/local/include',
                                    '/usr/lib',     ],
                    libraries = [], #SDL_image ?
                    library_dirs = ['usr/lib'],
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            'cube_lib/t_map.c',]
                    )

'''
setup(
    name = "test app",
    ext_modules = [cube_lib_terrain_map] + cythonize("*.pyx"),
)
'''

setup(
    name = "dc_mmo server",
    cmdclass = {'build_ext': build_ext},
    ext_modules = [terrain_map, ]+ cythonize("*.pyx")
)


#from distutils.core import setup
#from distutils.extension import Extension
#from Cython.Distutils import build_ext

#setup(
    #cmdclass = {'build_ext': build_ext},
    #ext_modules = [
        #Extension("world", ["world.pyx"])
    #]
#)
