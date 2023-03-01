# Memory-Efficient Fast Flow Accumulation (MEFFA)

A manuscript is submitted to [the Environmental Modelling and Software journal](https://www.sciencedirect.com/journal/environmental-modelling-and-software).

## Requirements

* C compiler with OpenMP support
* [GDAL](https://gdal.org/)

For Windows, use [MSYS2](https://www.msys2.org/) and [OSGeo4W](https://trac.osgeo.org/osgeo4w/) to install the GNU C compiler and GDAL, respectively.

## How to compile MEFFA

On Un*x, use `Makefile` for more-memory versions or `Makefile.lessmem` for less-memory versions. Similarly, on Windows, use `Makefile.osgeo4w` and `Makefile.osgeo4w.lessmem` for more- and less-memory versions, respectively. To compile a more-memory variant,
```bash
# on Un*x
make
# on Windows
make -f Makefile.osgeo4w
```
To compile a less-memory variant,
```bash
# on Un*x
make -f Makefile.lessmem
# on Windows
make -f Makefile.osgeo4w.lessmem
```

There are many `accumulate-*.c` files with different suffixes ({**d**ynamic, **g**uided, **s**tatic scheduling}, {**c**ollapse, **n**o collapse}, {**a**tomic, **f**lush}, {**t**ail recursion, **w**hile}), but only `accumulate.c` is compiled. Code for the {**m**ore, **l**ess memory} variants is in the same file and uses the `USE_LESS_MEMORY` macro defined by `Makefile.lessmem` or `Makefile.osgeo4w.lessmem`. To compile a specific variant of MEFFA, copy the corresponding `accumulate-*.c` file to `accumulate.c` first. The default `accumulate.c` is `accumulate-dnf_t.c` since dnfmt is recommended.

For example, to compile dcalt, copy ``accumulate-dca_t.c`` to ``accumulate.c`` and
```bash
# on Un*x
make -f Makefile.lessmem
# on Windows
make -f Makefile.osgeo4w.lessmem
```
