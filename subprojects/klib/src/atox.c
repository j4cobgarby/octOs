#include "klib.h"
#include "kio.h"

double katof(const char *s) {
    return 0.0;
}

int katoi(const char *s) {
    int val = 0;
    size_t ndigits = kstrlen(s);
    char prefix = 0;

    static int pows10[11] = {
        1,10,100,1000,10000,100000,
        1000000,10000000,100000000,1000000000
    };

    prefix = s[0];
    if (prefix == '-' || prefix == '+') {
        // If first char is + or -, move on to the number itself
        s++;
    }

    for (size_t d = 0; d < ndigits; d++) {
        if (s[d] > '9' || s[d] < '0') return 0;
        val += (s[d]-'0') * pows10[ndigits-d-1];
    }

    if (prefix == '-') val = -val;

    return val;
}

long katol(const char *s) {
    return (long)katoi(s);
}

long long katoll(const char *s) {
    return 0;
}