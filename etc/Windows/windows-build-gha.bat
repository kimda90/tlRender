set BUILD_TYPE=%1

mkdir build
cd build
cmake ..\etc\SuperBuild ^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_PREFIX_PATH=install ^
  -DTLRENDER_MMAP=%TLRENDER_MMAP% ^
  -DTLRENDER_PYTHON=%TLRENDER_PYTHON% ^
  -DTLRENDER_GLFW=%TLRENDER_GLFW% ^
  -DTLRENDER_OCIO=%TLRENDER_OCIO% ^
  -DTLRENDER_AUDIO=%TLRENDER_AUDIO% ^
  -DTLRENDER_JPEG=%TLRENDER_JPEG% ^
  -DTLRENDER_TIFF=%TLRENDER_TIFF% ^
  -DTLRENDER_STB=%TLRENDER_STB% ^
  -DTLRENDER_PNG=%TLRENDER_PNG% ^
  -DTLRENDER_EXR=%TLRENDER_EXR% ^
  -DTLRENDER_FFMPEG=%TLRENDER_FFMPEG% ^
  -DTLRENDER_NFD=%TLRENDER_NFD% ^
  -DTLRENDER_QT5=%TLRENDER_QT5% ^
  -DTLRENDER_PROGRAMS=%TLRENDER_PROGRAMS% ^
  -DTLRENDER_EXAMPLES=%TLRENDER_EXAMPLES% ^
  -DTLRENDER_TESTS=%TLRENDER_TESTS% ^
  -DTLRENDER_GCOV=%TLRENDER_GCOV%
cmake --build . -j 4 --config %BUILD_TYPE%
