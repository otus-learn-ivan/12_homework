#include <iostream>
#include <string>

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
  const size_t  id;
  std::shared_ptr<Tthreadsafe_queue <std::unique_ptr<Tmessage>>> qu;
  std::function<void(size_t,std::unique_ptr<Tmessage>) > action;
public:
  Tconsumer(const size_t  ID,std::shared_ptr<Tthreadsafe_queue <std::unique_ptr<Tmessage>>>& q,
            std::function<void(size_t, std::unique_ptr<Tmessage>)> act):
  id(ID), qu(q), action(act){}
  void operator()(std::stop_token stopToken){
      while (1) {
          {
              {
                  std::unique_lock<std::mutex> lock(qu->mutex);
                  qu->cv.wait(lock,stopToken, [this] { return !qu->data_queue.empty(); }); // Ждем, пока очередь не будет пуста
                  if(stopToken.stop_requested()&&qu->data_queue.empty()){
                     return;
                  }
                  action(id,std::move(qu->data_queue.front()));
                  qu->data_queue.pop();
              }
          }
      }
  }
  ~Tconsumer(){
      //std::cout << "~Tproducer() destroy" << std::endl;
  }
};


class Tget_point_to_n_tg{
  size_t number_tg;
  char seporator;
public:
  Tget_point_to_n_tg(size_t n,char sp) :number_tg(n), seporator(sp){};
  template<class Titer>
  Titer operator()(Titer begin,Titer end){
    auto piont_to_n_seporator  = std::find_if(begin, end, [this](const char& ch)
    { number_tg = ch == ',' ? number_tg - 1 : number_tg;
      return number_tg == 0; });
    return ++piont_to_n_seporator;
  }
  static Tget_point_to_n_tg factory(size_t n, char sp){
    return Tget_point_to_n_tg(n, sp);
  }
};

template <class Thandler,class Tseporator = Tget_point_to_n_tg>
class Tthread_mapper{
  Thandler handler_of_elm;
  size_t num_colum;
  char seporator;
public:
  Tthread_mapper(size_t num_colum =9 ,char seporator = '\n') :handler_of_elm{},num_colum{num_colum}, seporator{seporator}{}
  void operator()(std::unique_ptr<std::string> p_str){
      std::stringstream strm{
          Tseporator::factory(num_colum,seporator)(p_str->begin(),p_str->end()).base()
      };
      handler_of_elm(strm);
  }
  Thandler& get_handler(){
      return handler_of_elm;
  }
  std::function<void(size_t, std::unique_ptr<std::string>)> to_function() {
             return [this](size_t id, std::unique_ptr<std::string> p_str) {
                 (*this)(std::move(p_str));
             };
         }
};

struct Thandler_mapper_awerage{
  size_t number_of_numbers;
  long long summ_of_numbers;
  Thandler_mapper_awerage():number_of_numbers{}, summ_of_numbers{}{}
  void operator()(std::stringstream& strm){
    double number; strm >> number;
    summ_of_numbers += static_cast<long long>(number*100);
    number_of_numbers++;
//    if(number_of_numbers%1000 ==0){
//        std::cout << this <<" number_of_numbers:" << number_of_numbers << " summ_of_numbers: "<< summ_of_numbers <<"\n";
//    }

  }
  Thandler_mapper_awerage(Thandler_mapper_awerage&& oner) = default;
  Thandler_mapper_awerage(Thandler_mapper_awerage& oner) = default;
};

const size_t default_numbers_of_thread =5;

int main(int argc, char ** argv)
{
    using namespace  std;
    auto qu_in = std::make_shared<Tthreadsafe_queue <std::unique_ptr<string>>>();
//    auto qu_out = std::make_shared<Tthreadsafe_queue <std::unique_ptr<string>>>();

//    Tthread_mapper<Thandler_mapper_awerage> m_handler_av1{};
//    auto th1 = std::jthread(Tconsumer<string>{0,qu_in,m_handler_av1.to_function()});
//    Tthread_mapper<Thandler_mapper_awerage> m_handler_av2{};
//    auto th2 = std::jthread(Tconsumer<string>{1,qu_in,m_handler_av2.to_function()});
    size_t number_threads = argc>1?atoi(argv[1]):default_numbers_of_thread;
    std::vector<std::unique_ptr<Tthread_mapper<Thandler_mapper_awerage>>> v_hand_mapp_awerag;
    std::vector<std::jthread> threads;
    for(size_t i=0;i<number_threads;i++){
        v_hand_mapp_awerag.push_back(
            std::make_unique<Tthread_mapper<Thandler_mapper_awerage>>(Tthread_mapper<Thandler_mapper_awerage>{})
                    );
        threads.push_back(std::jthread(Tconsumer<string> {i,qu_in,v_hand_mapp_awerag[i]->to_function()}));
    }
    string line;
    while (std::getline(std::cin, line))
    {
        Tproducer<std::unique_ptr<string>>::Creator(qu_in).push(std::make_unique<string>(line));//.push(line);
    }

    for (auto& thread : threads) {
        thread.request_stop();
        if (thread.joinable()) {
            thread.join();
        }
    }
//    Thandler_mapper_awerage summ_average;
    for(auto& m_handler_av: v_hand_mapp_awerag){
//        summ_average.summ_of_numbers+=m_handler_av->get_handler().summ_of_numbers;
//        summ_average.number_of_numbers+=m_handler_av->get_handler().number_of_numbers;
        auto& handler_av= m_handler_av->get_handler();
        cout << handler_av.number_of_numbers
             <<" " << handler_av.summ_of_numbers
             // <<" " << handler_av.summ_of_numbers/handler_av.number_of_numbers/100.0
             << endl;
    }
//    cout <<"END " << summ_average.number_of_numbers
//         <<" " << summ_average.summ_of_numbers
//         <<" " << summ_average.summ_of_numbers/summ_average.number_of_numbers/100.0
//         << endl;

//    th1.request_stop();
//    th1.join();
//    auto& handler_av  = m_handler_av1.get_handler();
//    cout << "END " << handler_av.number_of_numbers
//         <<" " << handler_av.summ_of_numbers
//         <<" " << handler_av.summ_of_numbers/handler_av.number_of_numbers/100.0
//         << endl;
//    auto& handler_av2  = m_handler_av2.get_handler();
//    cout << "END " << handler_av2.number_of_numbers
//         <<" " << handler_av2.summ_of_numbers
//         <<" " << handler_av2.summ_of_numbers/handler_av2.number_of_numbers/100.0
//         << endl;

#if 0
//    auto startTime = std::chrono::high_resolution_clock::now();

    size_t number_threads = argc>1?atoi(argv[1]):default_numbers_of_thread;
    Tmapper<Thandler_mapper_awerage> mapper_awerage(number_threads);
    std::vector<Thandler_mapper_awerage> mapper_awerage_answer{
            mapper_awerage.start_mapper(line.c_str())};

//    auto endTime = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
//    std::cout << "Продолжительность работы функции: " << duration.count() << " мс" << std::endl;
    for(auto& answer:mapper_awerage_answer){
        std::cout << answer.number_of_numbers <<" " << answer.summ_of_numbers <<"\n";
    }
#endif
#if 0
    Thandler_mapper_awerage rez_handler;
    for(auto rez_handler_:mapper_awerage_answer){
        rez_handler.number_of_numbers+=rez_handler_.number_of_numbers;
        rez_handler.summ_of_numbers+=rez_handler_.summ_of_numbers;
    }


    cout << std::fixed <<"\nnumber_of_numbers: " <<rez_handler.number_of_numbers
         <<" summ_of_numbers: "<< rez_handler.summ_of_numbers
         <<" "<< static_cast<double>(rez_handler.summ_of_numbers) /100
         <<" "<< static_cast<double>(rez_handler.summ_of_numbers) / 100
        / static_cast<double>(rez_handler.number_of_numbers) << endl;

#endif
    return 0;
}
