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
	std::size_t quotePos = json.find_first_of("\"");
	std::size_t bracePos = json.find_first_of("{");
	if (quotePos != std::string::npos && 
		(bracePos == std::string::npos) || (quotePos < bracePos))
	{
		std::size_t endQuote = json.find_first_of("\"",quotePos+1);
		return endQuote;
	}
	else if (bracePos != std::string::npos)
	{
		int nesting = 1;
		for (unsigned int i = bracePos + 1; i < json.length(); i++)
		{
			if (json[i] == '{')
			{
				nesting++;
			}
			if (json[i] == '}')
			{
				nesting--;
				if (nesting == 0)
				{
					return i;
				}
			}

		}
		return -1;
	}
	else
	{
		return -2;
	}

}


std::string firstItem(std::string json)
{
	return json.substr(0,indexOfEndOfNextItem(json)+1);
}

std::string removeFirstitem(std::string json)
{
	int end = indexOfEndOfNextItem(json);
	return json.substr(end+1);
}
}