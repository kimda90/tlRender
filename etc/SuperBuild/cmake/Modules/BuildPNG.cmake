include(ExternalProject)

set(PNG_SHARED_LIBS ON)
set(PNG_STATIC_LIBS OFF)
if(NOT BUILD_SHARED_LIBS)
    set(PNG_SHARED_LIBS OFF)
    set(PNG_STATIC_LIBS ON)
endif()

set(PNG_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DPNG_SHARED=${PNG_SHARED_LIBS}
    -DPNG_STATIC=${PNG_STATIC_LIBS}
    -DPNG_TESTS=OFF
    -DPNG_ARM_NEON=off)
if(CMAKE_CXX_STANDARD)
    set(PNG_ARGS ${PNG_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

ExternalProject_Add(
    PNG
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/PNG
    DEPENDS ZLIB
    URL "http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.gz?download"
    CMAKE_ARGS ${PNG_ARGS})

