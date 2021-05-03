#include <iostream>
#include "file.hpp"

int main(int argc, const char* argv[]){
    File test("test_file.txt");
    std::cout << test[0] << std::endl;
    return 0;
}