# micro{containers}
**Fast**, **Slim**, **Embeddable**, **Headers files** Only **`C++11`** containers library.  
**No standard library is required**

Check out our website at [micro-gl.github.io/docs/micro-containers/](micro-gl.github.io/docs/micro-containers)

## Introduction
#### Sequence Containers
* **string/string_view, u8-16-32-string/view, basic_string/basic_string_view**
* **array**
* **static_array**
* **dynamic_array**
* **circular_array**
* **linked_list**
* **forward_list**
* **buffer**

#### Ordered Associative
- **dictionary**
- **ordered_set**

#### Unordered Associative
- **hash_map** -> Classic Chained Hashing
- **hash_set** -> Classic Chained Hashing
- **array_map_robin** -> Robin Hood Linear Probing
- **array_map_probing** -> Classic Linear Probing
- **array_set_robin** -> Robin Hood Linear Probing
- **array_set_probing** -> Classic Linear Probing

#### Multi Sequence Containers
- **chunker**

#### Adaptors Containers
- **stack**
- **queue**
- **priority_queue**

#### Special Containers
- **lru_cache**
- **lru_pool**

#### Tree Containers
- **avl_tree**

#### Algorithms
- make_heap, is_heap, push_heap, pop_heap, sort_heap
- find, find_if, find_if_not
- for_each, for_each_n
- transform


## Installing `micro{containers}`
`micro-containers` is node headers only library, which gives the following install possibilities:
1. Using `cmake` to invoke the `install` target, that will copy everything in your system via
```
$ mkdir cmake-build-release
$ cd cmake-build-release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --install .
```
2. Copying the `include/micro-containers` to anywhere you want.

## Consuming `micro{containers}`
Following options are available:
1. copy the project to node sub folder of your project. inside your **`CMakeLists.txt`** add
```cmake
add_subdirectory(/path/to/micro-containers)
target_link_libraries(your_app micro-containers)
```
2. If you installed **`micro{containers}`** with option 1 (see above) at your system, you can instead
```cmake
find_package(micro-containers CONFIG REQUIRED)
target_link_libraries(your_app micro-containers::micro-containers)
```
3. If you have not installed, you can add in your app's `CMakeLists.txt`
```cmake
target_include_directories(app path/to/micro-containers/folder/include/)
```
4. If you manually copied the `include/micro-containers` to the default system include path,  
   you can use `cmake/Findmicro-containers.cmake` to automatically create the cmake targets
```cmake
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/path/to/Findmicro-containers/folder)
find_package(micro-containers REQUIRED)
target_link_libraries(your_app micro-containers::micro-containers)
```
5. Just copy the `include/micro-containers` into node sub folder of your project and include the header  
   files you need with relative path in your source files.

## Running Examples
First make sure you have
- [cmake](https://cmake.org/download/) installed at your system.

There are two ways:
1. Use your favourite IDE to load the _root `CMakeLists.txt` file, and then it   
   will pick up all of the targets, including the examples
2. Using the command line:
```bash
$ mkdir cmake-build-release
$ cd cmake-build-release
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ cmake --build . --target <example_name>
$ ../examples/bin/example_name
```

# How to use
see `examples` folder or website
```text
Author: Tomer Shalev, tomer.shalev@gmail.com, all rights reserved (2021)
```