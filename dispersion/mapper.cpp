

#include <iostream>
#include <string>
#include "../produser_consumer.h"
#include "../mapper_all.h"
#include "unordered_map"

struct Thandler_mapper_dispersion{
  std::unordered_map<double, size_t> map_ds;
  Thandler_mapper_dispersion():map_ds{}{}
  void operator()(std::stringstream& strm){
    double number; strm >> number;
    map_ds[number]++;
//    if(number_of_numbers%1000 ==0){
//        std::cout << this <<" number_of_numbers:" << number_of_numbers << " summ_of_numbers: "<< summ_of_numbers <<"\n";
//    }
  }
  Thandler_mapper_dispersion(Thandler_mapper_dispersion&& oner) = default;
  Thandler_mapper_dispersion(Thandler_mapper_dispersion& oner) = default;
};

const size_t default_numbers_of_thread =5;


int main(int argc, char ** argv)
{
    size_t number_threads = argc>1?atoi(argv[1]):default_numbers_of_thread;
    auto v_hand_mapp_awerag = procces<T_string_queue,Thandler_mapper_dispersion>(number_threads);
    using namespace  std;

    for(auto& m_handler_av: *v_hand_mapp_awerag){
        for(const auto& pair :m_handler_av->get_handler().map_ds){
        cout << pair.first  << " "
             << pair.second << endl;
        }
    }
    return 0;
}




