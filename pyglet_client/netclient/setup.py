
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

setup(
    name = "My hello app",
    cmdclass = {'build_ext': build_ext},
    ext_modules = cythonize("*.pyx"),
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
