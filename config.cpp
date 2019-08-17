#include  "config.h"
#include "CJsonObject.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int httpd_config::read_conf(string filename)
{
    fstream fs(filename.c_str(), std::fstream::in | std::fstream::out);
    stringstream ss;
    ss << fs.rdbuf();
    string str = ss.str();

    neb::CJsonObject oJson(str);
    oJson.Get("root", this->root_);
   // std::cout << this->root_ << std::endl;
    oJson.Get("port", this->port_);
   // std::cout << this->port_ << std::endl;
    oJson.Get("thread_num", this->thread_num_);
   // std::cout << this->thread_num_ << std::endl;
    return 0;
}

void httpd_config::handle_for_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;    
}