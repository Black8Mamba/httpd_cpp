#ifndef CALLBACK_H
#define CALLBACK_H

class HttpRequest;
class yj_priority_queue;
class HttpResponse;
class EPoll;

namespace CallBack
{
void request(HttpRequest* request, yj_priority_queue* queue, EPoll* poll);
int http_close_connect(HttpRequest* request);
void handle_keep_alive(HttpRequest* request, HttpResponse* response);
}

#endif