#include <unordered_map>
#include <string>
#include "arg_utils.h"

std::unordered_map<std::string, std::string> parseCmdArgs(int argc, char* argv[])
{
    std::unordered_map<std::string, std::string> args;

    auto isKey = [&](std::string txt){
        if (txt.substr(0, 2) == "--")
            return true;
        return false;
    };

    for (int i = 1; i < argc; ++i)
    {
        std::string argValue = argv[i];
        if (isKey(argValue))
        {
            std::string key = argValue.substr(2);

            if (i + 1 < argc && !isKey(argv[i + 1])){
                args[key] = argv[i + 1];
            } else {
                args[key] = "";
            }
        }
    }
    return args;
}
