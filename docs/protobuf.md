# MiniRPC中Protobuf的具体设计

## 语法与数据类型
MiniRPC支持proto2和proto3语法。

### 标量数据类型
MiniRPC支持所有的标量数据类型及其编码格式，包括：
* 32位整数：int32 uint32 sint32 fixed32 sfixed32
* 64位整数：int64 uint64 sint64 fixed64 sfixed64
* 布尔值：bool
* 浮点数：float double
* 字符串: string bytes

string和bytes对应std::string。

### MiniRPC所做的扩展与修改
MiniRPC对repeated和map修饰的类型做了扩展，允许自定义其对应的C++容器类型。语法为[(mrpc.cpp_type) = XXX]，其中XXX表示C++容器类型。
代码示例如下：
```protobuf
syntax = "proto3";
package example;

import "mrpc/options.proto";

    repeated int32      int_repeat          = 1 [(mrpc.cpp_type) = vector];
    repeated bool       bool_repeat         = 2 [(mrpc.cpp_type) = list];
    map<string, int32>  map_s2i             = 3 [(mrpc.cpp_type) = map];
    map<string, uint32> map_s2u             = 4 [(mrpc.cpp_type) = unordered_map];
```
repeated字段支持扩展的C++容器类型为std::vector和std::list。

map字段支持扩展的C++容器类型为std::map和std::unordered_map。

未来还会支持其他容器类型。

如果不做声明，repeated修饰的字段默认为std::vector类型，map修饰的字段默认为std::map类型。

需要注意的是，repeated bool类型不允许声明为vector类型（如果不声明则默认就是vector类型，因此必须强制声明为其他类型）。因为标准库的std::vector<bool>类型是陷阱，应该禁止使用。

与官方实现的不同之处：
1. repeated修饰的字段在proto2中必须强制声明为[packed = true]（如果字段允许设置packed）。这是因为proto2一开始设计的编码格式不够紧凑，后来引入了更为高效紧凑的编码格式（声明为[packed = true]）。而proto3就是使用的这样紧凑的编码格式。我们有理由总是使用更为高效的编码格式。
2. MiniRPC不会对string的文本格式做检查（这是业务层应该做的事情）。
3. 枚举类型成员变量对应生成代码的类型为int32_t,而不是enum的具体类型，这样有利于控制反射机制的代码规模。此外，有些脚本语言（比如Lua）会把枚举对应为字符串，代码执行效率很低，因此在实际应用中经常会使用int32类型代替enum类型。

### 消息类型(message)
MiniRPC的实现中支持message类型，但不支持message内嵌套定义message和enum(对应于C++的内部类)。

### 枚举类型(enum)
支持。如前所述。

### 重复字段(repeated)
支持。如前所述。

### 键值映射(map)
支持。如前所述。

### Oneof类型
暂不支持。

### Any类型
暂不支持。

## Message基类
message类型生成的C++类定义，是Message类型的派生类。Message提供如下接口函数可供调用：
```cpp
namespace mrpc
{

class Message
{
public:
    // Name.
    virtual std::string_view GetName() const = 0;
    virtual std::string_view GetFullName() const = 0;

    // Reflection.
    virtual Message* New() const = 0;
    virtual void CopyFrom(const Message& msg) = 0;
    virtual const Descriptor* GetDescriptor() const = 0;

    // Clear data.
    virtual void Clear() = 0;

    // Byte size.
    inline size_t GetCachedSize() const;
    inline size_t ByteSize(bool skip_default = true) const;

    // Serialize.
    void SerializeToString(std::string& s, bool skip_default = true) const;

    // Parse.
    bool ParseFromString(std::string_view s);
};

}
```
大部分函数的命名与Google Protobuf官方实现保持一致。

## 反射
与Google Protobuf官方实现类似，MiniRPC提供了各种Descriptor类型和反射机制。
详见*mrpc/message/descriptor.h*和*mrpc/message/reflection.h*文件。

*[未完待续]*
