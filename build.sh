#/bin/bash

RELEASE_MODE=0
BUILD_TEST=0
PROCESS_NUM=1

while getopts "rtj:" OPTION; do
    case $OPTION in
        "r")
            RELEASE_MODE=1
            ;;
        "t")
            BUILD_TEST=1
            ;;
        "j")
            PROCESS_NUM=$OPTARG
            ;;
        "?")
            echo "Unknown options."
            exit 1
            ;;
    esac
done

CMAKE_ARG=

if [ $RELEASE_MODE == 0 ]; then
CMAKE_ARG+="-DCMAKE_BUILD_TYPE=Debug "
else
CMAKE_ARG+=" -DCMAKE_BUILD_TYPE=RelWithDebInfo "
fi

if [ $BUILD_TEST == 0 ]; then
CMAKE_ARG+="-DBUILD_TEST=no "
else
CMAKE_ARG+="-DBUILD_TEST=yes "
fi

#echo "CMAKE_ARG: $CMAKE_ARG"

if [ ! -d build ]; then
    mkdir build
else
    rm build/CMakeCache.txt 2>/dev/null
fi

cd build && cmake $CMAKE_ARG .. && make -j $PROCESS_NUM
