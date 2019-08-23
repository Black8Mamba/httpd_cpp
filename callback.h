#ifndef CALLBACK_H
#define CALLBACK_H

class HttpRequest;
class yj_priority_queue;
class HttpResponse;

namespace CallBack
{
void request(HttpRequest* request, yj_priority_queue* queue);
int http_close_connect(HttpRequest* request);
void handle_keep_alive(HttpRequest* request, HttpResponse* response);
}

#endif