#include <iostream>
#include <unistd.h>
#include "common.h"

int main()
{
    setlocale(LC_ALL, "");

    World world;
    try{
        playGame(world);
    } catch(std::runtime_error err){
        printf("\n\n%s\n", err.what());
        getchar();
    }

    return 0;
}
