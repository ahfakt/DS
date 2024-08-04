# [DS](https://ahfakt.github.io/DS/)

```shell
# Target system processor
SYSTEM_PROCESSOR=x64

# Debug, Release, RelWithDebInfo, MinSizeRel
BUILD_TYPE=Release

git clone https://github.com/ahfakt/DP.git
git clone https://github.com/ahfakt/Stream.git
git clone https://github.com/ahfakt/Format.git
git clone https://github.com/ahfakt/DS.git

# Generate
mkdir build && cd DS
cmake \
    -B../build/${SYSTEM_PROCESSOR}/${BUILD_TYPE}/DS \
    -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
    -G "Unix Makefiles"

# Build
# DPDoc
# Stream | StreamDoc
# Format | FormatDoc
# DSDoc
# Test targets are available only when BUILD_TYPE=Debug
# Documentation is available only when BUILD_TYPE=Release
cmake \
    --build ../build/${SYSTEM_PROCESSOR}/${BUILD_TYPE}/DS \
    --config ${BUILD_TYPE} \
    --target all
```
