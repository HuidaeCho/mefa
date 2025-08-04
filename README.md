# Memory-Efficient Flow Accumulation (MEFA)

Part of the [Memory-Efficient I/O-Improved Drainage Analysis System (MIDAS)](https://github.com/HuidaeCho/midas)

Please cite [Cho (2023)](https://doi.org/10.1016/j.envsoft.2023.105771) when you use this algorithm or the [r.flowaccumulation](https://grass.osgeo.org/grass-stable/manuals/addons/r.flowaccumulation.html) [GRASS](https://grass.osgeo.org/) addon.

**Citation**: Huidae Cho, July 2023. Memory-Efficient Flow Accumulation Using a Look-Around Approach and Its OpenMP Parallelization. Environmental Modelling & Software 167, 105771. [doi:10.1016/j.envsoft.2023.105771](https://doi.org/10.1016/j.envsoft.2023.105771). [Author's Version](https://idea.isnew.info/publications/Memory-efficient%20flow%20accumulation%20using%20a%20look-around%20approach%20and%20its%20OpenMP%20parallelization%20-%20Cho.2023.pdf).

![MEFA Graphical Abstract](https://idea.isnew.info/publications/Memory-efficient%20flow%20accumulation%20using%20a%20look-around%20approach%20and%20its%20OpenMP%20parallelization%20-%20Graphical%20abstract.png)

* Raw data: 30-m NED for the entire Texas
  * 1.8 billion cells including null
  * 773 million cells without null
* Red: MEFA
* Blue and purple: Benchmark algorithms

Predefined flow direction encodings in GeoTIFF: power2 (default, r.terraflow, ArcGIS), taudem (d8flowdir), 45degree (r.watershed), degree<br>
![image](https://github.com/user-attachments/assets/990f0530-fded-4ee5-bfbb-85056a50ca1c)
![image](https://github.com/user-attachments/assets/a02dfc15-a825-4210-82c4-4c9296dafadc)
![image](https://github.com/user-attachments/assets/64f5c65a-c7cc-4e06-a69f-6fccd6435426)
![image](https://github.com/user-attachments/assets/fafef436-a5f2-464a-89a8-9f50a877932c)

Custom flow direction encoding is also possible by passing `-e E,SE,S,SW,W,NW,N,NE` (e.g., 1,8,7,6,5,4,3,2 for taudem).

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
