// Convert an X68000 relocatable executable file to absolute address binary blob
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CODE_SIZE 0
#define DATA_SIZE 1
#define BSS_SIZE 2
#define RELOC_SIZE 3

//#define BIG_ENDIAN 

struct x68_x_header
{
    unsigned short magic;           // 0x4855
    unsigned char reserved1;        // 0
    unsigned char loadmode;         // 0 = normal
                                    // 1 = minimal memory
                                    // 2 = high address
    unsigned int base;              // base address (0)
    unsigned int entrypoint;
    unsigned int size[5];           // code, data, bss, reloc, symbols
	unsigned int db_line;	        // size of debugging info (line #)
	unsigned int db_syms;	        // size of debugging info (symbol)
	unsigned int db_str;	        // size of debugging info (string)
	unsigned int reserved2[4];	    // 0
	unsigned int bindlist;	        // bind list offset
} __attribute__((packed));          // 64/0x40 bytes

unsigned int get_dword(unsigned int data)
{
#if defined(BIG_ENDIAN)
    return data;
#else
    return (((data & 0xff000000) >> 24) | ((data & 0xff0000) >> 8) | ((data & 0xff00) << 8) | ((data & 0xff) << 24));
#endif
}

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
    struct x68_x_header *header;
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
            fprintf(stderr, "Error: wrong relocation address\n");
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
                    header = (struct x68_x_header *) in_mem;
                    printf("\n  Code size: 0x%x\n", get_dword(header->size[CODE_SIZE]));
                    printf("  Data size: 0x%x\n", get_dword(header->size[DATA_SIZE]));
                    printf("   Bss size: 0x%x\n", get_dword(header->size[BSS_SIZE]));
                    printf(" Reloc size: 0x%x\n", get_dword(header->size[RELOC_SIZE]));
                    printf("Entry point: 0x%x\n\n", get_dword(header->entrypoint));
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
                            printf("Relocated %s into %s at 0x%x\n", argv[2], argv[3], base_address);
                        }
                        else
                        {
                            fclose(out);
                            fprintf(stderr, "Error: not enough memory\n");
                            return EXIT_FAILURE;
                        }
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
        printf("usage: x2raw <relocation address in hex> <infile.x> <outfile.raw>\n");
    }
    return EXIT_SUCCESS;
}
