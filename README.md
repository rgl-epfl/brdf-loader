# BRDF Loader

This repository contains a self-contained implementation of evaluation and
sampling code for the paper

> *An Adaptive Parameterization for Efficient Material
>  Acquisition and Rendering*
>
> by Jonathan Dupuy and Wenzel Jakob

A Mitsuba plugin and Python routines for plotting are also provided.


## Evaluation and sampling code

A header file implementation of the model can be found in ``powitacq.h`` /
``powitacq.inl`` (for spectral files) and ``powitacq_rgb.h`` /
``powitacq_rgb.inl`` (for RGB files) Note that only the interface part is
enabled by default; to also compile the implementation, specify

```cpp
#define POWITACQ_IMPLEMENTATION 1
```

before including this the corersponding ``.h`` file. The namespace
``powitacq`` refers to the internal name of the project ("acquisition
using power iterations).

This proof-of-concept implementation involves some inefficiencies that should
be removed in a "production" setting.

1. The spectral verision returns captured spectrum using a ``std::valarray``,
   which causes dynamic memory allocation at every BRDF evaluation.

   In practice, the rendering system may only want to evaluate a fixed subset
   of the wavelengths, which could furthermore be stored on the stack.

2. The implementation doesn't rely on vectorization to accelerate simultaneous
   evaluation at multiple wavelengths.

## Python loader

The ``python`` directory contains functionality to load and save ``.bsdf``
files via Python/NumPy. The file ``visualize.py`` loads an RGB material file,
plots the VNDF and slice data, and then writes it back.

You can also use the C++ BSDF class in Python using the provided `nanobind` module.

### Installation
```
pip install .
```

See the examples in `test` on how to use the C++ and Python classes.
