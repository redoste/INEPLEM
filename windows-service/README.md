windows-service building documentation
======================================

Dependencies
------------

* [MinGW with Msys](http://mingw.org/)
* [cmake](https://cmake.org/)
* [libvnc v0.9.12](https://libvnc.github.io/)
* [FreeImage lite](https://github.com/WohlSoft/libFreeImage) (commit `48625fbc59a4f953e1f4a92458a805edd3dbf015`)

Patches
-------

### LibVnc

LibVNC must be patched before compiling: in `libvncserver/auth.c` comment the following code:

Line 302:
```c
/*if (!cl->screen->authPasswdData || cl->reverseConnection) {
	/* chk if this condition is valid or not.
	securityType = rfbSecTypeNone;
} else if (cl->screen->authPasswdData) {
    securityType = rfbSecTypeVncAuth;
}*/
```

And line 213:
```c
/*switch (primaryType) {
case rfbSecTypeNone:
    rfbRegisterSecurityHandler(&VncSecurityHandlerNone);
    break;
case rfbSecTypeVncAuth:
    rfbRegisterSecurityHandler(&VncSecurityHandlerVncAuth);
    break;
}*/
```

Another patch is required in `rfb/rfb.h` due to an unknown bug with MinGW headers:

Line 57:
```c
//typedef UINT32 in_addr_t;
```

### FreeImage Lite

Due to usage of the FreeImage Lite statically a patch is required in `FreeImageLite.h`:

Line 44:
```c
#define DLL_CALLCONV // __stdcall
```

And line 54:

```c
#define DLL_API // __declspec(dllimport)
```
Building and installing the libraries
-------------------------------------

You can configure libvnc and FreeImage lite with the following cmake parameters:
```
cmake.exe .. -G "MSYS Makefiles" -D CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -DWINVER=0x0601 -D_WIN32_WINNT=0x0601"
```
Simply use msys `make` to finish the build.

For installing them copy the header files in the `include` folder of MinGW and library archives in the `lib` folder.

Building INEPLEM
----------------

You can launch `build.bat` using two different parameters:

For building the `debug` binary without optimisation and with dynamic libraries use:
```
build.bat d
```

For building the `release` binary with optimisation and with static libraries use:
```
build.bat r
```
It is the build required by the `linux-loader`
