#include <stdio.h>
#include "common.h"
#include "encode.h"
#include "decode.h"

/*
Name: Pankaj Joshi 
Id: 25031_001

*/

//ALl the description is added in Readme.md file 





/*
gcc main.c encode.c decode.c -o stego.out
./stego.out -e beautiful.bmp secret.txt output.bmp
./stego.out -d output.bmp decoded

*/




/*
# Image Steganography using C
## Project Overview
Image Steganography is a technique used to hide secret information inside an image without changing 
the visible appearance of the image.
This project uses the Least Significant Bit (LSB) technique to hide text data inside a BMP image.
The program supports:
- Encoding secret text into a BMP image
- Decoding hidden text from a stego image
- Validation of input files
- Extension handling of secret file
- Magic string verification

*/

int main(int argc, char *argv[])
{
    EncodeInfo encodeInfo;
    DecodeInfo decodeInfo;

    if (argc == 1)
    {
        printf("Usage:\n");
        printf("./stego.out -e <source.bmp> <secret.txt> <output.bmp>\n");
        printf("./stego.out -d <stego.bmp> <output_file>\n");

        return 0;
    }

    int opr = check_operation(argv[1]);

    if (opr == encode)
    {

        if (validate_encode_args(argv, &encodeInfo) == failure)
        {
            printf("\nEncode validation failed\n");
            return 0;
        }

        if (do_encoding(argv, &encodeInfo) == success)
        {
            printf("\n=========================================\n");
            printf("Encoding Successfully Completed\n");
            printf("=========================================\n");
        }
        else
        {
            printf("Encoding Failed\n");
        }
    }

    else if (opr == decode)
    {
        if (argc < 3 || argc > 4) // if the output.bmp not passed
        {
            printf("Invalid decode arguments\n");
            printf("Use:\n");
            printf("./stego.out -d <stego.bmp> <output_file>\n");
            return 0;
        }

        if (validate_decode_args(argv, &decodeInfo) == failure)
        {
            printf("Decode validation failed\n");
            return 0;
        }

        if (do_decoding(&decodeInfo) == success)
        {
            printf("\n=========================================\n");
            printf("Decoding Successfully Completed\n");
            printf("=========================================\n");
        }
        else
        {
            printf("Decoding Failed\n");
        }
    }

    else
    {
        printf("Unsupported operation\n");
        printf("Use -e for encode or -d for decode\n");
    }

    return 0;
}