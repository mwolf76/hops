from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(
    cmdclass = {
        'build_ext': build_ext
    },

    ext_modules = [
        Extension("avl", ["avl.pyx"],
                  libraries=["avl"]
        ),
        Extension("ht", ["ht.pyx"],
                  libraries=["ht"]
        ),
        Extension("array", ["array.pyx"],
                  libraries=["array"]
        )
    ]
)
