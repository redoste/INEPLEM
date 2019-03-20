REM	Copyright: 2019 redoste
REM
REM	This file is part of INEPLEM.
REM
REM	INEPLEM is free software: you can redistribute it and/or modify
REM	it under the terms of the GNU General Public License as published by
REM	the Free Software Foundation, either version 3 of the License, or
REM	(at your option) any later version.
REM
REM	INEPLEM is distributed in the hope that it will be useful,
REM	but WITHOUT ANY WARRANTY; without even the implied warranty of
REM	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM	GNU General Public License for more details.
REM
REM	You should have received a copy of the GNU General Public License
REM	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
REM

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
windres ressource.rc -O coff -o obj/ressource.res
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
g++ -std=c++17 noneAuth.cpp -c -o obj/noneAuth.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 ui.cpp -c -o obj/ui.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 serviceToUi.cpp -c -o obj/serviceToUi.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 uiToService.cpp -c -o obj/uiToService.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 images.cpp -c -o obj/images.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 runAs.cpp -c -o obj/runAs.o -Wall -Wextra -DWIN32 %cflags%
g++ -std=c++17 obj/ressource.res obj/main.o obj/watchdog.o obj/serviceMain.o obj/serviceCore.o obj/net.o obj/qt.o obj/vncServer.o obj/italcExtension.o obj/msAuth.o obj/dh.o obj/d3des.o obj/noneAuth.o obj/ui.o obj/serviceToUi.o obj/uiToService.o obj/images.o obj/runAs.o -lvncserver -lFreeImageLite -lws2_32 %lflags%
@echo off
goto end

:error
echo Invalid build target
:end