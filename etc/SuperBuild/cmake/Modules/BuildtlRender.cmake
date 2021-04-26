include(ExternalProject)

set(tlRender_ARGS
    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DCMAKE_POLICY_DEFAULT_CMP0091:STRING=NEW
    -DCMAKE_FIND_FRAMEWORK=${CMAKE_FIND_FRAMEWORK}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    -DTLR_ENABLE_PYTHON=${TLR_ENABLE_PYTHON}
    -DTLR_BUILD_TESTS=${TLR_BUILD_TESTS})
if(CMAKE_CXX_STANDARD)
    set(tlRender_ARGS ${tlRender_ARGS} -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD})
endif()

set(tlRender_DEPENDS GLFW glad FreeType OTIO)
if(NOT WIN32)
    list(APPEND tlRender_DEPENDS FFmpeg)
endif()

ExternalProject_Add(
    tlRender
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/tlRender
    DEPENDS ${tlRender_DEPENDS}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../..
    CMAKE_ARGS ${tlRender_ARGS})
