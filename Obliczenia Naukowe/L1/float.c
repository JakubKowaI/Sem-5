#include <stdio.h>
#include <float.h>

int main(void) {
    printf("=== FLOAT ===\n");
    printf("FLT_MAX      = %.10e\n", FLT_MAX);
    printf("FLT_MIN      = %.10e\n", FLT_MIN);
    printf("FLT_EPSILON  = %.10e\n", FLT_EPSILON);

    printf("=== DOUBLE ===\n");
    printf("DBL_MAX      = %.20e\n", DBL_MAX);
    printf("DBL_MIN      = %.20e\n", DBL_MIN);
    printf("DBL_EPSILON  = %.20e\n", DBL_EPSILON);

    printf("=== LONG DOUBLE ===\n");
    printf("LDBL_MAX      = %.30Le\n", LDBL_MAX);
    printf("LDBL_MIN      = %.30Le\n", LDBL_MIN);
    printf("LDBL_EPSILON  = %.30Le\n", LDBL_EPSILON);

    return 0;
}
