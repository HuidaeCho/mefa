cmake_minimum_required(VERSION 3.16)

# XXX: undocumented
# https://stackoverflow.com/a/10306476/16079666
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(MEFA)

# for tail recursion
if(MSVC)
	add_compile_options(/O2)
else()
	add_compile_options(-O3)
endif()

file(GLOB src *.c *.h)
add_executable(mefa ${src})

if(MSVC AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.30")
	# for task
	# avoid error C7660: 'task': requires '-openmp:llvm' command line option(s)
	set(OpenMP_RUNTIME_MSVC "llvm")
endif()

find_package(OpenMP REQUIRED)
if(OpenMP_C_FOUND)
	target_link_libraries(mefa PUBLIC OpenMP::OpenMP_C)
	if(OpenMP_FOUND AND MSVC AND CMAKE_VERSION VERSION_LESS "3.30")
		# CMake < 3.30 doesn't support OpenMP_RUNTIME_MSVC
		# for task
		add_compile_options(-openmp:llvm)
	endif()
endif()

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
	target_link_libraries(mefa PUBLIC ${MATH_LIBRARY})
endif()

find_package(GDAL REQUIRED)
if(GDAL_FOUND)
	target_link_libraries(mefa PUBLIC ${GDAL_LIBRARY})
	target_include_directories(mefa PUBLIC ${GDAL_INCLUDE_DIR})
endif()
