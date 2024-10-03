#ifndef TEST_STR_FILE_H
#define TEST_STR_FILE_H
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

bool test_str_file(std::string name_file,std::string tst_str);

#endif // TEST_STR_FILE_H
