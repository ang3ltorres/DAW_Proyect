#pragma once

#include <fstream>
#include <iostream>
#include <string>

class Decoder {
    public:
        void changeStr(const std::string&);
        std::string getStr();
    private:
        std::string myString = "foo";
};