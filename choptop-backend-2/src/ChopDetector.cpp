#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ChopDetector.hpp"

#define N_FEATURES 4
#define N_CLASSES 2
#define N_VECTORS 1805
#define N_ROWS 2
#define N_COEFFICIENTS 1
#define N_INTERCEPTS 1
#define KERNEL_TYPE 'r'
#define KERNEL_GAMMA 0.2
#define KERNEL_COEF 0.0
#define KERNEL_DEGREE 3



int ChopDetector::predict(double features[]) {
    int i, j, k, d, l;

    double kernels[N_VECTORS];
    double kernel;
    switch (KERNEL_TYPE) {
        case 'l':
            // <x,x'>
            for (i = 0; i < N_VECTORS; i++) {
                kernel = 0.;
                for (j = 0; j < N_FEATURES; j++) {
                    kernel += vectors[i][j] * features[j];
                }
                kernels[i] = kernel;
            }
            break;
        case 'p':
            // (y<x,x'>+r)^d
            for (i = 0; i < N_VECTORS; i++) {
                kernel = 0.;
                for (j = 0; j < N_FEATURES; j++) {
                    kernel += vectors[i][j] * features[j];
                }
                kernels[i] = pow((KERNEL_GAMMA * kernel) + KERNEL_COEF, KERNEL_DEGREE);
            }
            break;
        case 'r':
            // exp(-y|x-x'|^2)
            for (i = 0; i < N_VECTORS; i++) {
                kernel = 0.;
                for (j = 0; j < N_FEATURES; j++) {
                    kernel += pow(vectors[i][j] - features[j], 2);
                }
                kernels[i] = exp(-KERNEL_GAMMA * kernel);
            }
            break;
        case 's':
            // tanh(y<x,x'>+r)
            for (i = 0; i < N_VECTORS; i++) {
                kernel = 0.;
                for (j = 0; j < N_FEATURES; j++) {
                    kernel += vectors[i][j] * features[j];
                }
                kernels[i] = tanh((KERNEL_GAMMA * kernel) + KERNEL_COEF);
            }
            break;
    }

    int starts[N_ROWS];
    int start;
    for (i = 0; i < N_ROWS; i++) {
        if (i != 0) {
            start = 0;
            for (j = 0; j < i; j++) {
                start += weights[j];
            }
            starts[i] = start;
        } else {
            starts[0] = 0;
        }
    }

    int ends[N_ROWS];
    for (i = 0; i < N_ROWS; i++) {
        ends[i] = weights[i] + starts[i];
    }

    if (N_CLASSES == 2) {

        for (i = 0; i < N_VECTORS; i++) {
            kernels[i] = -kernels[i];
        }

        double decision = 0.;
        for (k = starts[1]; k < ends[1]; k++) {
            decision += kernels[k] * coefficients[0][k];
        }
        for (k = starts[0]; k < ends[0]; k++) {
            decision += kernels[k] * coefficients[0][k];
        }
        decision += intercepts[0];

        if (decision > 0) {
            return 0;
        }
        return 1;

    }

    double decisions[N_INTERCEPTS];
    double tmp;
    for (i = 0, d = 0, l = N_ROWS; i < l; i++) {
        for (j = i + 1; j < l; j++) {
            tmp = 0.;
            for (k = starts[j]; k < ends[j]; k++) {
                tmp += kernels[k] * coefficients[i][k];
            }
            for (k = starts[i]; k < ends[i]; k++) {
                tmp += kernels[k] * coefficients[j - 1][k];
            }
            decisions[d] = tmp + intercepts[d];
            d = d + 1;
        }
    }

    int votes[N_INTERCEPTS];
    for (i = 0, d = 0, l = N_ROWS; i < l; i++) {
        for (j = i + 1; j < l; j++) {
            votes[d] = decisions[d] > 0 ? i : j;
            d = d + 1;
        }
    }

    int amounts[N_CLASSES];
    for (i = 0, l = N_CLASSES; i < l; i++) {
        amounts[i] = 0;
    }
    for (i = 0; i < N_INTERCEPTS; i++) {
        amounts[votes[i]] += 1;
    }

    int classVal = -1;
    int classIdx = -1;
    for (i = 0; i < N_CLASSES; i++) {
        if (amounts[i] > classVal) {
            classVal = amounts[i];
            classIdx= i;
        }
    }
    return classIdx;

}
