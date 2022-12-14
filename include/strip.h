#include <vector>
#include <string>
#include <iostream>

std::string subString(std::string str, int start, int end)
{
    std::string val;

    if (end < start)
    {
        return "";
    }
    else
    {
        for (int k = start; k < end; k++)
        {
            val += str.at(k);
        }
    }

    return val;
}

int indexOf(const char *str, char token)
{
    std::string str_ = std::string(str);
    int lenght = str_.length();

    for (int n = 0; n < lenght; n++)
    {
        if (str_[n] == token)
        {
            return n;
        }
    }

    return -1;
}

std::vector<std::string> strip(const char *str, char Token)
{
    std::vector<std::string> arr;
    std::string tempStr = std::string(str);
    while (tempStr.length() > 0)
    {
        int index = tempStr.find(Token);
        if (index == std::string::npos)
        {
            if (tempStr != "")
            {
                arr.push_back(tempStr);
            }
            break;
        }
        else
        {
            arr.push_back(subString(tempStr, 0, index).c_str());
            tempStr = subString(tempStr, index + 1, tempStr.length());
        }
    }

    return arr;
}
