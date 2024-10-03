#ifndef MAPPER_ALL_H
#define MAPPER_ALL_H

#include "produser_consumer.h"
//-------------------------------------------------------------
class Tget_point_to_n_tg{
  size_t number_tg;
  char seporator;
public:
  Tget_point_to_n_tg(size_t n,char sp) :number_tg(n), seporator(sp){};
  template<class Titer>
  Titer operator()(Titer begin,Titer end){
    auto piont_to_n_seporator  = std::find_if(begin, end, [this](const char& ch)
    { number_tg = ch == seporator ? number_tg - 1 : number_tg;
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
  Tthread_mapper(size_t num_colum =9 ,char seporator = ',') :handler_of_elm{},num_colum{num_colum}, seporator{seporator}{}
  void operator()(std::unique_ptr<std::string> p_str){
      std::stringstream strm{
          Tseporator::factory(num_colum,seporator)(p_str->begin(),p_str->end()).base()
      };
      handler_of_elm(strm);
  }
  Thandler& get_handler(){
      return handler_of_elm;
  }
  std::function<void(std::unique_ptr<std::string>)> to_function() {
             return [this](std::unique_ptr<std::string> p_str) {
                 (*this)(std::move(p_str));
             };
         }
};


using T_string_queue = Tthreadsafe_queue <std::unique_ptr<std::string>>;
template<class Tqueue,class Thandler>
std::unique_ptr<std::vector<std::unique_ptr<Tthread_mapper<Thandler>>>>
procces(size_t number_threads){
    using Tthread = Tthread_mapper<Thandler>;
    using namespace  std;
    auto qu_in = std::make_shared<Tqueue>();
    std::unique_ptr<std::vector<std::unique_ptr<Tthread>>> v_hand_mapp_awerag =make_unique<std::vector<std::unique_ptr<Tthread>>>();
    std::vector<std::jthread> threads;
    for(size_t i=0;i<number_threads;i++){
        v_hand_mapp_awerag->push_back(std::make_unique<Tthread>(Tthread{}));
        threads.push_back(std::jthread(Tconsumer<string> {qu_in,(*v_hand_mapp_awerag)[i]->to_function()}));
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
    return  v_hand_mapp_awerag;
}

#endif // MAPPER_ALL_H
