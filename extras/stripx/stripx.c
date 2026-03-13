// Remove any debug informations from an X68000 relocatable executable file.
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *in;
    FILE *out;
    int in_size;
    unsigned char *in_mem;
    int code_size;
    int data_size;
    int bss_size;
    int reloc_size;
    int symbol_size;

    if(argc == 3 || argc == 2)
    {
        in = fopen(argv[1], "rb");
        if(in)
        {
            fseek(in, 0, SEEK_END);
            in_size = ftell(in);
            fseek(in, 0, SEEK_SET);
            in_mem = malloc(in_size);
            if(in_mem)
            {
                fread(in_mem, 1, in_size, in);
                fclose(in);
                if(in_mem[0] == 'H' && in_mem[1] == 'U')
                {
                    if(argc == 2)
                    {
                        out = fopen(argv[1], "wb");
                    }
                    else
                    {
                        out = fopen(argv[2], "wb");
                    }
                    if(out)
                    {
                        code_size = (in_mem[12] << 24) | (in_mem[12 + 1] << 16) | (in_mem[12 + 2] << 8) | (in_mem[12 + 3]);
                        data_size = (in_mem[16] << 24) | (in_mem[16 + 1] << 16) | (in_mem[16 + 2] << 8) | (in_mem[16 + 3]);
                        bss_size = (in_mem[20] << 24) | (in_mem[20 + 1] << 16) | (in_mem[20 + 2] << 8) | (in_mem[20 + 3]);
                        reloc_size = (in_mem[24] << 24) | (in_mem[24 + 1] << 16) | (in_mem[24 + 2] << 8) | (in_mem[24 + 3]);
                        ((int *) in_mem)[7] = 0;
                        ((int *) in_mem)[8] = 0;
                        ((int *) in_mem)[9] = 0;
                        fwrite(in_mem, 1, 64, out);
                        fwrite(&in_mem[64], 1, code_size, out);
                        fwrite(&in_mem[64 + code_size], 1, data_size, out);
                        fwrite(&in_mem[64 + code_size + data_size], 1, reloc_size, out);
                        fclose(out);
                    }
                    else
                    {
                        fprintf(stderr, "Error: can't open output file\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: wrong file format\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                fclose(in);
                fprintf(stderr, "Error: not enough memory\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "Error: can't open input file\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("usage: stripx <infile.x> [outfile.x]\n");
    }
    return EXIT_SUCCESS;
}
