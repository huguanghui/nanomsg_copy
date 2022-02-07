#include <stdio.h>
#include <string.h>

#include "strncasecmp.h"


const char *nn_strcasestr(const char *str, const char *key)
{
    size_t len = strlen(key);

    while (*str != '\0') {
        if (nn_strncasecmp(str, key, len) == 0) {
            return str;
        }
        str++;
    }
    return (NULL);
}
