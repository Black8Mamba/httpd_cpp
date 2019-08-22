#include "http_request.h"
#include "http_parse.h"
#include <map>
#include <iostream>

using namespace std;


int main(void)
{

    //HttpRequest request;
    Buffer buf;
    HttpParse parse;
    char s[] = "GET /index.html HTTP/1.1\r\nHost: hackr.jp\r\nConnection:keep-alive\r\n";
    buf.append(s, sizeof(s));
    parse.parseRequest(&buf);
   const  HttpRequest *request = parse.getRequest();
    cout << request->methodString() << endl;
    cout << request->getVersion() << endl;
    cout << request->getPath() << endl;
    cout <<  request->getHeader("Host") << endl;
    cout << request->getHeader("Connection") << endl;
    return 0;
}