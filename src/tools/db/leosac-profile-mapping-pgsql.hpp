#pragma once

#include <chrono>

#pragma db value(std::chrono::system_clock::time_point) type("TIMESTAMP") null
#pragma db value(std::chrono::milliseconds) type("BIGINT") null
