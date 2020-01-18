#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <locale>
#include <algorithm>
#include <thread>
#include <math.h>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

#define ConsoleOutput std::wcout
#define ConsoleInput std::wcin
#define Delay(x) sleep_for(milliseconds(x))