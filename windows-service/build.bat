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
g++ -std=c++17 main.cpp -c -o obj/main.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 watchdog.cpp -c -o obj/watchdog.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 serviceMain.cpp -c -o obj/serviceMain.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 serviceCore.cpp -c -o obj/serviceCore.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 net.cpp -c -o obj/net.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 qt.cpp -c -o obj/qt.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 vncServer.cpp -c -o obj/vncServer.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 italcExtension.cpp -c -o obj/italcExtension.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 msAuth.cpp -c -o obj/msAuth.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 dh.cpp -c -o obj/dh.o -Wall -Wextra -DWIN32 %cflags%
gcc d3des.c -c -o obj/d3des.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 obj/main.o obj/watchdog.o obj/serviceMain.o obj/serviceCore.o obj/net.o obj/qt.o obj/vncServer.o obj/italcExtension.o obj/msAuth.o obj/dh.o obj/d3des.o -lvncserver -lws2_32 %lflags%
@echo off
goto end

:error
echo Invalid build target
:end