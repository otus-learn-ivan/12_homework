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

template <class Thandler,class Titer = const char*,class Tseporator = Tget_point_to_n_tg>
class Tthread_mapper{
  Thandler handler_of_elm;
  Titer begin, end;
  size_t num_colum;
  char seporator;
public:
  Tthread_mapper(Titer bg, Titer end,size_t num_colum =9 ,char seporator = '\n') :handler_of_elm{}, begin{bg}, end{end}, num_colum{num_colum}, seporator{seporator}{}
  Thandler operator()(){
    auto end_line = begin;
    while (end_line < end) {
      auto begin_line = end_line;
      end_line = std::find(end_line, end, '\n');
      std::stringstream strm{Tseporator::factory(num_colum,seporator)(begin_line, end_line)};
      handler_of_elm(strm);
      end_line++;
    }
    return handler_of_elm;
  }
  static Tthread_mapper creator(Titer bg, Titer end, size_t num_colum = 9, char seporator = '\n'){
    std::cout << "creator bg:" << (void*)bg <<  " end:" << (void*)end << " num_colum: " <<num_colum << " seporator: " << seporator << "\n";
    return Tthread_mapper(bg, end, num_colum, seporator);
  }
  Thandler& get_handler(){
      return handler_of_elm;
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
    if(number_of_numbers%1000 ==0){
        std::cout << this <<" number_of_numbers:" << number_of_numbers << " summ_of_numbers: "<< summ_of_numbers <<"\n";
    }
  }
  Thandler_mapper_awerage(Thandler_mapper_awerage&& oner) = default;
  Thandler_mapper_awerage(Thandler_mapper_awerage& oner) = default;
};

#include <future>

int main(int argc, char ** argv)
{
    using namespace  std;

    if(argc > 1){
        std::cout << argv << "\t" << "1" << std::endl;
    }

     boost::filesystem::path filename("AB_NYC_2019.csv");
     std::cout << filename << " Current path is : " << std::filesystem::current_path().string() + "AB_NYC_2019.csv" << std::endl;

    const boost::interprocess::mode_t mode = boost::interprocess::read_only;
    boost::interprocess::file_mapping fm(filename.c_str(), mode);
    boost::interprocess::mapped_region region(fm, mode, 0, 0);
    const char* begin = static_cast<const char*>(region.get_address());
    using namespace std;
    cout << region.get_size() << endl;
    const char* end = begin + region.get_size();
    size_t size_block = region.get_size()/10;

    auto startTime = std::chrono::high_resolution_clock::now();

    vector<std::future<Thandler_mapper_awerage>> vector_average_fut;
    while(begin < end){
        auto end_block = find(
                    begin + size_block > end?end:begin + size_block,
                    end,'\n');
        cout << "begin: " << (void*)begin << " end_block:" << (void*)end_block <<"\n"  ;
        vector_average_fut.push_back(std::async(std::launch::async,Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end_block)));
        begin = end_block+1;
    }

    Thandler_mapper_awerage rez_handler;

    for(auto&rez_handler_fut:vector_average_fut){
        Thandler_mapper_awerage rez_handler_{rez_handler_fut.get()};
        rez_handler.number_of_numbers+=rez_handler_.number_of_numbers;
        rez_handler.summ_of_numbers+=rez_handler_.summ_of_numbers;
    }

     auto endTime = std::chrono::high_resolution_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
     std::cout << "Продолжительность работы функции: " << duration.count() << " мс" << std::endl;

    cout << std::fixed <<"\nnumber_of_numbers: " <<rez_handler.number_of_numbers
         <<" summ_of_numbers: "<< rez_handler.summ_of_numbers
         <<" "<< static_cast<double>(rez_handler.summ_of_numbers) /100
         <<" "<< static_cast<double>(rez_handler.summ_of_numbers) / 100
        / static_cast<double>(rez_handler.number_of_numbers) << endl;



#if 0
    std::vector<Tthread_mapper< Thandler_mapper_awerage>>thread_mapper;
    thread_mapper.push_back(Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end));

    std::vector<std::jthread> threads;
    for(auto& average:thread_mapper){
        cout << &average.get_handler() << "\n";
        threads.push_back(std::jthread(average));
    }
    cout << "START threads size: " << threads.size() << "\n";
    for(auto&th:threads){
        th.join();
    }
    sleep(1);
    cout << "print thread_mapper size: "<< thread_mapper.size()<<"\n";
    for(auto& rez_handler:thread_mapper){
             cout << std::fixed <<"\nnumber_of_numbers: " <<rez_handler.get_handler().number_of_numbers
                  <<" summ_of_numbers: "<< rez_handler.get_handler().summ_of_numbers
                  <<" "<< static_cast<double>(rez_handler.get_handler().summ_of_numbers) /100
                  <<" "<< static_cast<double>(rez_handler.get_handler().summ_of_numbers) / 100
                    / static_cast<double>(rez_handler.get_handler().number_of_numbers) << endl;
    }

    cout << "END\n";
#endif
    // thread_mapper.push_back(Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end));

    // for(auto& th : thread_mapper){
    //    jthread(th);
    // }


//    unique_ptr<Thandler_mapper_awerage> rez_handler_p;//= = Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end);//();

//     auto thread_hn = jthread([&rez_handler_p,&begin,&end](){
//         rez_handler_p =  std::make_unique<Thandler_mapper_awerage>(Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end)());
//     });

//    thread_hn.join();

//    Thandler_mapper_awerage rez_handler {*rez_handler_p};

//     cout << std::fixed <<"\nnumber_of_numbers: " << rez_handler.number_of_numbers << " summ_of_numbers: " << rez_handler.summ_of_numbers <<" "
//           << rez_handler.summ_of_numbers /100  << " " << rez_handler.summ_of_numbers / 100 / rez_handler.number_of_numbers << endl;




    // cout << "print : " << pr << endl;
    // if(success){
    //     cout << price;
    // }else {
    //     cout << "ERROR\n";
    // }

    // while((*next)!='\n' && next < end ){
    //     cout << *next;
    //     next++;
    // }


    // std::string line;
    // while (std::getline(std::cin, line))
    // {
    //     std::cout << line << "\t" << "1" << std::endl;
    // }

    return 0;
}
