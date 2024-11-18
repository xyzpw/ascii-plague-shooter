#include <iostream>
#include <string>
#include <ctime>
#include "common.h"
#include <unistd.h>

int main()
{
    std::srand(std::time(0));
    setlocale(LC_ALL, "");

    World world;
    try{
        playGame(world);
    } catch(std::runtime_error err){
        std::cout << std::endl << std::endl << err.what() << std::endl;
        getchar();
    }

    return 0;
}
