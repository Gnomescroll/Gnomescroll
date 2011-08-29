
#from distutils.core import setup
#from distutils.extension import Extension
#from Cython.Distutils import build_ext

#setup(
    #cmdclass = {'build_ext': build_ext},
    #ext_modules = [Extension("world", ["world.pyx"])]
#)

print "Building Server Modules"
import os
OS = os.uname()[0]
print "Platform: %s" %(OS)

from distutils.core import setup, Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

extra_compile_args=["-g"]
extra_link_args=["-g"]

include_dirs =  [] #['/usr/local/include', '/usr/lib',     ]
library_dirs = [] #'usr/lib']

terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs ,
                    libraries = [], #SDL_image ?
                    library_dirs = library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            'cube_lib/t_map.c',
                            'cube_lib/t_properties.c']
                    )

alt_lib = ['rt']
if OS == "Darwin":
    alt_lib = []


include_dirs += ['c_lib', 'cube_lib']

c_lib = Extension('c_lib',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    libraries = [] + alt_lib,
                    library_dirs = library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['c_lib/c_lib.pyx',
                            'c_lib/physics_timer.c']
                    )

ray_trace = Extension('c_lib.ray_trace',
                    include_dirs = include_dirs,
                    libraries = [] + alt_lib,
                    library_dirs = library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/_ray_trace.pyx',
                    #'c_lib/ray_trace/ray_trace.c',
                    ]
                                )

init_c_lib = Extension('init_c_lib',
                    include_dirs = include_dirs,
                    libraries = [] + alt_lib,
                    library_dirs = library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/init_c_lib.pyx',
                    ]
                                )

c_lib_objects = Extension('c_lib.c_lib_objects',
                    include_dirs = include_dirs,
                    libraries = [] + alt_lib,
                    library_dirs = library_dirs,
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'c_lib/c_lib_objects.pyx',
                    ]
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
    ext_modules = [
        terrain_map,
        c_lib,
        ray_trace,
        init_c_lib,
        c_lib_objects,
    ] + cythonize("*.pyx")
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
