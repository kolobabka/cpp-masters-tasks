# Our daddy taught us not to be ashamed of our C++ projects ...

## Build system

```
$ conan install .
$ cmake -S . -B build/Release -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake
$ cmake --build build/Release
```

## Launch
```
./build/cram input_file [--dump-ast]
```

