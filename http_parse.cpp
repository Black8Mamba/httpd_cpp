#include "http_parse.h"
#include <algorithm> 

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

bool HttpParse::parseRequest()