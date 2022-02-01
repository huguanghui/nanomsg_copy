#include "err.h"
#include <string.h>

#ifdef NN_HAVE_WINDOWS
#include "win.h"
#endif

#ifdef NN_HAVE_BACKTRACE

void nn_backtrace_print(void)
{
    void *frames[50];
    int size;
    size = backtrace(frames, sizeof(frames) / sizeof(frames[0]));
    if (size > 1) {
        backtrace_symbols_fd(&frames[1], size - 1, fileno(stderr));
    }
}

#else
void nn_backtrace_print(void) {}
#endif /* NN_HAVE_BACKTRACE */

#include <stdlib.h>

void nn_err_abort(void) { abort(); }

int nn_err_errno(void) { return errno; }

const char *nn_err_strerror(int errnum)
{
    switch (errnum) {
#if defined(NN_HAVE_WINDOWS)
    case ENOTSUP:
        return "Not supported";
    case EPROTONOSUPPORT:
        return "Protocol not supported";
    case ENOBUFS:
        return "No buffer space available";
    case ENETDOWN:
        return "Network is down";
    case EADDRINUSE:
        return "Address in use";
    case EADDRNOTAVAIL:
        return "Adress not available";
    case ECONNREFUSED:
        return "Connection refused";
    case EINPROGRESS:
        return "Operation in progress";
    case ENOTSOCK:
        return "Not a socket";
    case EAFNOSUPPORT:
        return "Address family not supported";
    case EPROTO:
        return "Protocol error";
    case EAGAIN:
        return "Resource unavailable, try again";
    case EBADF:
        return "Bad file descriptor";
    case EINVAL:
        return "Invalid argument";
    case EMFILE:
        return "Too many open files";
    case EFAULT:
        return "Bad address";
    case EACCES:
        return "Permission denied";
    case ENETRESET:
        return "Connection aborted by network";
    case ENETUNREACH:
        return "Network unreachable";
    case EHOSTUNREACH:
        return "Host is unreachable";
    case ENOTCOMM:
        return "The socket is not connected";
    case EMSGSIZE:
        return "Message too large";
    case ETIMEOUT:
        return "Timed out";
    case ECONNABORTED:
        return "Connection aborted";
    case ECONNRESET:
        return "Connection reset";
    case ENOPROTOOPT:
        return "Protocol not available";
    case EISCONN:
        return "Socket is connected";
#endif
    case ETERM:
        return "Nanomsg library was terminated";
    case EFSM:
        return "Operation cannot be performed in this state";
    default:
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
        return strerror(errnum);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
    }
    return strerror(errnum);
}

#ifdef NN_HAVE_WINDOWS

int nn_err_wsa_to_posix(int wsaerr)
{
    switch (wsaerr) {
    case WSAEINPROGRESS:
        return EAGAIN;
    case WSAEBADF:
        return EBADF;
    case WSAEINVAL:
        return EINVAL;
    case WSAEMFILE:
        return EMFILE;
    case WSAEFAULT:
        return EFAULT;
    case WSAEPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    case WSAENOBUFS:
        return ENOBUFS;
    case WSAENETDOWN:
        return ENETDOWN;
    case WSAEADDRINUSE:
        return EADDRINUSE;
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case WSAEAFNOSUPPORT:
        return EAFNOSUPPORT;
    case WSAEACCES:
        return EACCES;
    case WSAENETRESET:
        return ENETRESET;
    case WSAENETUNREACH:
        return ENETUNREACH;
    case WSAEHOSTUNREACH:
        return EHOSTUNREACH;
    case WSAENOTCONN:
        return ENOTCOMM;
    case WSAEMSGSIZE:
        return EMSGSIZE;
    case WSAETIMEDOUT:
        return ETIMEOUT;
    case WSAECONNREFUSED:
        return ECONNREFUSED;
    case WSAECONNABORTED:
        return ECONNABORTED;
    case WSAECONNRESET:
        return ECONNRESET;
    case WSAENOTSOCK:
        return ENOTSOCK;
    case ERROR_BROKEN_PIPE:
        return ECONNRESET;
    case WSAESOCKTNOSUPPORT:
        return ESOCKTNOSUPPORT;
    case ERROR_NOT_CONNECTED:
        return ENOTCONN;
    case ERROR_PIPE_NOT_CONNECTED:
        return ENOTCONN;
    case ERROR_NO_DATA:
        return EPIPE;
    default:
        nn_assert(0);
    }
}

void nn_win_error(int err, char *buf, size_t bufsize)
{
    DWORD rc = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
        (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf,
        (DWORD)bufsize, NULL);
    nn_assert(rc);
}

#endif
