#include "stdio.h"

#include "opus/base/base_inc.h"
#include "opus/base/base_inc.c"

#include "opus/os/os_inc.h"
#include "opus/os/os_inc.c"

int32
main()
{
    ThreadContext tctx;
    tctx_init_and_equip(&tctx);

    Arena* persistent_arena = arena_new();
    String file_path        = string("C:\\.ignore\\test.txt");
    if (os_file_exists_at_path(file_path))
    {
        OS_Handle file     = os_file_open(file_path, OS_AccessFlag_Read);
        String    contents = os_file_read_all_as_string(file, persistent_arena);
        log_info("file contents: %s", contents.value);

        String working_directory = os_file_get_working_directory(persistent_arena);
        printf("working_directory: %s\n", working_directory.value);
    }
    else
    {
        log_info("file doesnt exist");
    }
}