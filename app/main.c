#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "funcs.h"

#ifdef BLOB_BUILD
void _start(ProgramArgs *args)
{
  arguments = *args;
    clock_t start_time, end_time;
   start_time = clock();
    printf("Arg1 ", arguments.name_of_file_text);
    printf("Arg1 ", arguments.text);
    printf("Arg1 ", arguments.name_of_output_file);
    main_logic(arguments.name_of_file_text, arguments.text, arguments.name_of_output_file);
    end_time = clock();
    print_line_double("Total time: ", (double)(end_time-start_time));
}
#else
int main(int argc, char* argv[])
{
    clock_t start_time, end_time;
    start_time = clock();
    if(argc != 4)
    {
        printf("Incorrect argument num\n");
        return -1;
    }
    
    main_logic(argv[1], argv[2], argv[3]);
    end_time = clock();
    printf("Total time: %f\n", (double)(end_time - start_time));
}
#endif