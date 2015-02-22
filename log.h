#ifndef __LOG_H__
#define __LOG_H__

#include <string>

namespace Log{
    void error(std::string place, std::string errorMessage, std::string expected, std::string got);
    void warn(std::string place, std::string warnMessage);
    void out(std::string message);
}

#endif