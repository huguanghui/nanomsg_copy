#include <ctype.h>
#include "strncasecmp.h"

int nn_strncasecmp(const char *a, const char *b, size_t len)
{
    int rv = 0;
    size_t count;
    for (count = 0; count < len; count++) {
        if ((*a == 0) && (*b == 0)) {
            return 0;
        }
        if ((rv = tolower(*a) - tolower(*b)) != 0) {
            return rv;
        }
        a++;
        b++;
    }
    return (0);
}
