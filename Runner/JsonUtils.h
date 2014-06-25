#pragma once
#include <string>

namespace JSON_UTIL
{


std::string stripQuotes(std::string valueString);
int indexOfEndOfNextItem(std::string json);

std::string firstItem(std::string json);
std::string removeFirstitem(std::string json);
}