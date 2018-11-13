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
 * @param integer Integer
 * @return Execution status
 */
int strToDouble(char *string, double *integer) {
    char *endptr;
    *integer = strtod(string, &endptr);
    if (*endptr != '\0') {
        fprintf(stderr, "Error in string conversion to double.");
        return CONVERSION_ERROR;
    }
    return NO_ERROR;
}

/**
 * Calculates the natural logarithm via a continued fraction
 * @param x Value whose logarithm is calculated
 * @param n Count of iterations
 * @return Natural logarithm
 */
double cfrac_log(double x, unsigned int n) {
    if (x < 0) {
        return NAN;
    }
    if (x == 0) {
        return -INFINITY;
    }
    double cf = 0;
    double a, b;
    double z = (x - 1) / (x + 1);
    for (; n >= 1; n--) {
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
    if (x < 0) {
        return NAN;
    }
    if (x == 0) {
        return -INFINITY;
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
 * Calculates the value of the exponential function of Y with a base X
 * @param x Base of exponential function
 * @param y Value whose exponential function is calculated
 * @param n Count of iterations
 * @return Value of the exponential function of Y with a base X
 */
double taylor_pow(double x, double y, unsigned int n) {
    double numerator = 1;
    double sum = numerator;
    for (unsigned int i = 1; i <= n; i++) {
        numerator *= taylor_log(x, n) * y / i;
        sum += numerator;
    }
    return sum;
}

/**
 * Calculates the value of the exponential function of Y with a base X
 * @param x Base of exponential function
 * @param y Value whose exponential function is calculated
 * @param n Count of iterations
 * @return Value of the exponential function of Y with a base X
 */
double taylorcf_pow(double x, double y, unsigned int n) {
    double numerator = 1;
    double sum = numerator;
    for (unsigned int i = 1; i <= n; i++) {
        numerator *= cfrac_log(x, n) * y / i;
        sum += numerator;
    }
    return sum;
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
 * Prints calculated the value of the exponential function of Y with a base X
 * @param x Base
 * @param y Value whose exponential function is calculated
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
    puts("\t--pow X Y N\t\tCalculates the value of the exponential function of Y with a base X with N iterations");
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
    if (argc == 4 && (strcmp(argv[1], "--log") == 0)) {
        double x, n;
        strToDouble(argv[2], &x);
        strToDouble(argv[3], &n);
        return printLog(x, n);
    } else if (argc == 5 && (strcmp(argv[1], "--pow") == 0)) {
        double x, y, n;
        strToDouble(argv[2], &x);
        strToDouble(argv[3], &y);
        strToDouble(argv[4], &n);
        return printPow(x, y, n);
    }
    return printUsage();
}