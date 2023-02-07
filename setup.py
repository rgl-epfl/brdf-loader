import sys, re, os

try:
    from skbuild import setup
    import nanobind
except ImportError:
    print("The preferred way to invoke 'setup.py' is via pip, as in 'pip "
          "install .'. If you wish to run the setup script directly, you must "
          "first install the build dependencies listed in pyproject.toml!",
          file=sys.stderr)
    raise

setup(
    name="brdf-loader",
    version="0.0.1",
    author="Jonathan Dupuy and Wenzel Jakob",
    author_email="wenzel.jakob@epfl.ch",
    description="Python binding for the evaluation and sampling code of the paper An Adaptive Parameterization for Efficient Material Acquisition and Rendering",
    url="https://github.com/rgl-epfl/brdf-loader",
    license="BSD",
    packages=['brdf_loader'],
    package_dir={'': 'python'},
    cmake_install_dir="python/brdf_loader",
    include_package_data=True,
    python_requires=">=3.8"
)
