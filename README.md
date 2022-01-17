# [DS](https://ahfakt.github.io/DS/)

```shell
# Target system processor
SYSTEM_PROCESSOR=x86_64

# Debug, Release, RelWithDebInfo, MinSizeRel ...
BUILD_TYPE=Debug

# Shared library files will be in ${INSTALL_PREFIX}/lib/${SYSTEM_PROCESSOR}/${BUILD_TYPE}
INSTALL_PREFIX=/home/user

# Uncomment to generate Doxygen documentation target
#DOC_ROOT=/home/user/doc

# cmake --help to see available generators
GENERATOR="Unix Makefiles"

git clone https://github.com/ahfakt/DP.git
git clone https://github.com/ahfakt/IO.git
git clone https://github.com/ahfakt/Stream.git
git clone https://github.com/ahfakt/DS.git

# Generate
mkdir build
cd DS
cmake \
    -B ../build/DS/${SYSTEM_PROCESSOR}/${BUILD_TYPE} \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
    -DDOC_ROOT=${DOC_ROOT} \
    -DCMAKE_DEPENDS_USE_COMPILER=FALSE \
    -G "${GENERATOR}"

# Build
# DPDoc
# IO | IOOBJ | IODoc
# Stream | StreamOBJ | StreamDoc
# DSDoc
# DSTest_Digraph_00 | DSTest_List_00 | DSTest_Map_00 | DSTest_Set_00 | DSTest_Vector_00
cmake \
    --build ../build/DS/${SYSTEM_PROCESSOR}/${BUILD_TYPE} \
    --target DSDoc \
    -- -j 6
```
