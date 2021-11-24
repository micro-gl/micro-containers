#========================================================================================
# Copyright (2021), Tomer Shalev (tomer.shalev@gmail.com, https://github.com/HendrixString).
# All Rights Reserved.
#
# this should help find the micro-containers headers-only package and define the micro-containers target that was
# installed on your system and does not include CMakeLists.txt file, so you can easily link to it.
# If successful, the following will happen:
# 1. micro-containers_INCLUDE_DIR will be defined
# 2. micro-containers::micro-containers target will be defined so you can link to it using target_link_libraries(..)
#========================================================================================
include(GNUInstallDirs)
include(FindPackageHandleStandardArgs)

find_path(micro-containers_INCLUDE_DIR
        NAMES micro-containers
        HINTS ${CMAKE_INSTALL_INCLUDEDIR}
        PATH_SUFFIXES clippers bitmaps samplers)
set(micro-containers_LIBRARY "/dont/use")
find_package_handle_standard_args(micro-containers DEFAULT_MSG
        micro-containers_LIBRARY micro-containers_INCLUDE_DIR)

if(micro-containers_FOUND)
    message("micro-containers was found !!!")
else(micro-containers_FOUND)
    message("micro-containers was NOT found !!!")
endif(micro-containers_FOUND)

if(micro-containers_FOUND AND NOT TARGET micro-containers::micro-containers)
    # build the target
    add_library(micro-containers::micro-containers INTERFACE IMPORTED)
    set_target_properties(
            micro-containers::micro-containers
            PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${micro-containers_INCLUDE_DIR}")
endif()