#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <map>
#include <cassert>
#include <cstdio>
#include <string>
//#include "timer.h"
#include "buffer.h"
using std::string;

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

    bool setMethod(const char* start, const char* end) 
    {
        assert(method_ == kInvalid);
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
        headers_[field] = value;
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
            result = it->second;
        
        return result;
    }

    std::map<string,string> headers() const { return headers_; }
    //void swap(HttpRequest& that);

private:
    int fd_;
    int epoll_fd_;
    Buffer buf;
    Method method_;
    Version version_;
    string path_;
    string query_;
   // yj_timer receive_timer_;
    std::map<string,string> headers_;
};

#endif