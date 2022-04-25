#ifndef BITMAP_H
#define BITMAP_H
#include <cstdint>

enum class FILE_ERROR
{
    SRC_NOT_VALID,
    OPEN_ERROR,
    SAVE_NOT_BMP,
    SAVE_ERROR
};
class Bitmap
{
private:
    struct FileHeader
    {
        static constexpr int FILE_HEADER_SIZE{14};

        char data[14];
        char *bfType{data};
        uint32_t &bfSize{*reinterpret_cast<uint32_t *>(data + 2)};
        uint16_t &bfReserved1{*reinterpret_cast<uint16_t *>(data + 6)};
        uint16_t &bfReserved2{*reinterpret_cast<uint16_t *>(data + 8)};
        uint32_t &bfOffsetBits{*reinterpret_cast<uint32_t *>(data + 10)};
        FileHeader(const char *path);
    };
    struct DibHeader
    {
        char data[40];
        uint32_t &biSize{*reinterpret_cast<uint32_t *>(data)};

        uint32_t &biWidth{*reinterpret_cast<uint32_t *>(data + 4)};
        uint32_t &biHeight{*reinterpret_cast<uint32_t *>(data + 8)};

        uint16_t &biPlanes{*reinterpret_cast<uint16_t *>(data + 12)};
        uint16_t &biBitCount{*reinterpret_cast<uint16_t *>(data + 14)};

        uint32_t &biCompression{*reinterpret_cast<uint32_t *>(data + 16)};

        uint32_t &biSizeImage{*reinterpret_cast<uint32_t *>(data + 20)};

        uint32_t &biXPelsPerMeter{*reinterpret_cast<uint32_t *>(data + 24)};
        uint32_t &biYPelsPerMeter{*reinterpret_cast<uint32_t *>(data + 28)};

        uint32_t &biClrUsed{*reinterpret_cast<uint32_t *>(data + 32)};
        uint32_t &biClrImportant{*reinterpret_cast<uint32_t *>(data + 36)};
        char *others;
        DibHeader(const char *path, int skip);
        ~DibHeader();
    };
    struct Matrix
    {
        char *pix;
        uint32_t mWidth, mHeight, mPadding;
        Matrix(const char *path, uint32_t offset, uint32_t imageSize, uint32_t width, uint32_t height, uint32_t padding);
        Matrix(uint32_t imageSize, uint32_t width, uint32_t height, uint32_t padding);

        char *operator()(uint32_t x, uint32_t y);
    };
    FileHeader fileHeader;
    DibHeader dibHeader;
    Matrix matrix;
    uint32_t iccConfigSize;
    char *iccConfig;

public:
    Bitmap(const char *path);
    ~Bitmap();
    void Rotate90();
    void Save(const char *path);
};
#endif // BITMAP_H
