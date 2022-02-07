#include <ctype.h>
#include "strcasecmp.h"

int nn_strcasecmp(const char *a, const char *b)
{
    int rv;
    for (;;) {
        if ((*a == 0) && (*b == 0)) {
            return (0);
        }
        if ((rv = (tolower(*a) - tolower(*b))) != 0) {
            return (rv);
        }
        a++;
        b++;
    }
    return (0);
}
