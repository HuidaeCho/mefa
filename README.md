# Memory-Efficient Flow Accumulation (MEFA)

Part of Memory-efficient Input Drainage paths Analysis System (MIDAS)

Please cite [Cho (2023)](https://doi.org/10.1016/j.envsoft.2023.105771) when you use this algorithm or the [r.flowaccumulation](https://grass.osgeo.org/grass83/manuals/addons/r.flowaccumulation.html) [GRASS](https://grass.osgeo.org) addon.

* Huidae Cho, July 2023. Memory-Efficient Flow Accumulation Using a Look-Around Approach and Its OpenMP Parallelization. Environmental Modelling & Software 167, 105771. [doi:10.1016/j.envsoft.2023.105771](https://doi.org/10.1016/j.envsoft.2023.105771).

![image](https://user-images.githubusercontent.com/7456117/230541813-893662a5-c10e-480c-931e-4de65f20f230.png)
* Raw data: 30-m NED for the entire Texas
  * 1.8 billion cells including null
  * 773 million cells without null
* Red: MEFA
* Blue and purple: Benchmark algorithms

Flow direction encoding in GeoTIFF:<br>
![fdr.png](fdr.png)

## Requirements

* C compiler with [OpenMP](https://www.openmp.org/) support
* [GDAL](https://gdal.org/)

For Windows, use [MSYS2](https://www.msys2.org/) and [OSGeo4W](https://trac.osgeo.org/osgeo4w/) to install [GCC](https://gcc.gnu.org/) and [GDAL](https://gdal.org/), respectively.

## How to compile MEFA

```bash
make
```

## Benchmark algorithms

* [MEFA-HP](https://github.com/HuidaeCho/high_performance_flow_accumulation) (algorithm index 7)
* [HPFA](https://github.com/HuidaeCho/high_performance_flow_accumulation) (algorithm index 1)
* [FastFlow](https://github.com/HuidaeCho/FastFlow) (Zhou's algorithm)
* [AreaD8](https://github.com/dtarb/TauDEM)
* [ArcGIS Pro Flow Accumulation](https://pro.arcgis.com/en/pro-app/latest/tool-reference/spatial-analyst/flow-accumulation.htm)

## Pre-/post-processing scripts

* [1.import_ned.zip](https://data.isnew.info/mefa/1.import_ned.zip)
* [2.prepare_inputs.zip](https://data.isnew.info/mefa/2.prepare_inputs.zip)
* [3.run_algorithms.zip](https://data.isnew.info/mefa/3.run_algorithms.zip)
* [4.check_outputs.zip](https://data.isnew.info/mefa/4.check_outputs.zip)
* [5.analyze_outputs.zip](https://data.isnew.info/mefa/5.analyze_outputs.zip)

## Data

Input flow direction files

* [taud8.tif](https://data.isnew.info/mefa/taud8.zip): AreaD8
* [fdr.tif](https://data.isnew.info/mefa/fdr.zip): Other algorithms

Output flow accumulation files

* [meac.tif](https://data.isnew.info/mefa/meac.zip): MEFA
* [mefahp.tif](https://data.isnew.info/mefa/mefahp.zip): MEFA-HP
* [hpfa.tif](https://data.isnew.info/mefa/hpfa.zip): HPFA
* [ffaczhou.tif](https://data.isnew.info/mefa/ffaczhou.zip): FastFlow
* [tauad801.tif](https://data.isnew.info/mefa/tauad801.zip): AreaD8
* [fac.tif](https://data.isnew.info/mefa/fac.zip): ArcGIS
