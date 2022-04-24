#include "format.h"

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

using std::string;

string Format::ElapsedTime(long int seconds) { 
    std::stringstream stringstream;
    stringstream << std::setfill('0') << std::setw(2) << (seconds / 3600) << ":";
    stringstream << std::setfill('0') << std::setw(2) << (seconds % 3600) / 60 << ":";
    stringstream << std::setfill('0') << std::setw(2) << (seconds % 3600) % 60;
    return stringstream.str();
}