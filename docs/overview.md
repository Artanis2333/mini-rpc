# overview
MiniRPC是用C++语言编写的轻量级RPC框架，计划提供以下功能：
* 支持Protobuf和HTTP协议，支持JSON数据格式。
* 提供可配置的多线程网络框架，支持同步阻塞、异步回调和协程三种编程模式，支持自定义协议格式。
* 提供对MySQL和Redis的原生支持。

目前已经完成的功能有：
* 支持Protobuf协议和JSON数据格式。
* 提供可配置的多线程网络框架，支持阻塞和异步回调，支持自定义协议格式。

## 通信协议
### Protobuf
MiniRPC并没有完全采用Google Protobuf的官方实现，而是以protoc插件的方式重写了.proto文件生成的C++代码。这似乎与避免重复造轮子的原则相悖。
然而，Google Protobuf官方的C++实现有以下几个问题，其实现方案并不足够好用。
* 不同于其他编程语言的Message对象接口，C++语言的对象接口复杂又繁琐，代码可读性差。例如想要修改某个Message对象的子对象，需要调用形如mutable_xxx()的函数返回一个指针。而修改Message对象的*repeated*成员和*map*成员就更复杂了。Google Protobuf甚至造了一个不同于标准库的map轮子。
* C++语言的Google Protobuf生成代码编译速度极慢。
* 有些第三方库也同样使用了Google Protobuf，两个不同版本的Google Protobuf库或者两个不同的.so文件往往会导致符号丢失或冲突。

MiniRPC以protoc插件的方式重写了.proto文件生成的C++代码。生成的代码很简洁，且*repeated*和*map*关键字修饰的成员变量都是以标准库容器实现的。
以示例代码来说明，形如如下Protobuf代码：
```protobuf
syntax = "proto3";
package example;

message Person
{
    string name = 1;
    int32 age = 2;
    Gender gender = 3;
    string email = 4;
    repeated string phone_numbers = 5;
    Address address = 6;
};

```
对应生成的C++代码：
```cpp
namespace example
{

class Person final : public mrpc::ReflectableMessage<Person>
{
public:
    std::string name;
    int32_t age = 0;
    int32_t gender = UNSPECIFIED;
    std::string email;
    std::vector<std::string> phone_numbers;
    Address address;

    // 其他成员函数
};

}
```
注意到，这里直接把*repeated*修饰的字段转化成了*std::vector*对象。实际上，MiniRPC会把*repeated*修饰的字段默认转化成*std::vector*对象，*map*修饰的字段默认转化成*std::map*对象，开发者也可以自定义其标准库容器类型，目前支持*std::vector*、*std::list*、*std::map*和*std::unordered_map*，未来会添加对其他容器类型的支持。
与Google Protobuf一样，MiniRPC也提供了Message对象的反射机制。

想要了解MiniRPC中Protobuf的具体设计，可阅读[MiniRPC中Protobuf的具体设计](docs/protobuf.md)。

### JSON数据格式
MiniRPC利用反射机制实现了Message对象和JSON对象的相互转化。
JSON库使用的是[nlohmann/json](https://github.com/nlohmann/json)。
```
namespace mrpc
{

using JsonObject = nlohmann::ordered_json;

struct JsonConvertParam
{
    // MessageToJson, MessageToJsonString
    bool treat_enum_as_string = true;       // 是否把枚举表达为字符串(否则表达为int32_t)

    // JsonToMessage, JsonStringToMessage
    bool skip_type_mismatch = true;         // 是否跳过不匹配的类型(否则会抛异常)

    // MessageToJsonString
    int indent = -1;                        // 缩进空格数(-1=紧凑)
    char indent_char = ' ';                 // 缩进字符(默认空格)
    bool ensure_ascii = true;               // 是否转义非ASCII字符
};

void MessageToJson(const Message& msg, JsonObject& json, const JsonConvertParam& param = JsonConvertParam());
void JsonToMessage(const JsonObject& json, Message& msg, const JsonConvertParam& param = JsonConvertParam());

void MessageToJsonString(const Message& msg, std::string& str, const JsonConvertParam& param = JsonConvertParam());
void JsonStringToMessage(const std::string& str, Message& msg, const JsonConvertParam& param = JsonConvertParam());

}
```

## 服务框架
MiniRPC使用Protobuf描述服务接口。
以示例代码来说明，形如如下Protobuf代码：
```protobuf
syntax = "proto3";
package example;

message EchoRequest
{
    string      data                = 1;
};

message EchoResponse
{
    string      data                = 1;
};

service EchoService
{
    rpc Echo(EchoRequest) returns (EchoResponse);
};

```
对应生成的C++代码：
```cpp
namespace example
{

class EchoService : public mrpc::Service
{
public:
    virtual int32_t Echo(const EchoRequest& req, EchoResponse& rsp) = 0;

    // 其他成员函数
};

class EchoServiceStub final : public mrpc::ServiceStub
{
public:
    int32_t Echo(const EchoRequest& req, EchoResponse& rsp);

    void Echo_Async(const EchoRequest& req, const std::shared_ptr<mrpc::AsyncCallback<EchoResponse>>& cb);
};
```
EchoServiceStub可以直接在worker线程和主线程中使用，通过配置名进行查找。可以看到，这里提供了同步阻塞和异步回调两种RPC方式。

开发需要继承实现EchoService，并按名字注册。进程配置根据注册名对服务进行进行配置。
```cpp
class EchoServiceImpl : public example::EchoService
{
    int32_t Echo(const example::EchoRequest& req, example::EchoResponse& rsp) override
    {
        rsp.data = req.data;
        LOG_DEBUG("Send msg, {}", rsp.data);
        return 0;
    }
};

int main(int argc, char* argv[])
{
    mrpc::Application app;
    int ret = app.ParseArgs(argc, argv);
    if (ret != 0)
    {
        LOG_ERROR("Parse args error, {}", ret);
        return ret;
    }

    app.RegisterService<EchoServiceImpl>("EchoService");

    // ...

    return 0;
}

```
### 进程配置
*[待补充]*

### 多线程模型
MiniRPC框架中有几下几种类型的线程：
* 客户端IO线程
* 服务端IO线程
* 服务端worker线程
* 日志线程
* 主线程

客户端IO线程每个进程中仅有1个。负责ServiceStub请求与回包的具体网络通信。底层使用了libuv库。
服务端IO线程每个注册的Service对应1个。负责该Service请求与回包的具体网络通信。底层使用了libuv库。
服务端worker线程数量可配置。负责调用RegisterService注册的Service派生类。每个worker线程对应一个Service对象实例。服务端worker线程中也可以调用其他ServiceStub。ServiceStub返回结果时，工作流(不论是同步调用还是异步调用都)会回到原worker线程。
日志线程顾名思义，就是将日志写入文件的线程。
主线程负责进程的初始化和服务注册。主线程也可以调用ServiceStub。ServiceStub返回结果时，工作流(不论是同步调用还是异步调用都)会回到主线程。此外，主线程还可以调用RegisterLoopCallback注册一个loop回调处理定时逻辑，loop间隔可配置。

### 自定义协议
MiniRPC框架支持自定义协议。代码根据名字注册协议的具体实现。进程配置里配置各个ServiceStub和Service使用的协议。
这里主要参考了[bRPC](https://github.com/apache/brpc)的设计理念。
```cpp
void GlobalRegisterProtocol(const std::string& name, const Protocol& protocol);
```
