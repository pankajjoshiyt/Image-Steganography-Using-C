#include <stdio.h>
#include "common.h"
#include "encode.h"

//______________________________________________________________________check Operation________________________________________
Opr_type check_operation(char *option)
{
    if (strcmp(option, "-e") == 0) // passing arguments is  -e for encoding
    {
        printf("=========================================\n            Start Encoding\n=========================================\n");
        return encode;
    }
    else if (strcmp(option, "-d") == 0) // passing argument  is - d for decoding
    {
        printf("=========================================\n Start Decoding\n=========================================\n");
        return decode;
    }
    else
    {
        return unsupported; // meance  in passing argument is nighter -e nor -d
    }
}
//______________________________________________________________________validation part________________________________________
Status validate_encode_args(char *argv[], EncodeInfo *encInfo)
{

    // Validate source image file
    if (argv[2] != NULL) // Checking source file argument pass
    {
        char *ext = strrchr(argv[2], '.');           // getting ext of soure file .bmp
        if (ext != NULL && strcmp(ext, ".bmp") == 0) // Checking extension is .bmp or not
        {
            encInfo->src_image_fname = argv[2];
            printf("source file : %s\n", encInfo->src_image_fname);
        }
        else
        {
            printf("\n___________________________\n");
            printf("Source file must be .bmp\n");
            printf("_____________________________\n");
            return failure;
        }
    }
    else
    {
        // Source file not passed
        printf("\n____________________\n");
        printf("Source file missing\n");
        printf("____________________\n");
        printf("Use:\n");
        printf("./stego.out -e <source.bmp> <secret.txt> <output.bmp>\n");

        return failure;
    }
    if (argv[3] != NULL)
    {
        encInfo->secret_fname = argv[3];
        char *ext = strrchr(argv[3], '.'); // storing the extension in pointer
        if (ext != NULL)
        {
            strcpy(encInfo->secret_extn, ext);                        // Copy extension (.txt)
            encInfo->secret_extn_size = strlen(encInfo->secret_extn); // Get extension size
        }
        else
        {
            printf("__________________________________\n");
            printf("Secret file must have extension\n"); // priting error
            printf("__________________________________\n");

            return failure;
        }
    }
    else
    {
        printf("__________________________________\n");
        printf("secrete file is missing \n"); // printing error file missing
        printf("__________________________________\n");
        return failure;
    }

    if (argv[4] != NULL) // Check output file argument passed
    {
        char *ext = strrchr(argv[4], '.');           // Find extension from output filename
        if (ext != NULL && strcmp(ext, ".bmp") == 0) // Checking extension is .bmp or not
        {
            encInfo->output_image_fname = argv[4];
            printf("output file : %s\n", encInfo->output_image_fname); // created output.bmp
        }
        else
        {
            printf("__________________________________\n");
            printf("Output file must be .bmp\n"); // printing the error
            printf("__________________________________\n");
            return failure;
        }
    }
    else
    {
        printf("Output file not passed\n"); // If output file not passed
        printf("Creating default stego.bmp\n");
        encInfo->output_image_fname = "stego.bmp";
    }
    return success;
}

// ============================================ Main function where we are calling all nesesory functions =====================

Status do_encoding(char *argv[], EncodeInfo *encInfo)
{
    if (open_files(encInfo) == failure) // opning files if faield then stop
        return failure;
    if (check_capacity(encInfo) == failure) //  checking capcity faield  then stop
        return failure;

    // calling one by functionss
    copy_bmp_header(encInfo);                   //  coping header
    encode_magic_string(MAGIC_STRING, encInfo); //  encoing magic string
    encode_secret_file_extn(encInfo);           //  ecoding secrete file extension
    encode_secret_file_data(encInfo);           //  ecoding  secrete file data
    copy_remaining_img_data(encInfo);           //  coping all remanig data

    // closing the  all opend files                 end all things we got the output.BMP file
    fclose(encInfo->src_image_fptr);
    fclose(encInfo->secret_fptr);
    fclose(encInfo->output_image_fptr);
    // after all we are returning the succes to main function for showing encoding done msg
    return success;
}
//============================================= End the main calling  =========================================================

//============================================= All functions Start ===========================================================
//______________________________________________________________________Opning File____________________________________________

Status open_files(EncodeInfo *encInfo)
{
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "rb"); // opning source  file in rb read binary mode
    if (encInfo->src_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Unable to open file %s\n", encInfo->src_image_fname);
        return failure;
    }
    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");        // opning secrete  file in rb read binary mode
    if (encInfo->secret_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Unable to open file %s\n", encInfo->secret_fname);
        return failure;
    }
    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "wb"); //opning output file in Wb write binary mode
    if (encInfo->output_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Unable to open file %s\n", encInfo->output_image_fname);
        return failure;
    }
    return success;
}

//______________________________________________________________________Checking Capacity______________________________________
Status check_capacity(EncodeInfo *encInfo)
{
    unsigned int image_capacity;
    unsigned int secret_size;
    image_capacity = get_image_size_for_bmp(encInfo->src_image_fptr);
    secret_size = get_file_size(encInfo->secret_fptr);
    printf("size of source file is %u\n", image_capacity);
    encInfo->secret_file_size = secret_size; // Storing the secret file size
    if (image_capacity > (54 + (secret_size * 8)))
    {
        printf("check capacity success\n");
        return success;
    }
    else
        return failure;
}

//______________________________________________________________________Get Size for beautiful.bmp File________________________
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    fseek(fptr_image, 18, SEEK_SET);           // Seek to 18th byte move curser to 18 posistion
    fread(&width, sizeof(int), 1, fptr_image); // Reading  the width int
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image); // Reading  the height int
    printf("height = %u\n", height);
    printf("=========================================\n\n");
    return width * height * 3; // Return image capacity
}

//______________________________________________________________________Geting size for Secret.txt file________________________
unsigned int get_file_size(FILE *fptr)
{
    unsigned int size;        // variable for stori g size
    fseek(fptr, 0, SEEK_END); // moving the pointer to 0th index
    size = ftell(fptr);       // EOF till return
    rewind(fptr);             //  reback to 0th index
    printf("size of secret file is %u\n", size);


//_______________________________________________________________________________________________
                    //Extra part added in project for exact secrete file size 
//_______________________________________________________________________________________________
    FILE *fp;
    fp = fopen("size.txt", "w");//creating a new file that store the secrete.txt file size for decoding  
    fprintf(fp, "%u", size);    // simple way to add reuse the file size 
    fclose(fp);
//_______________________________________________________________________________________________

    return size;
}

//______________________________________________________________________Copy Header from the file______________________________

Status copy_bmp_header(EncodeInfo *encInfo)
{
    char header[54];                                   // veriable declare for the 54 byte header
    rewind(encInfo->src_image_fptr);                   // Move to beginning of source image
    fread(header, 54, 1, encInfo->src_image_fptr);     // stroring the 54 bytes in the header
    fwrite(header, 54, 1, encInfo->output_image_fptr); // copy or write the header content in output file
    printf("coping .bmp Header succesfully\n");
    return success;
}

//______________________________________________________________________Storing magic Sting  #*________________________________
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8]; // used to store 8 byte from image
    int i;
    for (i = 0; magic_string[i] != '\0'; i++) // running loop 2 times
    {
        fread(buffer, 8, 1, encInfo->src_image_fptr);     // Reading  8 bytes from source image
        encode_1byte_to_lsb(magic_string[i], buffer);     // Encoding one byte into 8 bytes
        fwrite(buffer, 8, 1, encInfo->output_image_fptr); // Writing  encoded data to output image
    }
    printf("storing magic string success\n");
    return success; // return
}

//______________________________________________________________________Encode secrete file  ext_______________________________
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    char buffer[8];
    int i;
    char size_buffer[32];
    fread(size_buffer, 32, 1, encInfo->src_image_fptr); // Encode extension size first
    encode_4byte_to_lsb(encInfo->secret_extn_size, size_buffer);
    fwrite(size_buffer, 32, 1, encInfo->output_image_fptr);
    for (i = 0; encInfo->secret_extn[i] != '\0'; i++) // Encode extension characters .txt
    {
        fread(buffer, 8, 1, encInfo->src_image_fptr);         // Read 8 bytes from source image
        encode_1byte_to_lsb(encInfo->secret_extn[i], buffer); // Encode 1 character
        fwrite(buffer, 8, 1, encInfo->output_image_fptr);     // Write encoded data
    }
    printf("encode the secrete file success\n");
    return success;
}

//______________________________________________________________________Encode  Secret file data_______________________________
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];
    rewind(encInfo->secret_fptr); // Move secret file pointer to 0 inde
    // fseek(encInfo->secret_fptr, 0, SEEK_SET);          //   also we can use fseek funtion here as rewind  for back to 0 inddex
    while (fread(&ch, 1, 1, encInfo->secret_fptr) > 0)
    {
        fread(buffer, 8, 1, encInfo->src_image_fptr);     // Read 8 bytes from source image
        encode_1byte_to_lsb(ch, buffer);                  // Encode one character
        fwrite(buffer, 8, 1, encInfo->output_image_fptr); // Write encoded data
    }
    printf("encoded secret file data succes\n");
    return success;
}

//______________________________________________________________________encode 1 byte int LSB__________________________________
Status encode_1byte_to_lsb(char p, char *buffer_8)
{
    for (int i = 0; i < 8; i++) // running loop 8 times
    {
        buffer_8[i] = (buffer_8[i] & 0xFE) | ((p >> (7 - i)) & 1); //  0xFE in binary 1111 1110  & the  clear the LSB bit
    }
    return success;
}

//______________________________________________________________________encode 4 byte form LSB_________________________________
Status encode_4byte_to_lsb(int p, char *buffer_32)
{
    for (int i = 0; i < 32; i++)                                      // running loop 32 times for 4 byte  4*8=32
        buffer_32[i] = (buffer_32[i] & 0xFE) | ((p >> (31 - i)) & 1); // same as clear LSB
    return success;
}

//______________________________________________________________________Coping  remanig data___________________________________
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char c;
    while (fread(&c, 1, 1, encInfo->src_image_fptr) > 0) // coping the remanig bytes reading the all other byte
        fwrite(&c, 1, 1, encInfo->output_image_fptr);    // writing the other bytes
    return success;
}

// encoding cuccesfully