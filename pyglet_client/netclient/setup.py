
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

cube_lib = Extension('cube_lib',
                    include_dirs = ['/usr/local/include',],
                    libraries = [],
                    library_dirs = [],
                    extra_compile_args = [],
                    extra_link_args = [],
                    sources = ['cube_lib/cube_lib.pyx'])

setup(
    name = "My hello app",
    cmdclass = {'build_ext': build_ext},
    ext_modules = [cube_lib] + cythonize("*.pyx"),
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
