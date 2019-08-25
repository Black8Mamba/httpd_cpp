#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <cassert>
#include <cstdio>
#include <string>
#include "timer.h"
#include "buffer.h"
#include <functional>
using std::string;
#include <iostream>

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
            version_(kUnknown),
            timer_(NULL)
    { }

    void setVersion(Version v) { version_ = v; }

    Version getVersion(void) const { return version_; }

    bool setMethod(const char* start, const char* end) 
    {
        std::cout << method_ << std::endl;
       // assert(method_ == kInvalid);
        string m(start, end);
        if (m == "GET")
            method_ = kGet;
        else if (m == "POST")
            method_ = kPost;
        else if (m == "HEAD")
            method_ = kHead;
        else if (m == "PUT")
            method_ = kPut;
        else if (m == "DELETE")
            method_ = kDelete;
        else 
            method_ = kInvalid;

        return method_ != kInvalid;
    }

    const char* methodString() const
    {
        const char* result = "UNKNOWN";
        switch(method_)
        {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;
        }
        return result;
    }

    Method getMethod(void) const { return method_; }

    void setPath(const char* start, const char* end)
    {
        path_.assign(start, end);
    }

    void setTimer(yj_timer* timer) { timer_ = timer; }

    yj_timer* getTimer() { return timer_; }

    const string& getPath() const { return path_; }

    void setQuery(const char* start, const char* end)
    { 
        query_.assign(start, end);
    }
    
    const string& getQuery() const { return query_; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        string field(start, colon);
        ++colon;
        while(colon < end && isspace(*colon))
            ++colon;
        string value(colon, end);
        while(!value.empty() && isspace(value[value.size()-1]))
            value.resize(value.size()-1);
        headers_.insert(std::pair<string, string>(field, value));
       // std::cout << field << ":" << value << std::endl;
        //std::cout << "size:" << headers_.size() << std::endl;
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
            result = it->second;
        
        return result;
    }

    void addHeaderFunc(const string& field, std::function<void(void)> func)
    {
        if (headers_.count(field) != 0) //存在field字段,添加对应函数
            func_[field] = func;
    }

    void handleFunc()
    {
        for (auto &func : func_) {
            (func.second)();
        }
    }

    void printHeaders()
    {
        for (auto &node : headers_) {
            std::cout << node.first<< ": " << node.second << std::endl;
        }
    }

    std::map<string,string> headers() const { return headers_; }
    //void swap(HttpRequest& that);

    void setFd(int fd) { fd_ = fd; }
    int getFd() { return fd_; }


private:
    int fd_;
    int epoll_fd_;
    Method method_;
    Version version_;
    string path_;
    string query_;
    yj_timer *timer_;
    std::map<string,string> headers_;
    std::map<string, std::function<void(void)>> func_; //header处理函数函数？
};

#endif