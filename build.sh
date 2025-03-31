set -e

build_type=Debug
if [ "$1" = "release" ]; then
    build_type=RelWithDebInfo
elif [ ! -z $1 ]; then
    echo Invalid argument $1, exiting...
    exit
fi

cmake -S . -B build/ -D CMAKE_BUILD_TYPE=$build_type
cmake --build build --target test --config $build_type
cmake --build build --target run --config $build_type
