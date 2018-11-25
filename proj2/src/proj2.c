/*
 * Copyright (C) 2018  Roman Ondráček <xondra58@stud.fit.vutbr.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Exit codes
 */
enum exitStatuses {
    NO_ERROR,
    CONVERSION_ERROR,
};

/**
 * Converts string into double
 * @param string String to convert
 * @param number Converted double
 * @return Execution status
 */
int strToDouble(char *string, double *number) {
    char *endptr;
    *number = strtod(string, &endptr);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to double.\n");
        return CONVERSION_ERROR;
    }
    return NO_ERROR;
}

/**
 * Converts string into unsigned integer
 * @param string String to convert
 * @param number Converted unsigned integer
 * @return Execution status
 */
int strToUInt(char *string, unsigned int *number) {
    char *endptr;
    *number = (unsigned int) strtoul(string, &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to unsigned int.\n");
        return CONVERSION_ERROR;
    }
    return NO_ERROR;
}

/**
 * Calculates the value of the power function of Y with a base X
 * @param f Function for natural logarithm calculation
 * @param x Base of power function
 * @param y Value whose power function is calculated
 * @param n Count of iterations
 * @return Value of the power function of Y with a base X
 */
double calcExp(double (*f)(double, unsigned int), double x, double y, unsigned int n) {
    double fraction = 1;
    double sum = fraction;
    for (unsigned int i = 1; i < n; i++) {
        fraction *= ((*f)(x, n) * y) / i;
        sum += fraction;
    }
    return sum;
}

/**
 * Check if value is valid for natural logarithm calculation
 * @param x Value whose logarithm is calculated
 * @return Value whose logarithm is calculated
 */
double checkLog(double x) {
    if (x < 0) {
        return NAN;
    }
    if (x == 0) {
        return -INFINITY;
    }
    if (x == INFINITY) {
        return INFINITY;
    }
    return x;
}

/**
 * Calculates the natural logarithm via a continued fraction
 * @param x Value whose logarithm is calculated
 * @param n Count of iterations
 * @return Natural logarithm
 */
double cfrac_log(double x, unsigned int n) {
    double i = checkLog(x);
    if (isnan(i) || isinf(i)) {
        return i;
    }
    double cf = 0;
    double a, b;
    double z = (x - 1) / (x + 1);
    for (--n; n >= 1; n--) {
        a = (2 * n) + 1;
        b = n * z;
        b *= b;
        cf = b / (a - cf);
    }
    return ((2 * z) / (1 - cf));
}

/**
 * Calculates the natural logarithm via a Taylor series
 * @param x Value whose logarithm is calculated
 * @param n Count of iterations
 * @return Natural logarithm
 */
double taylor_log(double x, unsigned int n) {
    double z = checkLog(x);
    if (isnan(z) || isinf(z)) {
        return z;
    }
    double numerator = 1;
    double sum = 0;
    if (x < 1) {
        for (unsigned int i = 1; i <= n; i++) {
            numerator *= (1 - x);
            sum -= numerator / i;
        }
    } else {
        for (unsigned int i = 1; i <= n; i++) {
            numerator *= (x - 1) / x;
            sum += numerator / i;
        }
    }
    return sum;
}

/**
 * Calculates the value of the power function of Y with a base X
 * @param x Base of power function
 * @param y Value whose power function is calculated
 * @param n Count of iterations
 * @return Value of the power function of Y with a base X
 */
double taylor_pow(double x, double y, unsigned int n) {
    return calcExp(taylor_log, x, y, n);
}

/**
 * Calculates the value of the power function of Y with a base X
 * @param x Base of power function
 * @param y Value whose power function is calculated
 * @param n Count of iterations
 * @return Value of the power function of Y with a base X
 */
double taylorcf_pow(double x, double y, unsigned int n) {
    return calcExp(cfrac_log, x, y, n);
}

/**
 * Prints calculated the natural logarithm
 * @param x Value whose logarithm is calculated
 * @param n Count of iterations
 * @return Execution status
 */
int printLog(double x, unsigned int n) {
    printf("       log(%g) = %.12g\n", x, log(x));
    printf(" cfrac_log(%g) = %.12g\n", x, cfrac_log(x, n));
    printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
    return NO_ERROR;
}

/**
 * Prints calculated the value of the power function of Y with a base X
 * @param x Base
 * @param y Value whose power function is calculated
 * @param n Count of iterations
 * @return Execution status
 */
int printPow(double x, double y, unsigned int n) {
    printf("         pow(%g,%g) = %.12g\n", x, y, pow(x, y));
    printf("  taylor_pow(%g,%g) = %.12g\n", x, y, taylor_pow(x, y, n));
    printf("taylorcf_pow(%g,%g) = %.12g\n", x, y, taylorcf_pow(x, y, n));
    return NO_ERROR;
}

/**
 * Prints program's usage
 * @return Execution status
 */
int printUsage() {
    puts("Usage: ./proj2 [options] [arguments]");
    puts("Options:");
    puts("\t--log X N\t\tCalculates the natural logarithm of X with N iterations");
    puts("\t--pow X Y N\t\tCalculates the value of the power function of Y with a base X with N iterations");
    puts("\t-h, --help\t\tPrints help (this message) and exits");
    return NO_ERROR;
}

/**
 * Main program function
 * @param argc Count of arguments
 * @param argv Program's arguments
 * @return Execution status
 */
int main(int argc, char *argv[]) {
    double x, y;
    unsigned int n;
    if (argc == 4 && (strcmp(argv[1], "--log") == 0)) {
        strToDouble(argv[2], &x);
        strToUInt(argv[3], &n);
        return printLog(x, n);
    } else if (argc == 5 && (strcmp(argv[1], "--pow") == 0)) {
        strToDouble(argv[2], &x);
        strToDouble(argv[3], &y);
        strToUInt(argv[4], &n);
        return printPow(x, y, n);
    }
    return printUsage();
}
