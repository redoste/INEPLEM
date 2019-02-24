INEPLEM
=======

**INEPLEM** (Italc Ne sEra PLus dE ce Monde) est un service Windows temporaire remplaçant **icas** (Italc Client Service) et installable via un périphérique bootable.
Il est composé de deux grandes parties :
* Le service Windows, développé en C++ à l'aide de l'API Windows native. (`Windows.h`)
* La minuscule *distribution* (si on peut l'appeler comme ca) Linux, écrite *from scratch* (elle n'est pas basé sur une autre distrib), fournis avec les scripts d'instalation du service

Pour la compilation du windows-service
--------------------------------------

* Configuration d'example pour libvncserver
`cmake.exe ../libvncserver-LibVNCServer-0.9.12/ -G "MSYS Makefiles" -D CMAKE_C_FLAGS="${CMAKE_C_FLAGS} -DWINVER=0x0501 -D_WIN3_WINNT=0x0501"`
Avec msys, mingw et cmake dans le path (un rollback de mingw32-w32api à la 5.0.1 peut être nécésaire)

* Patch pour la libvncserver:

rfb.h: Ligne 57, commenter `//typedef UINT32 in_addr_t;`

auth.c: Ligne 302, commenter

```
/*if (!cl->screen->authPasswdData || cl->reverseConnection) {
	/* chk if this condition is valid or not.
	securityType = rfbSecTypeNone;
} else if (cl->screen->authPasswdData) {
    securityType = rfbSecTypeVncAuth;
}*/
```

et ligne 213, commenter

```
/*switch (primaryType) {
case rfbSecTypeNone:
    rfbRegisterSecurityHandler(&VncSecurityHandlerNone);
    break;
case rfbSecTypeVncAuth:
    rfbRegisterSecurityHandler(&VncSecurityHandlerVncAuth);
    break;
}*/
```