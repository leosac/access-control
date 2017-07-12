#pragma once

#include <chrono>

#pragma db value(std::chrono::system_clock::time_point) type("TIMESTAMP") null
