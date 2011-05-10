
#from distutils.core import setup
#from distutils.extension import Extension
#from Cython.Distutils import build_ext

#setup(
    #cmdclass = {'build_ext': build_ext},
    #ext_modules = [Extension("world", ["world.pyx"])]
#)


from distutils.core import setup
from Cython.Build import cythonize

setup(
    name = "test app",
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
