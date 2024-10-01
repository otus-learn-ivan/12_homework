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
//    std::cout << "creator bg:" << (void*)bg <<  " end:" << (void*)end << " num_colum: " <<num_colum << " seporator: " << seporator << "\n";
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

template <typename Thandler_mapper>
struct Tmapper{
    size_t count_theread;
    Tmapper (size_t count_theread):count_theread(count_theread){}
      std::vector<Thandler_mapper>  start_mapper(const char* path){
        using namespace std;
//        boost::filesystem::path filename("AB_NYC_2019.csv");
        boost::filesystem::path filename(path);
        const boost::interprocess::mode_t mode = boost::interprocess::read_only;
        boost::interprocess::file_mapping fm(filename.c_str(), mode);
        boost::interprocess::mapped_region region(fm, mode, 0, 0);
        const char* begin = static_cast<const char*>(region.get_address());
//        cout << region.get_size() << endl;
        const char* end = begin + region.get_size();
        size_t size_block = region.get_size()/count_theread;
        vector<std::future<Thandler_mapper>> vector_average_fut;
        while(begin < end){
            auto end_block = find(
                        begin + size_block > end?end:begin + size_block,
                        end,'\n');
//            cout << "begin: " << (void*)begin << " end_block:" << (void*)end_block <<"\n"  ;
            vector_average_fut.push_back(std::async(std::launch::async,Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end_block)));
            begin = end_block+1;
        }

        std::vector<Thandler_mapper> rez_handler;

        for(auto&rez_handler_fut:vector_average_fut){
            rez_handler.emplace_back(rez_handler_fut.get());//(answ);
        }
        return rez_handler;
    }
};

const size_t default_numbers_of_thread =5;

int main(int argc, char ** argv)
{
    std::cerr << " HELLO MAPPER    !"<<  std::endl;

    using namespace  std;
    string line;
    std::getline(std::cin, line);
//    while (std::getline(std::cin, line))
//    {
//        std::cout <<"line: " << line << std::endl;
//    }

    std::cerr << "path: "<< line << "   !"<<  std::endl;

    sleep(1);



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
