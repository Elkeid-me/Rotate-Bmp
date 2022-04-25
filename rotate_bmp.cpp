#include "Bitmap.h"
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
enum class PARAM_ERROR
{
    TOO_MANY_PARAM,
    TOO_FEW_PARAM
};
void paramErrMsg(const char *param, const char *message);
int main(int argc, char *argv[])
{
    try
    {
        if (argc < 3)
            throw PARAM_ERROR::TOO_FEW_PARAM;
        else if (argc > 3)
            throw PARAM_ERROR::TOO_MANY_PARAM;
        Bitmap bitmap(argv[1]);
        bitmap.Rotate90();
        bitmap.Save(argv[2]);
    }
    catch (PARAM_ERROR error)
    {
        switch (error)
        {
        case PARAM_ERROR::TOO_FEW_PARAM:
            std::cout << "\033[31m"
                      << "Too few parameters."
                      << "\033[0m" << std::endl;
            break;
        case PARAM_ERROR::TOO_MANY_PARAM:
            for (int i{3}; i < argc; i++)
                paramErrMsg(argv[i], "Too many parameters.");
            break;
        }
        return EXIT_FAILURE;
    }
    catch (FILE_ERROR error)
    {
        switch (error)
        {
        case FILE_ERROR::SRC_NOT_VALID:
            paramErrMsg(argv[1], "File is NOT a valid 24-bit or 32-bit .bmp bitmap.");
            break;
        case FILE_ERROR::OPEN_ERROR:
            paramErrMsg(argv[1], "File does NOT exist or does NOT have permission.");
            break;
        case FILE_ERROR::SAVE_NOT_BMP:
            paramErrMsg(argv[2], "Destination path is NOT a .bmp file.");
            break;
        case FILE_ERROR::SAVE_ERROR:
            paramErrMsg(argv[2], "Destination path is NOT valid. Maybe no permission.");
            break;
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
void paramErrMsg(const char *arg, const char *message)
{
    std::cout << arg << std::endl;
    std::cout << "\033[32m" << std::setiosflags(std::ios::left) << std::setfill('~') << std::setw(strlen(arg)) << "^" << std::endl;
    std::cout << "|\n|\n";
    std::cout << "\033[31m" << message << "\033[0m" << std::endl;
}
