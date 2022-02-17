#ifndef NN_WIN_INCLUDED
#define NN_WIN_INCLUDED

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <process.h>
#include <ws2tcpip.h>

struct sockaddr_un {
    short sun_family;
    char sun_path[sizeof(struct sockaddr_storage) - sizeof(short)];
};

#define ssize_t int

#endif // !NN_WIN_INCLUDED
