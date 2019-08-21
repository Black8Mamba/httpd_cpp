#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include "http_request.h"

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

    bool parseRequest(char *buf);
    bool gotAll() const { return state_ == kGotAll; }
    //void resset();

private:
    bool processRequestLine(const char* begin, const char* end);
    HttpRequestParseState state_;
    HttpRequest request_;
};

#endif