#include "config.h"

#include <iostream>

int main(void)
{
    httpd_config c;
    string s("./httpd.json");
    c.read_conf(s);
    c.handle_for_sigpipe();
    
    std::cout << c.get_root() << std::endl;
    std::cout << c.get_port() << std::endl;
    std::cout << c.get_thread_num() << std::endl;

    return 0;

}