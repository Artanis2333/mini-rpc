#include <iostream>
#include <mrpc/message/json.h>

#include "message_example.mrpc.h"

int main(int argc, char* argv[])
{
    example::Person person;
    person.name = "Alice";
    person.age = 25;
    person.gender = example::FEMALE;
    person.email = "alice@163.com";
    person.phone_numbers = { "138****3210", "186****5678" };
    person.address.country = "China";
    person.address.city = "Shanghai";
    person.address.street = "Caohejing Hi-Tech Park";

    std::string str;
    mrpc::JsonConvertParam param;
    param.indent = 4;
    mrpc::MessageToJsonString(person, str, param);
    std::cout << str << std::endl;

    return 0;
}
