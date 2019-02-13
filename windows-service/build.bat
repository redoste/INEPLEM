@echo off
if "%1" == "d" goto debug
if "%1" == "r" goto release
goto error

:debug
set cflags=
set lflags=-o INEPLEM1.debug.exe
goto build
:release
set cflags=-Os -s
set lflags=-static -Os -s -o INEPLEM1.release.exe
goto build

:build
mkdir obj 2>nul
echo on
g++ -std=c++17 main.cpp -c -o obj/main.o -Wall -Wextra %cflags%
g++ -std=c++17 watchdog.cpp -c -o obj/watchdog.o -Wall -Wextra %cflags%
g++ -std=c++17 serviceMain.cpp -c -o obj/serviceMain.o -Wall -Wextra %cflags%
g++ -std=c++17 serviceCore.cpp -c -o obj/serviceCore.o -Wall -Wextra %cflags%
g++ -std=c++17 net.cpp -c -o obj/net.o -Wall -Wextra %cflags%
g++ -std=c++17 qt.cpp -c -o obj/qt.o -Wall -Wextra %cflags%
g++ -std=c++17 vncProxy.cpp -c -o obj/vncProxy.o -Wall -Wextra %cflags%
g++ -std=c++17 pipe.cpp -c -o obj/pipe.o -Wall -Wextra %cflags%
g++ -std=c++17 obj/main.o obj/watchdog.o obj/serviceMain.o obj/serviceCore.o obj/net.o obj/qt.o obj/vncProxy.o obj/pipe.o -lws2_32 %lflags%
@echo off
goto end

:error
echo Invalid build target
:end