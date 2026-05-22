#include <stdio.h>
#include <string.h>
#include "common.h"
#include "decode.h"

//______________________________________________________________________validation part________________________________________

Status validate_decode_args(char *argv[], DecodeInfo *decInfo)
{

    if (argv[2] != NULL) // Checking  stego image pass
    {
        char *ext = strrchr(argv[2], '.');           // getting ext
        if (ext != NULL && strcmp(ext, ".bmp") == 0) // Checking  extension is .bmp or not
        {
            decInfo->stego_image_fname = argv[2];
            printf("stego image : %s\n", decInfo->stego_image_fname);
        }
        else
        {
            printf("____________________________\n");
            printf("output image must be .bmp\n");
            printf("____________________________\n");
            return failure;
        }
    }
    else
    {
        printf("\n______________________\n");
        printf("output.bmp image missing\n");
        printf("________________________\n");
        return failure;
    }

    if (argv[3] != NULL) // Validate output decoded file
    {
        strcpy(decInfo->secret_fname, argv[3]);
        printf("decoded file : %s\n", decInfo->secret_fname);
    }
    else
    {
        printf("Decoded output file not passed\n");
        printf("Creating default file : decoded\n");
        strcpy(decInfo->secret_fname, "decoded");
    }

    return success;
}
//============================================= Main function from here we are calling all nesesory functions =====================

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == failure) // opning file if failed then stop
        return failure;
    fseek(decInfo->stego_image_fptr, 54, SEEK_SET); // move pointer  54 position for skiping header
    if (decode_magic_string(decInfo) == failure)    // if magic string matching failed again stop
    {
        printf("Magic string mismatch\n");
        return failure;
    }

    decode_secret_file_extn(decInfo); // calling secrete file ext
    decode_secret_file_data(decInfo); //  decoding secrete data

    fclose(decInfo->stego_image_fptr); // complite all close all files
    fclose(decInfo->secret_fptr);

    return success;
}

//============================================= End main  ==========================================================
//============================================= All functions Start ===========================================================

//______________________________________________________________________Opning File____________________________________________

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->stego_image_fptr = fopen(decInfo->stego_image_fname, "rb"); // opning file in binary read mode
    if (decInfo->stego_image_fptr == NULL)                               //  failed to open show error
    {
        perror("fopen");
        fprintf(stderr, "Unable to open file %s\n", decInfo->stego_image_fname);
        return failure;
    }
    return success; // files opend
}

//______________________________________________________________________Decode Magic String____________________________________

Status decode_magic_string(DecodeInfo *decInfo)
{
    char buffer[8]; // 8 byte
    char magic[3];  // new array for string magic string
    for (int i = 0; i < 2; i++)
    {
        fread(buffer, 8, 1, decInfo->stego_image_fptr); // decoding magic string
        magic[i] = decode_1byte_from_lsb(buffer);
    }
    magic[2] = '\0';                      // stroring null char in array
    if (strcmp(magic, MAGIC_STRING) == 0) // comparing decoded magic string with orignal magic string
    {
        printf("magic string matched\n");
        return success;
    }
    return failure;
}

//______________________________________________________________________Decode secret extension_______________________________

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[32];                                           // buffer for  4*8 =32 for decoding secrete file extension
    fread(buffer, 32, 1, decInfo->stego_image_fptr);           // read 32 bit
    decInfo->secret_extn_size = decode_4byte_from_lsb(buffer); // decodig 4byte  storing it
    for (int i = 0; i < decInfo->secret_extn_size; i++)        // running loop by ext size time
    {
        char temp[8];
        fread(temp, 8, 1, decInfo->stego_image_fptr);
        decInfo->secret_extn[i] = decode_1byte_from_lsb(temp); // extra aray for storing ext  e.g. (.txt)  file will becomes decoded.txt
    }
    decInfo->secret_extn[decInfo->secret_extn_size] = '\0';    // at end storing '\0' char
    strcat(decInfo->secret_fname, decInfo->secret_extn);
    printf("decoded extension : %s\n", decInfo->secret_extn);  // printing decode file ext  .txt
    return success;
}

//______________________________________________________________________Decode Secret file data_______________________________

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char h;
    decInfo->secret_fptr = fopen(decInfo->secret_fname, "w"); // opning decode.txt in wrte mode
    if (decInfo->secret_fptr == NULL)                         // if not open then failure
        return failure;
//_______________________________________________________________________________________________
                    //Extra part added in project for exact secrete file size 
//_______________________________________________________________________________________________
    FILE *fp;                                               
    fp = fopen("size.txt", "r");                             // opning the secrete file size file 
    fscanf(fp, "%lu", &decInfo->secret_file_size);           // in the file storing size 
    fclose(fp);
//_______________________________________________________________________________________________
    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
        fread(buffer, 8, 1, decInfo->stego_image_fptr); // reading the secrete data from stego
        h = decode_1byte_from_lsb(buffer);
        fwrite(&h, 1, 1, decInfo->secret_fptr);         // writing secrete data in decoded.txt
    }
    printf("decoded secret file success\n");            // printing msg on stdout
    return success;
}

//______________________________________________________________________Decode 1 byte from LSB_______________________________

char decode_1byte_from_lsb(char *buffer_8)
{
    char c = 0;
    for (int i = 0; i < 8; i++)           // run loop 8 times because one character = 8 bits
        c = (c << 1) | (buffer_8[i] & 1); // Shifting previous bits to left by 1 position and  Extract LSB
    return c;
}

//______________________________________________________________________Decode 4 byte from LSB_______________________________

int decode_4byte_from_lsb(char *buffer_32)
{
    int c = 0;                             // Store decoded int value
    for (int i = 0; i < 32; i++)           // 4 byte means 4*8 32 times
        c = (c << 1) | (buffer_32[i] & 1); // extract lsb from image
    return c;
}