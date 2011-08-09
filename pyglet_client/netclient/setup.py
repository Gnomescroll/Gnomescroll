from distutils.core import setup, Extension
from Cython.Build import cythonize
#from distutils.extension import Extension
from Cython.Distutils import build_ext
import subprocess
import platform

#will compile a module called SDL in netclient directory
#python linux_SDL_setup.py build_ext --inplace

SYSTEM=platform.system()

SDL_CFLAGS=""
SDL_LDFLAGS=""

try:
    SDL_CFLAGS=subprocess.Popen(['sdl-config', '--cflags'], stdout=subprocess.PIPE).communicate()[0]
    SDL_LDFLAGS=subprocess.Popen(['sdl-config', '--libs'], stdout=subprocess.PIPE).communicate()[0]
except WindowsError:
    # sdl-config is a shell script, windows users will have to provide the path.
    pass

debug = True

extra_compile_args=[SDL_CFLAGS]
extra_link_args=[SDL_LDFLAGS]

libraries=['SDL']

if SYSTEM == 'Windows':
    libraries+=['GLee','opengl32','glu32']
    include_dirs = ['/usr/include/SDL']
else:
    libraries+=['GL','GLU']
    include_dirs = ['/usr/include/SDL']


if debug == True:
    extra_compile_args+=["-g"]
    extra_link_args+=["-g"]

SDL_gl = Extension('SDL.gl',
                    include_dirs = include_dirs,
                    libraries = libraries+['SDL_image'], #SDL_image ?
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['SDL/SDL_functions.c',
                                'SDL/camera.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',
                                'SDL/particle_functions.c',
                                'SDL/gl.pyx'],
                    )

SDL_input = Extension('SDL.input',
                    libraries = libraries+['SDL_image'], # "Chrome"], #SDL_image ?
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/input.pyx',
                                'SDL/input_functions.c',
                                'SDL/SDL_functions.c',]
                                )

SDL_hud = Extension('SDL.hud',
                    include_dirs = include_dirs,
                    libraries = libraries+['SDL_image', 'SDL_ttf'], #SDL_image ?
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [ 'SDL/hud.pyx',
                                'SDL/SDL_text.c',
                                'SDL/draw_functions.c',
                                'SDL/texture_loader.c',]
                                )

terrain_map = Extension('cube_lib.terrain_map',
                    #define_macros =  [('PLATFORM', 'linux')]
                    include_dirs = include_dirs,
                    optimize = 2,
                    #include_dirs = ['gl_lib'],  #this does nothing?
                    libraries = libraries+['SDL_image'], #SDL_image ?
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = ['cube_lib/terrain_map.pyx',
                            'cube_lib/t_map.c',
                            'cube_lib/t_properties.c',
                            'cube_lib/t_vbo.c',
                            'cube_lib/t_viz.c']
                    )

vox_lib = Extension('vox_lib',
                    include_dirs = include_dirs,
                    libraries = libraries, #SDL_image ?
                    extra_compile_args = extra_compile_args,
                    extra_link_args = extra_link_args,
                    sources = [
                    'vox_lib/vox_functions.c',
                     'vox_lib/vox_lib.pyx',]
                                )


setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [vox_lib, SDL_gl, SDL_input, SDL_hud, terrain_map, ], #+ cythonize("*.pyx")
)
