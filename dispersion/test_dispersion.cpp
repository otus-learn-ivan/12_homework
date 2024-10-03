#define BOOST_TEST_MODULE test_dispersion

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
#include <boost/filesystem.hpp>
#include <iostream>
#include <chrono>
#include <signal.h>
#include "../test_str_file.h"
BOOST_AUTO_TEST_SUITE(test_dispersion)


BOOST_AUTO_TEST_CASE(test_test_ds)
{
  BOOST_CHECK(true == true);
}

BOOST_AUTO_TEST_CASE(test_valid){
    system ("cat ../AB_NYC_2019.csv | ./mapper_ds | sort -k1 |./reducer_ds > output_ds");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    BOOST_CHECK(test_str_file("output_ds","57679.7"));
}

BOOST_AUTO_TEST_SUITE_END()
