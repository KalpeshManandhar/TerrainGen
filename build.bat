echo off
@REM setlocal

@REM set IMGUI_DIR=.\imgui
set IMGUI_DIR=
set GLAD_DIR=E:\libraries\glad
set GLFW_DIR=E:\libraries\glfw
set GLM_DIR=E:\libraries\glm
@REM set KHR_DIR=.\khr
set STB_DIR=E:\libraries\stb

@REM set INCLUDES=-I%IMGUI_DIR% -I%GLAD_DIR%\include -I%GLFW_DIR%\include -I%STB_DIR% -I%GLM_DIR% -I%KHR_DIR%
set INCLUDES=-I%GLAD_DIR%\include -I%GLFW_DIR%\include -I%STB_DIR% -I%GLM_DIR% 
set LIBS=-L%GLFW_DIR%\x64
set LINKS=-lopengl32 -lglfw3dll -lglfw3 -lglfw3_mt -lShell32 

set OUTPUT=out.exe
set OUTPUT_DIR=.\out

set SRC=.\src
@REM set SOURCES=%SRC%\*.cpp %IMGUI_DIR%\*.cpp %GLAD_DIR%\src\glad.c
set SOURCES=%SRC%\*.cpp %SRC%\math\*.cpp %GLAD_DIR%\src\glad.c

mkdir %OUTPUT_DIR%

clang++ -std=c++20 %INCLUDES% %LIBS% %SOURCES% -o %OUTPUT_DIR%\%OUTPUT% %LINKS% -Xlinker /subsystem:console

copy %GLFW_DIR%\x64\glfw3.dll %OUTPUT_DIR%\