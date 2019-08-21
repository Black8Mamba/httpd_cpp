#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <cassert>
#include <cstdio>
#include <string>
#include "timer.h"
using std::string;

#define MAX_BUF 8124

class HttpRequest
{
public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
        : method_(kInvalid),
            version_(kUnknown)
    { }

    void setVersion(Version v) { version_ = v; }
    Version getVersion(void) const { return version_; }
    bool setMethod(const char* start, const char* end) { ;}
    Method getMethod(void) const { return method_; }
    void setPath(const char* start, const char* end);
    const string& getPath() const { return path_; }
    void setQuery(const char* start, const char* end);
    const string& getQuery() const { return query_; }
private:
    int fd_;
    int epoll_fd_;
    char buff[MAX_BUF];
    Method method_;
    Version version_;
    string path_;
    string query_;
    yj_timer receive_timer_;
    std::map<string,string> headers_;
};

#endif