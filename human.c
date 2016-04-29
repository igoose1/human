/*
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as double
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "arg.h"

#define EXA  1152921504606846976
#define PETA 1125899906842624
#define TERA 1099511627776
#define GIGA 1073741824
#define MEGA 1048576
#define KILO 1024

#define DEFAULT_SCALE 0

/*
 * Help, I need somebody
 * Help, not just anybody
 * Help, you know I need someone
 * Help!
 *
 */
void usage (char *progname)
{
    printf("usage: %s [-hbkmgt] <number>\n", progname);
}

/*
 * calculate a power of number
 * disclaimers: return no more than a "long" so use wisely...
 *
 */
long power (long number, int pow)
{
    return pow > 0 ? number * power(number, pow - 1) : 1;
}

/*
 * read the environment varaible "SCALE" and returns its value.
 * returns DEFAULT_SCALE if it does not return a number.
 *
 */
int getscale()
{
    /* would you rather use getenv() twice ? or allocate a pointer ? */
    char *scale = NULL;
    scale = getenv("SCALE");

    /* in atoi, we trust. maybe. */
    return scale ? atoi(scale) : DEFAULT_SCALE;
}

/*
 * calculate the best factorization for a number, depending on its value.
 * actual max factorisation is 1024^5 (EiB)
 *
 */
char factorize (double number)
{
    return number >= EXA  ? 'E' :
           number >= PETA ? 'P' :
           number >= TERA ? 'T' :
           number >= GIGA ? 'G' :
           number >= MEGA ? 'M' :
           number >= KILO ? 'K' :
           0;
}

/*
 * calculate a human-readable version of the given number, depending of the
 * factorisation level given.
 *
 */
double humanize (double number, char factor)
{
    int pow = 0;

    /* cascading switch. note the lack of "break" statements */
    switch (factor) {
        case 'E' : pow++;
        case 'P' : pow++;
        case 'T' : pow++;
        case 'G' : pow++;
        case 'M' : pow++;
        case 'K' : pow++; break;
        default  : return number;
    }

    /* return the number divided by the correct factorization level */
    return number /= power(1024, pow);
}

/*
 * finally print a number in human-readable format,
 *
 */
int human(char* s, char fac)
{
    int pow = 0;
    double number = 0;

    switch (s[strnlen(s, LINE_MAX) - 1]) {
        case 'E': pow++;
        case 'P': pow++;
        case 'T': pow++;
        case 'G': pow++;
        case 'M': pow++;
        case 'K': pow++;
        case 'B': s[strnlen(s, LINE_MAX) - 1] = 0;
    }

    /* get the number and convert it to bytes. If there is none, strtold will return 0 */
    number  = strtold(s, NULL);
    number *= power(1024, pow);

    if (number < 0) {
        fprintf(stderr, "I ain't gonna do that. Deal with it.");
        return -1;
    }

    /* use explicit factorization. otherwise, guess the best one */
    fac = fac > 0 ? fac : factorize(number);

    /* actually print the result, isn't that what we're here for after all ? */
    printf("%.*f", getscale(), humanize(number, fac));
    if (fac && fac != 'B') {
        putchar(fac);
    }
    putchar('\n');
    return 0;
}

int main (int argc, char **argv)
{
    char fac = 0;
    char *argv0, in[LINE_MAX];

    /* only switches are use to force factorization */
    ARGBEGIN {
        case 'h': usage(argv0); exit(0); break;
        case 'e': fac = 'E'; break;
        case 'p': fac = 'P'; break;
        case 't': fac = 'T'; break;
        case 'g': fac = 'G'; break;
        case 'm': fac = 'M'; break;
        case 'k': fac = 'K'; break;
        case 'b': fac = 'B'; break;
        default: break;
    } ARGEND;

    if (argc > 0) {
        /* consume numbers from arguments, if any */
        while (argc --> 0) {
            human(*argv++, fac);
        }
    } else {
        /* read numbers from stdin if no args, one per line */
        while (fgets(in, LINE_MAX, stdin) != NULL) {
            /* overwrite the '\n' */
            in[strnlen(in, LINE_MAX) - 1] = 0;
            human(in, fac);
        }
    }

    return 0;
}
