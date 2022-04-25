#include "Bitmap.h"
#include <cstring>
#include <fstream>
#include <regex>
uint32_t BYTE_PER_PIX;
Bitmap::FileHeader::FileHeader(const char *path)
{
    // 检查源文件路径是否为*.bmp
    std::regex checkBmp(".*\\.bmp", std::regex_constants::icase);
    if (!std::regex_match(path, checkBmp))
        throw FILE_ERROR::SRC_NOT_VALID;

    std::ifstream ifs(path, std::ios::binary);
    if (ifs.fail())
        throw FILE_ERROR::OPEN_ERROR;

    // 读取与检查 Magic Number
    ifs.read(data, 14);
    if (bfType[0] != 'B' || bfType[1] != 'M')
        throw FILE_ERROR::SRC_NOT_VALID;

    ifs.close();
}

Bitmap::DibHeader::DibHeader(const char *path, int skip)
{
    std::ifstream ifs(path, std::ios::binary);
    ifs.seekg(skip);
    ifs.read(data, 40);
    switch (biBitCount)
    {
    case 24:
        BYTE_PER_PIX = 3;
        break;
    case 32:
        BYTE_PER_PIX = 4;
        break;
    default:
        throw FILE_ERROR::SRC_NOT_VALID;
    }
    uint32_t padding{(4 - ((biWidth * BYTE_PER_PIX) & 0x3)) & 0x3};
    biSizeImage = (biWidth * BYTE_PER_PIX + padding) * biHeight;
    if (biSize > 40)
    {
        others = new char[biSize - 40];
        ifs.read(others, biSize - 40);
    }
    else
        others = nullptr;
    ifs.close();
}
Bitmap::DibHeader::~DibHeader()
{
    if (others)
        delete[] others;
}
Bitmap::Matrix::Matrix(const char *path, uint32_t offset, uint32_t imageSize, uint32_t width, uint32_t height, uint32_t padding)
    : pix{new char[imageSize]},
      mWidth{width},
      mHeight{height},
      mPadding{padding}
{
    std::ifstream ifs(path, std::ios::binary);
    ifs.seekg(offset);
    ifs.read(pix, imageSize);
}
Bitmap::Matrix::Matrix(uint32_t imageSize, uint32_t width, uint32_t height, uint32_t padding)
    : pix{new char[imageSize]},
      mWidth{width},
      mHeight{height},
      mPadding{padding} {}
char *Bitmap::Matrix::operator()(uint32_t x, uint32_t y) { return pix + (mWidth * BYTE_PER_PIX + mPadding) * y + x * BYTE_PER_PIX; }
Bitmap::Bitmap(const char *path)
    : fileHeader(path),
      dibHeader(path, Bitmap::FileHeader::FILE_HEADER_SIZE),
      matrix(path, fileHeader.bfOffsetBits, dibHeader.biSizeImage,
             dibHeader.biWidth, dibHeader.biHeight,
             (4 - ((dibHeader.biWidth * BYTE_PER_PIX) & 0x3)) & 0x3),
      iccConfigSize{fileHeader.bfSize - fileHeader.bfOffsetBits - dibHeader.biSizeImage}
{
    if (iccConfigSize)
    {
        iccConfig = new char[iccConfigSize];
        std::ifstream ifs(path);
        ifs.seekg(fileHeader.bfOffsetBits + dibHeader.biSizeImage);
        ifs.read(iccConfig, iccConfigSize);
    }
    else
        iccConfig = nullptr;
}
Bitmap::~Bitmap()
{
    if (matrix.pix)
        delete[] matrix.pix;
}
void Bitmap::Rotate90()
{
    std::swap(dibHeader.biWidth, dibHeader.biHeight);

    uint32_t newPadding{(4 - ((dibHeader.biWidth * BYTE_PER_PIX) & 0x3)) & 0x3};
    uint32_t desSize{(dibHeader.biWidth * BYTE_PER_PIX + newPadding) * dibHeader.biHeight};
    uint32_t difference{desSize - dibHeader.biSizeImage}; // 这里不必考虑desSize与dibHeader.biSizeImage孰大孰小的问题，因为无符号整数的溢出是回绕的
    fileHeader.bfSize += difference;                      // 这里也一样
    dibHeader.biSizeImage = desSize;
    std::swap(dibHeader.biXPelsPerMeter, dibHeader.biYPelsPerMeter);
    Matrix tmp(dibHeader.biSizeImage, dibHeader.biWidth, dibHeader.biHeight, newPadding);
    for (uint32_t x{0}; x < matrix.mWidth; x++)
    {
        for (uint32_t y{0}; y < matrix.mHeight; y++)
            std::memcpy(tmp(y, tmp.mHeight - x - 1), matrix(x, y), BYTE_PER_PIX);
    }
    if (matrix.pix)
        delete[] matrix.pix;
    matrix.pix = tmp.pix;
}
void Bitmap::Save(const char *path)
{
    // 检查保存路径是否为*.bmp
    std::regex checkBmp(".*\\.bmp", std::regex_constants::icase);
    if (!std::regex_match(path, checkBmp))
        throw FILE_ERROR::SAVE_NOT_BMP;

    std::ofstream ofs(path, std::ios::binary);
    ofs.write(fileHeader.data, 14);
    ofs.write(dibHeader.data, 40);
    if (dibHeader.others)
        ofs.write(dibHeader.others, dibHeader.biSize - 40);
    if (ofs.fail())
        throw FILE_ERROR::SAVE_ERROR;
    ofs.seekp(fileHeader.bfOffsetBits);
    ofs.write(matrix.pix, dibHeader.biSizeImage);
    if (iccConfig)
        ofs.write(iccConfig, iccConfigSize);
    ofs.close();
}
