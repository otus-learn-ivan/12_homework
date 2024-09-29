// bulk_server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#if 0
//#include <iostream>
//#include <boost/asio.hpp>
//
//#include <boost/thread/thread.hpp>
////#include <boost/thread.hpp>
//#include <thread>
//#include <iostream>
//
//namespace detail {
//  template <class T>
//  struct task_wrapped {
//  private:
//    T task_unwrapped_;
//  public:
//    explicit task_wrapped(const T& f)
//      : task_unwrapped_(f)
//    {}
//    void operator()() const {
//      // Сброс прерывания.
//      try {
//        boost::this_thread::interruption_point();
//      }
//      catch (const boost::thread_interrupted&) {}
//      try {
//        // Выполнение задачи.
//        task_unwrapped_();
//      }
//      catch (const std::exception& e) {
//        std::cerr << "Exception: " << e.what() << '\n';
//      }
//      catch (const boost::thread_interrupted&) {
//        std::cerr << "Thread interrupted\n";
//      }
//      catch (...) {
//        std::cerr << "Unknown exception\n";
//      }
//    }
//  };
//} // namespace detail
//
//namespace detail {
//  template <class T>
//  task_wrapped<T> make_task_wrapped(const T& task_unwrapped) {
//    return task_wrapped<T>(task_unwrapped);
//  }
//} // namespace detail
//
//#include <boost/asio/io_service.hpp>
////class tasks_processor : private boost::noncopyable {
//class tasks_processor{
//  protected:
//  static boost::asio::io_context& get_ios() {
//    static boost::asio::io_context ios;
//    static boost::asio::io_context::work work(ios);
//    return ios;
//  }
//public:
//  template <class T>
//  static void push_task(const T& task_unwrapped) {
//    get_ios().post(detail::make_task_wrapped(task_unwrapped));
//  }
//  static void start() {
//    get_ios().run();
//  }
//  static void stop() {
//    get_ios().stop();
//  }
//
//}; // tasks_processor
//int func_test() {
//  static int counter = 0;
//  ++counter;
//  boost::this_thread::interruption_point();
//  switch (counter) {
//  case 3:
//    throw std::logic_error("Just checking");
//  case 10:
//     //Эмуляция прерывания потока.
//     //Перехват внутри task_wrapped. Выполнение следующих задач продолжится.
//    throw boost::thread_interrupted();
//  case 90:
//     //Остановка task_processor.
//    tasks_processor::stop();
//  }
//  return counter;
//}
//void runServer() {
//  std::cout << std::this_thread::get_id() << " Running server..." << std::endl;
//
//  boost::asio::io_context ioContext;
//
//  boost::asio::signal_set signals{ ioContext, SIGINT, SIGTERM };
//  signals.async_wait([&](auto, auto) { ioContext.stop(); });
//
//  //boost::asio::co_spawn(ioContext, listen, boost::asio::detached);
//
//  ioContext.run();
//
//  std::cout << std::this_thread::get_id() << " Server stopped." << std::endl;
//}

#if 0
int main()
{
  for (std::size_t i = 0; i < 100; ++i) {
    tasks_processor::push_task(&func_test);
  }
  // Обработка не была начата.
  assert(func_test() == 1);
  // Мы также можем использовать лямбда-выражение в качестве задачи.
  // Асинхронно считаем 2 + 2.
  int sum = 0;
  tasks_processor::push_task(
    [&sum]() { sum = 2 + 2; }
  );
  // Обработка не была начата.
  assert(sum == 0);
  // Не выбрасывает исключение, но блокирует текущий поток выполнения до тех пор,
  // пока одна из задач не вызовет tasks_processor::stop().
  tasks_processor::start();
  assert(func_test() == 91);
}
#endif

//#include <fstream>
//#include <boost/filesystem.hpp>
//#include <iostream>
//int main()
//{
//  using namespace std;
//
//  size_t amount_tread = 3;
//
//  boost::filesystem::path basa_file("AB_NYC_2019.csv");
//  if (!boost::filesystem::exists(basa_file)){
//    // то выводим следующее сообщение об ошибке и выполняем функцию exit()
//    cerr << "Uh oh, SomeText.txt could not be opened for reading!" << endl;
//    exit(1);
//  }
//
//  boost::filesystem::if
//
//
//  auto  begin_block=  basa_file.i ;
//  basa_file.seekg(0, std::ios::end);
//  cout << basa_file.tellg() << endl;
//  size_t block_file_size = basa_file.tellg() / amount_tread;
//  cout << block_file_size << endl;
//  basa_file.seekg(block_file_size, std::ios::beg);
//  string line;
//  getline(basa_file, line);
//  cout << basa_file.tellg() << endl;
//
//
//  //while(basa_file){
//  //  string basa_str;
//  //  basa_file >> basa_str;
//  //  cout << basa_str << endl;
//  //}
//
//}
//#endif

#include <iostream>
#include <string>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <filesystem>
//#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>
#include <algorithm>

class Tget_point_to_n_tg{
  size_t number_tg;
  char seporator;
public:
  Tget_point_to_n_tg(size_t n,char sp) :number_tg(n), seporator(sp){};
  template<class Titer>
  Titer operator()(Titer begin,Titer end){
    auto piont_to_n_seporator  = std::find_if(begin, end, [this](const char& ch) { number_tg = ch == ',' ? number_tg - 1 : number_tg;
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
    return Tthread_mapper(bg, end, num_colum, seporator);
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
  }
};


int main(int argc, char** argv)
{
  if (argc > 1) {
//    std::cout << argv << "\t" << "1" << std::endl;
    std::cout <<  "ПППППП" << std::endl;
  }

  //std::filesystem::directory_entry

  // //string filename ="AB_NYC_2019.csv";
  // //boost::system::error_code error;

//     boost::filesystem::path filename(std::filesystem::current_path().string()+"AB_NYC_2019.csv");
  std::filesystem::path filename("AB_NYC_2019.csv");
  std::cout << filename << " Current path is : " << std::filesystem::current_path().string() + "AB_NYC_2019.csv" << std::endl;
//
//
//  // boost::filesystem::path filename = full_path ; //// "my_file.txt";
//
  const boost::interprocess::mode_t mode = boost::interprocess::read_only;
  boost::interprocess::file_mapping fm(filename.c_str(), mode);
  boost::interprocess::mapped_region region(fm, mode, 0, 0);
  const char* begin = static_cast<const char*>(region.get_address());
  using namespace std;
  cout << region.get_size() << endl;
  const char* end = begin + region.get_size();
  const char* next = begin;
  auto end_line=std::find(begin, end, '\n');
  std::for_each(begin, end_line, [](auto& ch) {std::cout << ch; });


  

  auto tg_n = std::find_if(begin, end_line, [](const char&ch){
    static int count = 0;
    count = ch == ',' ? count + 1: count;
    //if(ch == ','){
    //  stringstream str(&ch);
    //  cout << str.str() << endl;
    //}
    return count >= 9;
  });
  stringstream str(tg_n+1);
  double s;
  str >> s;
  cout << "\n answer : " << s;

  stringstream str1(Tget_point_to_n_tg::factory(9, ',')(begin, end_line));
  str1 >> s;
  cout << "\n answer1 : " << s;
#if 0
  end_line = begin;
  size_t number = 0;
  std::vector<double> prices;
  while(end_line < end){
    auto begin_line = end_line;
    end_line = std::find(end_line, end, '\n');
    double price;
    stringstream strm{ Tget_point_to_n_tg::factory(9, ',')(begin_line, end_line) };
    strm >> price;
    //cout << "\n answer " << number++ <<" : " << price << " begin:"<< std::hex <<(void*)begin_line <<" end:" <<  (void*)end_line << std::dec;
    end_line++;
    prices.push_back(price);
  }
  cout << "\n prices.size : " << prices.size() <<"\n";
#endif

  auto rez_handler = Tthread_mapper< Thandler_mapper_awerage>::creator(begin, end)();

  cout << std::fixed <<"\nnumber_of_numbers: " << rez_handler.number_of_numbers << " summ_of_numbers: " << rez_handler.summ_of_numbers <<" "
        << rez_handler.summ_of_numbers /100  << " " << rez_handler.summ_of_numbers / 100 / rez_handler.number_of_numbers << endl;

  //while ((*next) != '\n' && next < end) {
  //  cout << *next;
  //  next++;
  //}

//
//  // std::string line;
//  // while (std::getline(std::cin, line))
//  // {
//  //     std::cout << line << "\t" << "1" << std::endl;
//  // }

  return 0;
}
