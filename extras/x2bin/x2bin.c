// Convert an X68000 relocatable executable file to absolute address binary blob
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *in;
    FILE *out;
    char *end = NULL;
    int in_size;
    int i;
    unsigned char *in_mem;
    unsigned char *bss_mem;
    unsigned char *code_data_mem;
    unsigned char *reloc_mem;
    unsigned int base_address;
    int reloc_data;
    int reloc_offset;
    unsigned int reloc_orig;
    int code_size;
    int data_size;
    int bss_size;
    int reloc_size;
    int symbol_size;

    if(argc == 4)
    {
        base_address = strtoul(argv[1], &end, 16);
        if(argv[1] == end)
        {
            fprintf(stderr, "error: wrong relocation address\n");
            return EXIT_FAILURE;
        }
        
        in = fopen(argv[2], "rb");
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
                    out = fopen(argv[3], "wb");
                    if(out)
                    {
                        code_size = (in_mem[12] << 24) | (in_mem[12 + 1] << 16) | (in_mem[12 + 2] << 8) | (in_mem[12 + 3]);
                        data_size = (in_mem[16] << 24) | (in_mem[16 + 1] << 16) | (in_mem[16 + 2] << 8) | (in_mem[16 + 3]);
                        bss_size = (in_mem[20] << 24) | (in_mem[20 + 1] << 16) | (in_mem[20 + 2] << 8) | (in_mem[20 + 3]);
                        reloc_size = (in_mem[24] << 24) | (in_mem[24 + 1] << 16) | (in_mem[24 + 2] << 8) | (in_mem[24 + 3]);
                        ((int *) in_mem)[7] = 0;
                        ((int *) in_mem)[8] = 0;
                        ((int *) in_mem)[9] = 0;
                        
                        code_data_mem = &in_mem[64];
                        reloc_mem = &in_mem[64 + code_size + data_size];
                        i = 0;
                        reloc_offset = 0;
                        while(i < reloc_size)
                        {
                            reloc_data = (reloc_mem[i] << 8) | (reloc_mem[i + 1]);
                            if(reloc_data & 1)
                            {
                                i += 2;
                                reloc_data = (reloc_mem[i] << 24) | (reloc_mem[i + 1] << 16) | (reloc_mem[i + 2] << 8) | (reloc_mem[i + 3]);
                                i += 4;
                            }
                            reloc_offset += reloc_data;
                            reloc_orig = (code_data_mem[reloc_offset] << 24) | (code_data_mem[reloc_offset + 1] << 16) |
                                         (code_data_mem[reloc_offset + 2] << 8) | (code_data_mem[reloc_offset + 3]);
                            reloc_orig += base_address;
                            code_data_mem[reloc_offset] = (reloc_orig >> 24) & 0xff;
                            code_data_mem[reloc_offset + 1] = (reloc_orig >> 16) & 0xff;
                            code_data_mem[reloc_offset + 2] = (reloc_orig >> 8) & 0xff;
                            code_data_mem[reloc_offset + 3] = reloc_orig & 0xff;
                            i += 2;
                        }
                        
                        // Write code section
                        fwrite(&in_mem[64], 1, code_size, out);
                        // Write data section
                        fwrite(&in_mem[64 + code_size], 1, data_size, out);
                        bss_mem = malloc(bss_size);
                        if(bss_mem)
                        {
                            memset(bss_mem, 0, bss_size);
                            fwrite(bss_mem, 1, bss_size, out);
                            fclose(out);
                            printf("relocated %s into %s at 0x%x\n", argv[2], argv[3], base_address);
                        }
                        else
                        {
                            fclose(out);
                            fprintf(stderr, "error: not enough memory\n");
                            return EXIT_FAILURE;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "error: can't open output file\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    fprintf(stderr, "error: wrong file format\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                fclose(in);
                fprintf(stderr, "error: not enough memory\n");
                return EXIT_FAILURE;
            }
        }
        else
        {
            fprintf(stderr, "error: can't open input file\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("usage: x2raw <relocation address in hex> <infile.x> <outfile.raw>\n");
    }
    return EXIT_SUCCESS;
}
