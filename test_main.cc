#include "http_server.h"

class MyService : public Service {
 public:
  MyService():pool(2){}
  void OnRequest(HttpRequst * req, Done done) {
    std::cout << "OnRequest" << std::endl;
    pool.AddTask(boost::bind(&MyService::OnRequestDoInThreads, this, req, done));
  }
  void OnRequestDoInThreads(HttpRequst * req, Done done) {
    std::cout << "OnRequestDoInThreads" << std::endl;
    req->resopnse = "200 ok to:\n" + req->raw_header;
    done();
  }
 private:
  ThreadPool pool;
};

int main(int argc, const char *argv[]) {
  MyService myservice;
  HttpServer server(9870, &myservice);
  server.Run();
  return 0;
}

