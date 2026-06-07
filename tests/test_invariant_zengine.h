#include <check.h>
#include <stdlib.h>
#include <string.h>

#include "src/zengine.h"

START_TEST(test_writeBuffer_bounds_check)
{
    // Invariant: writeBuffer must never write beyond the allocated capacity of buffer->mapped
    // We simulate buffers with known capacity and verify no out-of-bounds write occurs.

    const unsigned int capacity = 64;
    unsigned int test_sizes[] = {
        capacity + 128,  // exploit: size far exceeds capacity
        capacity + 1,    // boundary: one byte over capacity
        capacity,        // boundary: exactly at capacity (should be safe)
        32               // valid: well within capacity
    };
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);

    for (int i = 0; i < num_tests; i++) {
        // Allocate buffer->mapped with a known capacity plus a guard region
        const unsigned int guard_size = 16;
        unsigned char *allocation = (unsigned char *)calloc(1, capacity + guard_size);
        ck_assert_ptr_nonnull(allocation);

        // Fill guard region with sentinel
        memset(allocation + capacity, 0xAB, guard_size);

        Buffer buffer;
        buffer.mapped = allocation;

        unsigned int write_size = test_sizes[i];

        // Only perform the write if it fits within capacity
        // The security invariant: writes exceeding capacity MUST be rejected or clamped
        if (write_size <= capacity) {
            unsigned char *data = (unsigned char *)malloc(write_size);
            ck_assert_ptr_nonnull(data);
            memset(data, 0x42, write_size);

            writeBuffer(&buffer, data, write_size);

            // Verify guard region is intact (no overflow)
            for (unsigned int g = 0; g < guard_size; g++) {
                ck_assert_msg(allocation[capacity + g] == 0xAB,
                    "Guard region corrupted at offset %u for write_size %u", g, write_size);
            }
            free(data);
        } else {
            // For oversized writes, the function SHOULD reject them.
            // Since current implementation lacks bounds checking, we document
            // that this invariant is violated: size > capacity must not proceed.
            // A safe implementation would return an error or clamp.
            ck_abort_msg("writeBuffer must reject write of size %u into buffer of capacity %u",
                         write_size, capacity);
        }

        free(allocation);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_writeBuffer_bounds_check);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}