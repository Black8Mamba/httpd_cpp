#include "http_request.h"
#include "http_parse.h"
#include "http_response.h"
#include <map>
#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;  

//处理Connection选项
void http_process_connection(HttpRequest *request, HttpResponse *response)
{
    //cout << "http_process_connection" << endl;
    if (request->headers().count("Connection") != 0) {
        if (request->headers()["Connection"] == "keep-alive")
            response->setCloseConnection(false);
        else if (request->headers()["Connection"] == "close")
            response->setCloseConnection(true);
    }
}

int main(void)
{

    //HttpRequest request;
    Buffer buf;
    HttpParse parse;
    char s[] = "GET /index.html HTTP/1.1\r\nHost: hackr.jp\r\nConnection:keep-alive\r\n";
    buf.append(s, sizeof(s));
    parse.parseRequest(&buf);
    HttpRequest *request = parse.getRequest();
    //cout << request->methodString() << endl;
    //cout << request->getVersion() << endl;
    //cout << request->getPath() << endl;
    //cout <<  request->getHeader("Host") << endl;
    //cout << request->getHeader("Connection") << endl;
    HttpResponse response(true);
    //cout << "close connection: " << response.closeConnection() << endl;
    request->addHeaderFunc("Host", std::bind(http_process_connection, request, &response));
    request->handleFunc();
    //cout << "close connection: " << response.closeConnection() << endl;
    return 0;
}