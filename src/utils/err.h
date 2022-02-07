#ifndef NN_ERR_INCLUDED
#define NN_ERR_INCLUDED

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../nn.h"

#include "fast.h"

#if defined(_MSC_VER)
#define NN_NORETURN __declspec(noreturn)
#elif defined(__GNUC__)
#define NN_NORETURN __attribute__((noreturn))
#else
#define NN_NORETURN
#endif

#define nn_assert(x)                                                           \
    do {                                                                       \
        if (nn_slow(!(x))) {                                                   \
            nn_backtrace_print();                                              \
            fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #x, __FILE__,    \
                __LINE__);                                                     \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define nn_assert_state(obj, state_name)                                       \
    do {                                                                       \
        if (nn_slow((obj)->state != state_name)) {                             \
            nn_backtrace_print();                                              \
            fprintf(stderr,                                                    \
                "Assertion failed: %d == %s (%s:%d)\n"(obj)->state,            \
                #state_name, __FILE__, __LINE__);                              \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define alloc_assert(x)                                                        \
    do {                                                                       \
        if (nn_slow(!x)) {                                                     \
            nn_backtrace_print();                                              \
            fprintf(stderr, "%s [%d] (%s:%d)\n", nn_err_strerror(errno),       \
                (int)errno, __FILE__, __LINE__);                               \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define errno_assert(x)                                                        \
    do {                                                                       \
        if (nn_slow(!(x))) {                                                   \
            nn_backtrace_print();                                              \
            fprintf(stderr, "%s [%d] (%s:%d)\n", nn_err_strerror(errno),       \
                (int)errno, __FILE__, __LINE__);                               \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define errnum_assert(cond, err)                                               \
    do {                                                                       \
        if (nn_slow(!(cond))) {                                                \
            nn_backtrace_print();                                              \
            fprintf(stderr, "%s [%d] (%s:%d)\n", nn_err_strerror(err),         \
                (int)err, __FILE__, __LINE__);                                 \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define win_assert(x)                                                          \
    do {                                                                       \
        if (nn_slow(!(x))) {                                                   \
            char errstr[256];                                                  \
            DWORD errnum = WSAGetLastError();                                  \
            nn_backtrace_print();                                              \
            nn_win_error((int)errnum, errstr, 256);                            \
            fprintf(stderr, "%s [%d] (%s:%d)\n", errstr, (int)errnum,          \
                __FILE__, __LINE__);                                           \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define wsa_assert(x)                                                          \
    do {                                                                       \
        if (nn_slow(!(x))) {                                                   \
            char errstr[256];                                                  \
            DWORD errnum = WSAGetLastError();                                  \
            nn_backtrace_print();                                              \
            nn_wsa_error((int)errnum, errstr, 256);                            \
            fprintf(stderr, "%s [%d] (%s:%d)\n", errstr, (int)errnum,          \
                __FILE__, __LINE__);                                           \
            fflush(stderr);                                                    \
            nn_err_abort();                                                    \
        }                                                                      \
    } while (0)

#define nn_fsm_assert(message, state, src, type)                               \
    do {                                                                       \
        nn_backtrace_print();                                                  \
        fprintf(stderr, "%s: state=%d source=%d action=%d (%s:%d)\n", message, \
            state, src, type, __FILE__, __LINE__);                             \
        fflush(stderr);                                                        \
        nn_err_abort();                                                        \
    } while (0)

#define nn_fsm_bad_action(state, src, type)                                    \
    nn_fsm_error("Unexpected action", state, src, type)
#define nn_fsm_bad_state(state, src, type)                                     \
    nn_fsm_error("Unexpected state", state, src, type)
#define nn_fsm_bad_source(state, src, type)                                    \
    nn_fsm_error("Unexpected source", state, src, type)

#define CT_ASSERT_HELPER2(prefix, line) prefix##line
#define CT_ASSERT_HELPER1(prefix, line) CT_ASSERT_HELPER2(prefix, line)
#if defined(__COUNTER__)
#define CT_ASSERT(x)                                                           \
    typedef int CT_ASSERT_HELPER1(ct_assert_, __COUNTER__)[(x) ? 1 : -1]
#else
#define CT_ASSERT(x)                                                           \
    typedef int CT_ASSERT_HELPER1(ct_assert_, __LINE__)[(x) ? 1 : -1]
#endif

NN_NORETURN void nn_err_abort(void);
int nn_err_errno(void);
const char *nn_err_strerror(int errnum);
void nn_backtrace_print(void);

#ifdef NN_HAVE_WINDOWS
int nn_err_wsa_to_posix(int wsaerr);
void nn_win_error(int err, char *buf, size_t bufsize);
#endif

#endif // !NN_ERR_INCLUDED
