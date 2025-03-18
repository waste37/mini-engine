if [ -z $1 ]; then
    cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug
elif [ "$1" = "release" ]; then
    cmake -S . -B build/ -D CMAKE_BUILD_TYPE=RelWithDebInfo
else
    echo Invalid argument $1, exiting...
    exit
fi

cmake --build build --target test && cmake --build build --target run
