#include <stdio.h>
#include "test_main.h"
#include "func_test.h"

int main(int argc, char **argv)
{
    test_vector_char();

    test_vector_float();

    test_vector_uint();

    test_vector_int();

    return 0;
}