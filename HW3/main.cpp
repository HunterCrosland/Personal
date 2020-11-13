#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include "compare.h"

int main()
{

    namespace fs = std::filesystem;
    std::string to_path;

    //dir query
    std::cout << "Please enter your directory: ";
    std::cin >> to_path;
    fs::path p1(to_path);
    
    //inits compare class
    compare new_compare(p1);

    //calls call_funcs which does the rest of the work
    new_compare.call_funcs();


    return 0;

}