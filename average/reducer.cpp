#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
struct Treduce_av{
    size_t number;
    size_t sum;
    double get_average(){
        return sum/number/100.0;
    }
};

int main()
{
    std::string line;
    vector<Treduce_av> reduce_av;
    while (std::getline(std::cin, line))
    {
        stringstream strm (line);
        Treduce_av value;
        strm >>value.number;
        strm >>value.sum;
        reduce_av.push_back(value);
    }
    Treduce_av all_av{};
    for(const auto& av:reduce_av){
        all_av.number+=av.number;
        all_av.sum+=av.sum;
    }
    std::cout << all_av.get_average() << std::endl;
    return 0;
}
