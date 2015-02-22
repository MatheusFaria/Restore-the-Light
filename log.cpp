#include "log.h"

#include <iostream>
#include <string>

namespace Log{
    void error(std::string place, std::string errorMessage, std::string expected, std::string got){
        std::cerr << place << ": " << errorMessage;
        std::cerr << "\nExpected: " << expected << " -- Got: " << got << "\n";
    }

    void warn(std::string place, std::string warnMessage){
        std::cerr << place << ": " << warnMessage << "\n";
    }

    void out(std::string message){
        std::cout << message << "\n";
    }
}