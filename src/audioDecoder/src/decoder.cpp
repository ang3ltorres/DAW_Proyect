#include <../include/decoder.hpp>

void Decoder::changeStr (const std::string &newStr)
{
    myString = newStr;
}

std::string Decoder::getStr () {
    return myString;
}