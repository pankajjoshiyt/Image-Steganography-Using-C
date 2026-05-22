#ifndef DECODE_H
#define DECODE_H

#include "common.h"

typedef struct DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;      // Store stego image name
    FILE *stego_image_fptr;       // File pointer for stego image

    /* Secret File Info */
    char secret_fname[25];        // Store output secret file name
    FILE *secret_fptr;            // File pointer for output secret file
    char secret_extn[7];          // Store extension
    int secret_extn_size;         // Store extension size
    unsigned long int secret_file_size; // Store secret file size

} DecodeInfo;


/* Read and validate Decode args */
Status validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode 1 byte from LSB */
char decode_1byte_from_lsb(char *buffer_8);

/* Decode 4 bytes from LSB */
int decode_4byte_from_lsb(char *buffer_32);

#endif