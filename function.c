#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "common.h"

// -----------------------------------------------------------------------------------  Opning File   --------------------------------------------------------

Status open_files(EncodeInfo *encInfo)
{
    encInfo->src_image_fptr = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->src_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return failure;
    }
    encInfo->secret_fptr = fopen(encInfo->secret_fname, "r");
    if (encInfo->secret_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return failure;
    }
    encInfo->output_image_fptr = fopen(encInfo->output_image_fname, "wb");
    if (encInfo->output_image_fptr == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->output_image_fname);
        return failure;
    }
    return success;
}

//--------------------------------------------------------------------------------   Checking Capacity   ---------------------------------------------------------------------
/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    unsigned int image_capacity;
    unsigned int secret_size;

    image_capacity = get_image_size_for_bmp(encInfo->src_image_fptr);
    secret_size = get_file_size(encInfo->secret_fptr);

    // Stor ing the secret file size
    encInfo->secret_file_size = secret_size;

    /*
       54 -> BMP header
       each byte requires 8 bytes of secrete file
    */

    if (image_capacity > (54 + (secret_size * 8)))
    {
        printf(" check capacity success");
        return success;
    }
    else
    {
        return failure;
    }
}

// --------------------------------------------------------------------------------  Get Size for beautiful.bmp File  ----------------------------------------------------
unsigned int get_image_size_for_bmp(FILE *fptr_image)
{
    unsigned int width, height;
    fseek(fptr_image, 18, SEEK_SET);           // Seek to 18th byte move curser to 18 posistion
    fread(&width, sizeof(int), 1, fptr_image); // Reading  the width int
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image); // Reading  the height int
    printf("height = %u\n", height);
    return width * height * 3; // Return image capacity
}
//----------------------------------------------------------------------------------- Geting size for Secret.txt file ---------------------------------------------------------------
/* Get secret file size */
unsigned int get_file_size(FILE *fptr)
{
    unsigned int size;
    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
    rewind(fptr);

    printf(" size of file is %u\n", size);
    return size;
}

// ------------------------------------------------------------------------------------ Copy Header from the file -----------------------------------------------------------

/* Copy bmp image header */
Status copy_bmp_header(EncodeInfo *encInfo)
{
    char header[54]; // veriable declare for the 54 byte header
    // Move to beginning of source image
    rewind(encInfo->src_image_fptr);
    // Read 54 bytes from src image
    fread(header, 54, 1, encInfo->src_image_fptr); // stroring the 54 bytes in the header
    // Write 54 bytes into output image
    fwrite(header, 54, 1, encInfo->output_image_fptr); // copy the header content in output file
    printf(" coping .bmp Header succesfully\n");
    return success;
}

//----------------------------------------------------------- -----------------------------Storing magic Sting  #*  -----------------------------------------------------------------------
/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char buffer[8];
    int i;
    for (i = 0; magic_string[i] != '\0'; i++) // running loop 2 times
    {

        fread(buffer, 8, 1, encInfo->src_image_fptr);     // Reading  8 bytes from source image
        encode_1byte_to_lsb(magic_string[i], buffer);     // Encoding one byte into 8 bytes
        fwrite(buffer, 8, 1, encInfo->output_image_fptr); // Writing  encoded data to output image
        /*
        pointer  = buffer
        size     = 8
        count    = 1
        file_ptr = encInfo->src_image_fptr
        */
    }
    printf("storing magic string success\n");
    return success; // return complition
}

// ------------------------------------------------------------------------------------------ Encode secrete file  ext  ------------------------------------------------------------------
/* Encode secret file extension */
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

// ------------------------------------------------------------------------------------------Encode  Secret file data ----------------------------------------------------------------------

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buffer[8];
    rewind(encInfo->secret_fptr); // Move secret file pointer to 0 inde
    // fseek(encInfo->secret_fptr, 0, SEEK_SET); //   also we can use fseek funtion here as rewind  for back to 0 inddex
    while (fread(&ch, 1, 1, encInfo->secret_fptr) > 0)
    {
        fread(buffer, 8, 1, encInfo->src_image_fptr);     // Read 8 bytes from source image
        encode_1byte_to_lsb(ch, buffer);                  // Encode one character
        fwrite(buffer, 8, 1, encInfo->output_image_fptr); // Write encoded data
    }
    printf("encoded secret file data succes");
    return success;
}

// --------------------------------------------------------------------------------------   encode 1 byte int LSB    -----------------------------------------------------------------------
/* Encode 1 byte into LSB of image data array */
Status encode_1byte_to_lsb(char data, char *buffer_8)
{
    for (int i = 0; i < 8; i++)
    {
        buffer_8[i] = (buffer_8[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return success;
}

// --------------------------------------------------------------------------------------- encode 4 byte form LSB ---------------------------------------------------------------------
/* Encode 4byte(int) into LSB of image data array */
Status encode_4byte_to_lsb(int data, char *buffer_32)
{
    for (int i = 0; i < 32; i++)
    {
        buffer_32[i] = (buffer_32[i] & 0xFE) | ((data >> (31 - i)) & 1);
    }
    return success;
}

// ------------------------------------------------------------------------------------  Coping  remanig data  -------------------------------------------------------------------------
/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;

    while (fread(&ch, 1, 1, encInfo->src_image_fptr) > 0)
    {
        fwrite(&ch, 1, 1, encInfo->output_image_fptr);
    }

    return success;
}