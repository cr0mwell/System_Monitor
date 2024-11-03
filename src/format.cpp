#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

string Format::ElapsedTime(long int seconds) {
    long hours, minutes;
    hours = seconds / 3600;
    seconds = seconds % 3600;
    minutes = seconds / 60;
    seconds = seconds % 60;

    string sec = std::to_string(seconds);
    sec.insert(0, 2 - sec.length(), '0');

    string mins = std::to_string(minutes);
    mins.insert(0, 2 - mins.length(), '0');

    string hrs = std::to_string(hours);
    hrs.insert(0, 2 - hrs.length(), '0');

    string result = hrs + ":" + mins + ":" + sec;
    return result;
}