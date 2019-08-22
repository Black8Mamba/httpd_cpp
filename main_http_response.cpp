#include "http_response.h"
#include "buffer.h"
#include <iostream>

using namespace std;

int main(void)
{
    HttpResponse response(false);
    Buffer buf;
    response.setContentType("text/html");
    response.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
    response.setStatusMessage(response.getMessageFromStatus());
    response.appendToBuffer(&buf);
    string s = buf.retrieveAllAsString();
    cout << s << endl;
    return 0;
}