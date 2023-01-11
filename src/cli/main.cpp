#include <iostream>
#include <rpc/client.h>

int main()
{
    rpc::client client("127.0.0.1", 11925);
    auto result = client.call("testCall").as<int>();
    std::cout << "The result is: " << result << std::endl;
    return 0;
}