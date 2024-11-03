#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

std::string Format::ElapsedTime(long sec) {
    unsigned hours = sec / 3600;
    unsigned minutes = (sec - hours * 3600) / 60;
    unsigned seconds = sec - hours * 3600 - minutes * 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(4) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
    return oss.str();
}