
#include "test_str_file.h"

bool test_str_file(std::string name_file,std::string tst_str){
    std::cout << name_file<<"\n";
    std::ifstream file(name_file);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << name_file << std::endl;
        return false;
    }
    std::string line;
    std::getline(file, line);
    file.close();
    std::cout << tst_str<<" " << line << " " << (tst_str == line) << std::endl;
    return tst_str == line;
}
