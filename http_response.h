#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <map>
#include <string>


class Buffer;
using std::map;
using std::string;

class HttpResponse
{
public:
    enum HttpStatusCode // 状态码定义
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k403Fobidden = 403,
        k404NotFound = 404,   
    };

    explicit HttpResponse(bool close)
        : statusCode_(kUnknown),
          closeConnection_(close)
    {}

    void setStatusCode(HttpStatusCode code) { statusCode_ = code; }

    void setStatusMessage(const string& message) { statusMessage_ = message; }

    string getMessageFromStatus() const 
    {
        switch(statusCode_)
        {
            case k200Ok:
                return "OK";
                break;
            case k301MovedPermanently:
                return "Moved Permanently";
                break;
            case k400BadRequest:
                return "Bad Request";
                break;
            case k404NotFound:
                return "Not Found";
                break;
            default:
                return "";
                break;
        }
    }

    void setCloseConnection(bool on) { closeConnection_ = on; }

    bool closeConnection() const { return closeConnection_; }
    
    void setContentType(const string& contentType)
    { addHeader("Content-Type", contentType); }

    void addHeader(const string& key, const string& value)
    { headers_[key] = value; }

    void setBody(const string& body)
    { body_ = body; }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<string, string> headers_; //首部字段
    HttpStatusCode statusCode_; //状态码
    string statusMessage_;  //状态信息
    bool closeConnection_;  //连接标记
    string body_;  //报文主体
};

#endif