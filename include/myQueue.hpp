#include <iostream>
#include <vector>

class CircleDataSet
{
public:
    int size = 1;
    std::vector<std::string> data;
    CircleDataSet(int l)
    {
        size = l;
    }
    // private:
    void startDataSet()
    {
        for (int cursor = size; cursor != 0; cursor--)
        {
            data.push_back("None");
            std::cout << data.at(0);
        }
    }
    std::vector<std::string> manageDataSet(const char *newEntry)
    {
        for (int cursor = 0; cursor != size; cursor++)
        {
            if (data[cursor] == "None")
            {
                data[cursor] = newEntry;
            }
            else
            {
                if (cursor != size - 1)
                {
                    data[cursor] = data[cursor + 1];
                }
                else
                {
                    data[cursor] = newEntry;
                }
            }
        }
        return data;
    }
    bool isEmpty()
    {
        bool isEmpty;
        for (int cursor = 0; cursor != size; cursor++)
        {
            if (data.at(cursor) != "None")
            {
                isEmpty = true;
                return isEmpty;
            }
        }
        isEmpty = false;
        return isEmpty;
    }
    std::vector<std::string> giveDataSet()
    {
        return data;
    }
    void cleanDataSet(){
        for (int cursor = 0; cursor != size; cursor++)
        {
            data[cursor] = "None";
        }
    }
};

// int main()
// {
//     CircleDataSet myData(5);

//     myData.startDataSet();

//     myData.manageDataSet("1");
//     myData.manageDataSet("2");
//     myData.manageDataSet("3");
//     myData.manageDataSet("4");
//     myData.manageDataSet("5");
//     myData.manageDataSet("6");
//     myData.manageDataSet("7");

//     std::vector<std::string> vect = myData.manageDataSet("8");

//     for (int i = 0; i < vect.size(); i++)
//     {
//         std::cout << vect.at(i) << std::endl;
//     }

//     return 0;
// }