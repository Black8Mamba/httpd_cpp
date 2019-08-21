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
    HttpParse()
        : state_(kExpectRequestLine)
    { }

    bool parseRequest(Buffer *buf);
    bool gotAll() const { return state_ == kGotAll; }
    const HttpRequest*  getRequest() const { return &request_; }
    //void resset();

private:
    bool processRequestLine(const char* begin, const char* end);
    HttpRequestParseState state_;
    HttpRequest request_;
};

#endif