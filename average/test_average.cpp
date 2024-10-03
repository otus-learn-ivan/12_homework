#define BOOST_TEST_MODULE test_average

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <iomanip>

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <thread>
#include <chrono>

#include <boost/test/unit_test.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <chrono>
#include <signal.h>

#include "../test_str_file.h"

BOOST_AUTO_TEST_SUITE(test_average)


BOOST_AUTO_TEST_CASE(test_test_av)
{
  BOOST_CHECK(true == true);
}

BOOST_AUTO_TEST_CASE(test_valid){
    system ("cat ../AB_NYC_2019.csv | ./mapper_av | sort -k1 |./reducer_av > output_av");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    BOOST_CHECK(test_str_file("output_av","152.62"));
}


BOOST_AUTO_TEST_SUITE_END()
