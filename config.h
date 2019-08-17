#ifndef CONFIG_H
#define CONFIG_H
#include <string>

using std::string;

class httpd_config
{
public:
    httpd_config() { }
    int read_conf(string filename);
    void handle_for_sigpipe();

    string get_root() { return root_; }
    int get_port() { return port_; }
    int get_thread_num() { return thread_num_; }

private:
    string root_;
    int port_;
    int thread_num_;
};

#endif