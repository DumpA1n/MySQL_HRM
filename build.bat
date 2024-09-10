@echo off
:: 设置脚本出错时终止
setlocal enabledelayedexpansion
set errorlevel=0

:: 创建并进入 build 目录
if not exist build (
    mkdir build
)
cd build

:: 使用 CMake 生成 Ninja 构建文件
echo Generating build files with CMake...
cmake -G "Visual Studio 17 2022" .. || (
    echo CMake generation failed!
    exit /b 1
)

echo Building project...

:: 设置 Visual Studio 的开发环境（x64 平台）
call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

:: 进入项目目录
cd /d E:\MySQLProject\MySQL_HRM\build

:: 执行 msbuild 编译解决方案
msbuild MySQL_HRM.sln /p:Configuration=Release || (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!

:: 返回初始目录
cd ..

cd build\bin\Release\
.\MySQL_HRM.exe

:: copy build\bin\Release\MySQL_HRM.exe .\

:: 等待用户按键，防止窗口自动关闭
:: pause

:: 结束脚本
exit /b 0
