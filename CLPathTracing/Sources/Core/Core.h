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


/** OpenGL Libraries **/
#include <GL/glew.h>	   /* OpenGL Extension Loader */
#include <GL/glut.h>       /* OpenGL Utility Toolkit */


/** OpenCL Libraries */
#define __CL_ENABLE_EXCEPTIONS
#include "OpenCL/cl.hpp" /* OpenCL C++ Bindings */


using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

using namespace cl;

template <typename T>
std::string toStringWithPrecision(const T aValue, const int n = 2)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << aValue;
	return out.str();
}

/* Custom macros */
#define ConsoleOutput std::cout
#define ConsoleInput std::cin
#define ConsoleError std::cerr

#define Delay(x) sleep_for(milliseconds(x))
#define WaitForEnter() ConsoleOutput << "Press [ENTER] to continue..."; do {} while (getchar() != '\n')
#define ARRAY_SIZE(arr) *(&arr + 1) - arr

