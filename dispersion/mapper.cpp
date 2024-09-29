#include <iostream>
#include <string>


int main(int argc, char ** argv)
{
    if(argc > 1){
        std::cout << argv << "\t" << "1" << std::endl;
    }
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::cout << line << "\t" << "1" << std::endl;
    }

    return 0;
}
