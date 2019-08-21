#include "buffer.h"
#include <iostream>

using namespace std;

int main(void)
{
    Buffer buf;

    cout << "readable: " << buf.readableBytes() << endl;
    cout << "wrirable: " << buf.writableBytes() << endl;
    cout << "prependable: " << buf.prependableBytes() << endl;

    char s[] = "hello\r\n";
    buf.append(s, sizeof(s));

    const char* crlf = buf.findCRLF();
    if (crlf[0] == '\r')
        cout << "CR find" << endl;
    if (crlf[1] == '\n')
        cout << "LF find " << endl;

    const char* eol = buf.findEOL();
    if (*eol == '\n');
        cout << "Eol find" << endl;

    cout << "readable: " << buf.readableBytes() << endl;
    cout << "wrirable: " << buf.writableBytes() << endl;
    cout << "prependable: " << buf.prependableBytes() << endl;

    buf.appendInt32(1024);

    cout << "readable: " << buf.readableBytes() << endl;
    cout << "wrirable: " << buf.writableBytes() << endl;
    cout << "prependable: " << buf.prependableBytes() << endl;

    cout << "string: " << buf.retrieveAsString(sizeof(s)) << endl;

    cout << "readable: " << buf.readableBytes() << endl;
    cout << "wrirable: " << buf.writableBytes() << endl;
    cout << "prependable: " << buf.prependableBytes() << endl;

    cout << "int32: " << buf.readInt32() << endl;

    cout << "readable: " << buf.readableBytes() << endl;
    cout << "wrirable: " << buf.writableBytes() << endl;
    cout << "prependable: " << buf.prependableBytes() << endl;


    return 0;
}