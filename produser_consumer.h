#ifndef PRODUSER_CONSUMER_H
#define PRODUSER_CONSUMER_H

#include <iostream>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <filesystem>
#include <algorithm>
#include <cassert>
#include <thread>
#include <memory>
#include <future>

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

template <class T>
struct Tthreadsafe_queue {
  std::mutex mutex;
  std::condition_variable_any cv;
  std::queue<T> data_queue;
};

template <class T>
class Tproducer{
  std::shared_ptr<Tthreadsafe_queue <T>> qu;
public:
  Tproducer(std::shared_ptr<Tthreadsafe_queue <T>> q):qu(q){}
  static Tproducer Creator(std::shared_ptr<Tthreadsafe_queue <T>> q){
    return Tproducer(q);
  }
  void push(T value) {
    std::lock_guard<std::mutex> lock(qu->mutex);
    qu->data_queue.push(std::move(value));
    qu->cv.notify_all(); // Сигнализируем о доступном элементе
  }
};

template <class Tmessage>
class Tconsumer{
  std::shared_ptr<Tthreadsafe_queue <std::unique_ptr<Tmessage>>> qu;
  std::function<void(std::unique_ptr<Tmessage>)>action;
public:
  Tconsumer(std::shared_ptr<Tthreadsafe_queue <std::unique_ptr<Tmessage>>>& q,
            std::function<void(std::unique_ptr<Tmessage>)> act):qu(q), action(act){}
  void operator()(std::stop_token stopToken){
      while (1) {
          {
              {
                  std::unique_lock<std::mutex> lock(qu->mutex);
                  qu->cv.wait(lock,stopToken, [this] { return !qu->data_queue.empty(); }); // Ждем, пока очередь не будет пуста
                  if(stopToken.stop_requested()&&qu->data_queue.empty()){
                     return;
                  }
                  action(std::move(qu->data_queue.front()));
                  qu->data_queue.pop();
              }
          }
      }
  }
  ~Tconsumer(){
      //std::cout << "~Tproducer() destroy" << std::endl;
  }
};

#endif // PRODUSER_CONSUMER_H
