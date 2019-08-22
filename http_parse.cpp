#include "http_parse.h"
#include <algorithm> 
#include <iostream>

bool HttpParse:: processRequestLine(const char* begin, 
            const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space)) {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end) {
            const char* question = std::find(start, space, '?');
            if (question != space) {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            } else {
                request_.setPath(start, space);
            }
            start = space + 1;
            succeed = (end - start == 8) && std::equal(start, end-1, "HTTP/1.");
            if (succeed) {
                if (*(end-1) == '1')
                    request_.setVersion(HttpRequest::kHttp11);
                else if (*(end-1) == '0')
                    request_.setVersion(HttpRequest::kHttp10);
                else 
                    succeed = false;
            }
        }
    }

    return succeed;
}

//has timer;
bool HttpParse::parseRequest(Buffer* buf)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore) {
        if (state_ == kExpectRequestLine) {
            const char* crlf = buf->findCRLF();
            if (crlf) {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok) {
                    //request_设置定时器
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders;
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectHeaders) {
            std::cout << "enter kExpectHeaders" << std::endl;
            const char* crlf = buf->findCRLF();
            if (crlf) {                
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) {
                    std::cout << "find : " << std::endl;
                    request_.addHeader(buf->peek(), colon, crlf);
                    //state_ = kExpectBody;
                } else {
                    state_= kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectBody) {
            hasMore = false;
        }
    }

    return ok;
}