#include "stdio.h"

#include "opus/base/base_inc.h"
#include "opus/base/base_inc.c"

typedef struct
{
    int32 x;
} TestStruct;

int
main()
{
    Arena* persistent_arena = arena_new();

    TestStruct* test_struct = arena_push_struct(persistent_arena, TestStruct);
    test_struct->x          = 5;
    int32 t                 = 12;
    printf("test %d", test_struct->x);
}