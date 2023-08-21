#/bin/bash

INSTALL_PATH=
RELEASE_MODE=0
BUILD_TEST=0
PROCESS_NUM=1

while getopts "i:rtj:" OPTION; do
    case $OPTION in
        "i")
            INSTALL_PATH=$OPTARG
            ;;
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
            echo "Usage:"
            echo "  -i install_path     Set install path."
            echo "  -r                  Build in release mode."
            echo "  -t                  Build test cases."
            echo "  -j jobs             Same with make -j."
            exit 1
            ;;
    esac
done

if [ ! -z $INSTALL_PATH ]; then
    if [ ! -d $INSTALL_PATH ]; then
        mkdir -p $INSTALL_PATH
    fi
    CMAKE_ARG="-DCMAKE_INSTALL_PREFIX=$INSTALL_PATH"
    CMAKE_ARG+=" -DCMAKE_INSTALL_LIBDIR=lib"
fi

if [ $RELEASE_MODE == 0 ]; then
    CMAKE_ARG+=" -DCMAKE_BUILD_TYPE=Debug"
else
    CMAKE_ARG+=" -DCMAKE_BUILD_TYPE=RelWithDebInfo"
fi

if [ $BUILD_TEST != 0 ]; then
    CMAKE_ARG+=" -DBUILD_TEST=ON"
fi

#echo "CMAKE_ARG: $CMAKE_ARG"

if [ ! -d build ]; then
    mkdir build
else
    rm build/CMakeCache.txt 2>/dev/null
fi

cd build && cmake $CMAKE_ARG .. && make -j $PROCESS_NUM || exit 1

if [ ! -z $INSTALL_PATH ]; then
    make install
fi
