#pragma once
#pragma warning(disable : 4996)

/* C++ Standard libraries */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <streambuf>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <thread>
#include <math.h>

/* OpenCL C++ Bindings */
#define __CL_ENABLE_EXCEPTIONS
#include "OpenCL/cl.hpp"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

using namespace cl;

/* Custom macros */
#define ConsoleOutput std::cout
#define ConsoleInput std::cin
#define ConsoleError std::cerr


#define Delay(x) sleep_for(milliseconds(x))
#define WaitForEnter() ConsoleOutput << "Press [ENTER] to continue..."; do {} while (getchar() != '\n')