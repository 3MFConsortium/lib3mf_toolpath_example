mkdir build
cd build
mkdir Release
mkdir Debug

copy ..\bin\lib3mf_win64.dll Release\lib3mf_win64.dll
copy ..\bin\lib3mf_win64.dll Debug\lib3mf_win64.dll

cmake ..\Source
cmake --build . --config Release

