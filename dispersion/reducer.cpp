#include <iostream>
#include <string>


//struct

int main(int argc, char ** argv)
{
    if(argc > 1){
        std::cout << argv << "\t" << "1" << std::endl;
    }
    size_t count = 0;
    std::string line;
    while (std::getline(std::cin, line))
    {
        count += 1;
    }
    std::cout << count << std::endl;
    return 0;
}
