#include "JsonUtils.h"


namespace JSON_UTIL
{

std::string stripQuotes(std::string valueString)
{
	std::size_t index = valueString.find_first_of('\"');
	if (index != std::string::npos)
	{
		valueString = valueString.substr(index+1);
		index =  valueString.find_first_of('\"');
		if (index != std::string::npos)
		{
			valueString = valueString.substr(0,index);
		}
	}
	return valueString;
}

int indexOfEndOfNextItem(std::string json)
{

	std::size_t firstNonSpacePos = json.find_first_not_of(" \n\t");

	if (firstNonSpacePos == std::string::npos)
	{
		return -1;
	}

	if (json[firstNonSpacePos] == '\"')
	{
		std::size_t endQuote = json.find_first_of("\"",firstNonSpacePos+1);
		return endQuote;
	} 
	else if (json[firstNonSpacePos] == '{' || json[firstNonSpacePos] == '(')
	{
		int nesting = 1;
		for (unsigned int i = firstNonSpacePos + 1; i < json.length(); i++)
		{
			if (json[i] == '{' || json[i] == '[')
			{
				nesting++;
			}
			if (json[i] == '}' || json[i] == ']')
			{
				nesting--;
				if (nesting == 0)
				{
					return i;
				}
			}

		}
		return -2;
	}
	else
	{
		std::size_t end = json.find_first_of(" ,\t\n:]}", firstNonSpacePos);
		if (end != std::string::npos)
		{
			return end-1;
		}
		else
		{
			return -3;
		}
	}

}


std::string firstItem(std::string json)
{
	std::size_t firstNonSpace = json.find_first_not_of(" \t\n");
	if (firstNonSpace != std::string::npos)
	{
		return json.substr(firstNonSpace,indexOfEndOfNextItem(json)+1 - firstNonSpace);
	}
	else
	{
		// TODO:  Should probably throw here ..
		return "";
	}
}

std::string removeFirstitem(std::string json)
{
	int end = indexOfEndOfNextItem(json);
	return json.substr(end+1);
}

std::string getFromArray(std::string json, int index)
{
	std::size_t firstBracket = json.find_first_of("[");
	if (firstBracket == std::string::npos)
	{
		// TODO:  should probably throw here ...
		return "";
	}
	json = json.substr(firstBracket+1);
	for (int i = 0; i < index; i++)
	{
		std::size_t firstNonSpace = json.find_first_not_of(" \t\n");
		if (firstNonSpace == std::string::npos)
		{
			// TODO:  should probably throw here ...
			return "";
		}
		if (json[firstNonSpace] == ']')
		{
			// TODO:  should probably throw here ...
			return "";
		}
		json = removeFirstitem(json);
		firstNonSpace = json.find_first_not_of(" \t\n");
		if (firstNonSpace == std::string::npos)
		{
			// TODO:  should probably throw here ...
			return "";
		}
		if (json[firstNonSpace] != ',')
		{
			// TODO:  should probably throw here ...
			return "";
		}
		json = json.substr(1);
	}
	std::size_t firstNonSpace = json.find_first_not_of(" \t\n");
	if (firstNonSpace != std::string::npos && json[firstNonSpace] != ']')
	{
		return firstItem(json);
	}
	return "";
}



std::string getFromDictionary(std::string json, std::string key)
{
	std::size_t firstBracket = json.find_first_of("{");
	if (firstBracket == std::string::npos)
	{
		// TODO:  should probably throw here ...
		return "";
	}

	std::string remainder = json.substr(firstBracket+1);

	std::size_t nextIndex = remainder.find_first_not_of("\t \n");
	while (nextIndex != std::string::npos && remainder[nextIndex] != '}')
	{
		std::string nextKey = JSON_UTIL::stripQuotes(JSON_UTIL::firstItem(remainder));
		remainder = JSON_UTIL::removeFirstitem(remainder);
	
		size_t colonIndex = remainder.find_first_of(":");
		if (colonIndex == std::string::npos)
		{
			// ERROR!
			nextIndex = std::string::npos;
			break;
		}
		remainder = remainder.substr(colonIndex+1);
		std::string value = JSON_UTIL::firstItem(remainder);
		remainder = JSON_UTIL::removeFirstitem(remainder);
		if (stripQuotes(nextKey) == stripQuotes(key))
		{
			return value;
		}

		nextIndex = remainder.find_first_not_of("\t \n");
		if (nextIndex != std::string::npos && remainder[nextIndex] == ',')
		{
			nextIndex++;
		}
		else
		{
			return "";
		}
		remainder = remainder.substr(nextIndex);
		nextIndex = remainder.find_first_not_of("\t \n");
	}
	// TODO:  Throw here?
	return "";
}

}