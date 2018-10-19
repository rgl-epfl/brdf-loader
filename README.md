# BRDF Loader

This repository provides a self-contained implementation of the
evaluation and sampling code for the paper

> *An Adaptive Parameterization for Efficient Material
>  Acquisition and Rendering*
>
> by Jonathan Dupuy and Wenzel Jakob

This file only contains the interface. To also compile
the implementation, specify

```cpp
#define POWITACQ_IMPLEMENTATION 1
```

before including this header file. The namespace ``powitacq``
refers to the internal name of the project ("acquisition
using power iterations).

This proof-of-concept implementation involves some
inefficiencies that should be removed in a
"production" setting.

1. It returns the full captured spectrum using a
   ``std::valarray``, which causes dynamic
   memory allocation at every BRDF evaluation.

   In practice, the rendering system may only want
   to evaluate a fixed subset of the wavelengths,
   which could furthermore be stored on the stack.

2. The implementation doesn't rely on vectorization
   to accelerate simultaneous evaluation at multiple
   wavelengths.

In addition, we also provide a loader for our BRDF conversions
from spectral to RGB (see powitacq_rgb.h). The interface and use
is the same as that provided in powitacq.h.
