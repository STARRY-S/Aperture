#include <stdio.h>
#include "test_main.h"
#include "func_test.h"

int main(int argc, char **argv)
{
    test_vector_char();

    test_vector_float();

    test_vector_uint();

    test_vector_int();

    LOGD("Debug msg");
    LOGI("Info msg");
    LOGW("Warn msg");
    LOGE("Error msg");

    test_ap_memory();

    return 0;
}