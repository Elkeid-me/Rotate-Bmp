# 旋转 bmp

支持 24 位、32 位 bmp 位图，带有友好的错误提示。

## 编译

对于 Windows 用户，有编译用批处理文件 `compile.bat`。内容如下：

```
g++ -m64 -O2 -std=c++11 -o "rotatebmp.exe" "rotate_bmp.cpp" "Bitmap.cpp"
```

## 使用

终端中输入命令：

```
rotatebmp.exe "源文件名" "目标文件名"
```

**注意：**

- 源文件名和目标文件名必须包含扩展名 `.bmp`。
- 本程序只能接收两个参数，多或少都会报错。
