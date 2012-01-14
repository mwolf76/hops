from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(
    cmdclass = {
        'build_ext': build_ext
    },

    ext_modules = [
        Extension("avl", ["avl.pyx"],
                  libraries=["avl"],
#                   extra_compile_args=["-O0"],
        ),
        Extension("ht", ["ht.pyx"],
                  libraries=["ht"],
#                  extra_compile_args=["-O3", "-funroll-loops", "-fomit-frame-pointer"],
        ),
        Extension("array", ["array.pyx"],
                  libraries=["array"],
#                  extra_compile_args=["-O0"],

        )
    ]
)
