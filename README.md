# Memory-Efficient Fast Flow Accumulation (MEFFA)

A manuscript is submitted to [the Environmental Modelling and Software journal](https://www.sciencedirect.com/journal/environmental-modelling-and-software).

## Requirements

* C compiler with [OpenMP](https://www.openmp.org/) support
* [GDAL](https://gdal.org/)

For Windows, use [MSYS2](https://www.msys2.org/) and [OSGeo4W](https://trac.osgeo.org/osgeo4w/) to install [GCC](https://gcc.gnu.org/) and [GDAL](https://gdal.org/), respectively.

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

In the `extra` directory, there are many `accumulate-*.c` files with different suffixes ({**d**ynamic, **g**uided, **s**tatic scheduling}, {**c**ollapse, **n**o collapse}, {**a**tomic, **f**lush}, {**t**ail recursion, **w**hile}), but only `accumulate.c` is compiled. Code for the {**m**ore, **l**ess memory} variants is in the same file and uses the `USE_LESS_MEMORY` macro defined by `Makefile.lessmem` and `Makefile.osgeo4w.lessmem`. To compile a specific variant of MEFFA, copy the corresponding `accumulate-*.c` file to `accumulate.c` first. The default `accumulate.c` is `accumulate-dnf_t.c` since dnfmt is recommended.

For example, to compile dcalt, copy `accumulate-dca_t.c` to `accumulate.c` and
```bash
# on Un*x
make -f Makefile.lessmem
# on Windows
make -f Makefile.osgeo4w.lessmem
```

## Benchmark algorithms

* [MEFFA-HP](https://github.com/HuidaeCho/high_performance_flow_accumulation) (algorithm index 7)
* [HPFA](https://github.com/HuidaeCho/high_performance_flow_accumulation) (algorithm index 1)
* [FastFlow](https://github.com/HuidaeCho/FastFlow) (Zhou's algorithm)
* [AreaD8](https://github.com/dtarb/TauDEM)
* [ArcGIS Pro Flow Accumulation](https://pro.arcgis.com/en/pro-app/latest/tool-reference/spatial-analyst/flow-accumulation.htm)

## Pre-/post-processing scripts

* [1.import_ned.zip](https://data.isnew.info/meffa/1.import_ned.zip)
* [2.prepare_inputs.zip](https://data.isnew.info/meffa/2.prepare_inputs.zip)
* [3.run_algorithms.zip](https://data.isnew.info/meffa/3.run_algorithms.zip)
* [4.check_outputs.zip](https://data.isnew.info/meffa/4.check_outputs.zip)
* [5.analyze_outputs.zip](https://data.isnew.info/meffa/5.analyze_outputs.zip)

## Data

Input flow direction files

* [taud8.tif](https://data.isnew.info/meffa/taud8.tif): AreaD8
* [fdr.tif](https://data.isnew.info/meffa/fdr.tif): Other algorithms

Output flow accumulation files

* [meac.tif](https://data.isnew.info/meffa/meac.tif): MEFFA
* [meffahp.tif](https://data.isnew.info/meffa/meffahp.tif): MEFFA-HP
* [hpfa.tif](https://data.isnew.info/meffa/hpfa.tif): HPFA
* [ffaczhou.tif](https://data.isnew.info/meffa/ffaczhou.tif): FastFlow
* [tauad801.tif](https://data.isnew.info/meffa/tauad801.tif): AreaD8
* [fac.tif](https://data.isnew.info/meffa/fac.tif): ArcGIS
