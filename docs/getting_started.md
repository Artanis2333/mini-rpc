# 构建
MiniRPC鼓励将第三方依赖静态编译并集成到同一路径下，这样便于框架的打包和分发，且运行MiniRPC服务的机器不必再安装依赖。
MiniRPC的第三方依赖有：
* [protobuf](https://github.com/protocolbuffers/protobuf)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [googletest](https://github.com/google/googletest)

## 支持的环境
目前仅支持Linux。

## Linux
指定MiniRPC的安装路径，这里以 **${HOME}/mini-rpc** 为例。
```shell
export MINI_PPC_INSTALL_PATH=${HOME}/mini-rpc
```

### protobuf
编译安装protobuf
```shell
wget -O protobuf-21.5.tar.gz https://github.com/protocolbuffers/protobuf/archive/refs/tags/v21.5.tar.gz
tar zxf protobuf-21.5.tar.gz
cd protobuf-21.5/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release \
    -Dprotobuf_BUILD_TESTS=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=${MINI_PPC_INSTALL_PATH}/3party/protobuf \
    -DCMAKE_INSTALL_LIBDIR=lib \
    ..
make -j 4
make install
cd ..
cp LICENSE ${MINI_PPC_INSTALL_PATH}/3party/protobuf
```

### jsoncpp
编译安装jsoncpp
```shell
wget -O jsoncpp-1.9.5.tar.gz https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.5.tar.gz
tar zxf jsoncpp-1.9.5.tar.gz
cd jsoncpp-1.9.5
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_OBJECT_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=${MINI_PPC_INSTALL_PATH}/3party/jsoncpp \
    -DCMAKE_INSTALL_LIBDIR=lib \
    ..
make -j 4
make install
cd ..
cp LICENSE ${MINI_PPC_INSTALL_PATH}/3party/jsoncpp
```

### googletest
编译安装googletest
```shell
wget -O googletest-1.14.0.tar.gz https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
tar zxf googletest-1.14.0.tar.gz
cd googletest-1.14.0
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=${MINI_PPC_INSTALL_PATH}/3party/googletest \
    -DCMAKE_INSTALL_LIBDIR=lib \
    ..
make -j 4
make install
cd ..
cp LICENSE ${MINI_PPC_INSTALL_PATH}/3party/googletest
```

### MiniRPC
编译安装Debug和RelWithDebInfo两种版本
```shell
git clone https://github.com/Artanis2333/mini-rpc.git
cd mini-rpc

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_INSTALL_PREFIX=${MINI_PPC_INSTALL_PATH} \
-DCMAKE_INSTALL_LIBDIR=lib \
..
make -j 4
make install

cd ..
rm -r build

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX=${MINI_PPC_INSTALL_PATH} \
    -DCMAKE_INSTALL_LIBDIR=lib \
    ..
make -j 4
make install
```
