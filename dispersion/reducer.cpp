#include <iostream>
#include <string>

#include <unordered_map>
#include <map>
#include <sstream>
#include <cmath>


int main()
{
    using namespace std;

    size_t count_all = 0;
    std::map<double,size_t> members;
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::stringstream strm (line);
        double key;
        size_t count;
        strm >>key;
        strm >>count;
        members[key]+=count;
        count_all+=count;
    }
    double sum1{},sum2{};
    for(const auto& pair:members){
        double p_i = static_cast<double>(pair.second)/count_all;
        sum1+= pow(pair.first,2)*p_i;
        sum2+= pair.first*p_i;
    }
    double dispers = sum1 - pow(sum2,2);
    std::cout << dispers << "\n";
    return 0;
}
