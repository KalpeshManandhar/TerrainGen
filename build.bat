echo off

@REM set these folders depending on your machine
set GLAD_DIR=E:\libraries\glad
set GLFW_DIR=E:\libraries\glfw
set GLM_DIR=E:\libraries\glm
set STB_DIR=E:\libraries\stb

set INCLUDES=-I%GLAD_DIR%\include -I%GLFW_DIR%\include -I%STB_DIR% -I%GLM_DIR% 
set LIBS=-L%GLFW_DIR%\x64
set LINKS=-lopengl32 -lglfw3dll -lglfw3 -lglfw3_mt -lShell32 

set OUTPUT=out.exe
set OUTPUT_DIR=.\out

set SRC=.\src
set SOURCES=%SRC%\*.cpp %SRC%\math\*.cpp %SRC%\Allocator\*.cpp %SRC%\Graphics\*.cpp %GLAD_DIR%\src\glad.c

mkdir %OUTPUT_DIR%

clang++ -std=c++20 %INCLUDES% %LIBS% %SOURCES% -o %OUTPUT_DIR%\%OUTPUT% %LINKS% -Xlinker /subsystem:console

copy %GLFW_DIR%\x64\glfw3.dll %OUTPUT_DIR%\