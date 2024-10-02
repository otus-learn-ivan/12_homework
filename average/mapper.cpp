#include <iostream>
#include <string>
#include "../produser_consumer.h"
#include "../mapper_all.h"

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
    size_t number_threads = argc>1?atoi(argv[1]):default_numbers_of_thread;
    auto v_hand_mapp_awerag = procces<T_string_queue,Thandler_mapper_awerage>(number_threads);
    using namespace  std;

    for(auto& m_handler_av: *v_hand_mapp_awerag){
        auto& handler_av= m_handler_av->get_handler();
        cout << m_handler_av->get_handler().number_of_numbers << " "
             << m_handler_av->get_handler().summ_of_numbers   << endl;
    }
    return 0;
}
