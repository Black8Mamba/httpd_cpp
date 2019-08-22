#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include "http_request.h"
#include "buffer.h"

class HttpParse
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,        
    };
    HttpParse(HttpRequest request)
        : state_(kExpectRequestLine),
           request_(request) 
    { }

    bool parseRequest(Buffer *buf);
    bool gotAll() const { return state_ == kGotAll; }
    HttpRequest  getRequest() { return request_; }
    void setRequest(HttpRequest request) { request_ = request; }
    //void resset();

private:
    bool processRequestLine(const char* begin, const char* end);
    HttpRequestParseState state_;
    HttpRequest request_;
};

#endif