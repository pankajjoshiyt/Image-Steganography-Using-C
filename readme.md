


================================================================================================

# Image Steganography using C
## Project Overview
Image Steganography is a technique used to hide secret information inside an image without changing the visible appearance of the image. 
This project uses the Least Significant Bit (LSB) technique to hide text data inside a BMP image.
The program supports:
- Encoding secret text into a BMP image
- Decoding hidden text from a stego image
- Validation of input files
- Extension handling of secret file
- Magic string verification

================================================================================================

**flow of the Encoding Process**
 * Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes

 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: success or failure, on file errors
 
**getting the 1 bit how its working**
pointer  = buffer
size     = 8
count    = 1
file_ptr = encInfo->src_image_fptr

For project these are all command line arguments:
**Encoding**
./stego.out -e beautiful.bmp secret.txt output.bmp
Arguments:
argv[0] --> ./stego.out
argv[1] --> -e
argv[2] --> beautiful.bmp
argv[3] --> secret.txt
argv[4] --> output.bmp

**Decoding**
./stego.out -d output.bmp decoded
Arguments:
argv[0] --> ./stego.out
argv[1] --> -d
argv[2] --> output.bmp
argv[3] --> decoded


Generated file:
decoded.txt
because extension (`.txt`) is automatically decoded and appended.

================================================================================================
## Project Files
main.c
encode.c
encode.h
decode.c
decode.h
common.h
beautiful.bmp
secret.txt
output.bmp
decoded.txt
size.txt      // extra file for storing secrete.txt file size 
README.md
================================================================================================

### size.txt
This file stores the size of the secret file.
Purpose:
- During encoding, the size of `secret.txt` is calculated using:
unsigned int get_file_size(FILE *fptr)
- The calculated size is written into `size.txt`
Example:
24
- During decoding, the size is read from `size.txt`
- The decoder uses this value to determine how many bytes should be decoded.
This prevents decoding extra image bytes and avoids unwanted characters.

================================================================================================

## Project Flow
### Encoding Flow
Input:
beautiful.bmp + secret.txt
        ↓
Validate arguments
        ↓
Open files
        ↓
Check image capacity
        ↓
Copy BMP header (54 bytes)
        ↓
Encode magic string (#*)
        ↓
Encode secret file extension (.txt)
        ↓
Encode secret text using LSB
and add size in in size.txt for decoding 
        ↓
Copy remaining image data
        ↓
Generate output.bmp

================================================================================================
### Decoding Flow
Input:
output.bmp
        ↓
Validate arguments
        ↓
Open stego image
        ↓
Skip BMP header
        ↓
Decode magic string
        ↓
Verify hidden data exists
        ↓
Decode extension (.txt)
        ↓
Decode secret text size with size.txt
        ↓
Create decoded.txt

================================================================================================
## LSB Working Principle
The Least Significant Bit technique stores one bit of secret data into the least significant bit of image bytes.
Example:
Secret character:
A
ASCII:
01000001
Image bytes:
11110010
11001011
10101100
...
After encoding:
11110010
11001011
10101101
...
Only the last bit changes, therefore image appearance remains almost unchanged.

================================================================================================
## Encoding Process
### Command:
bash
gcc main.c encode.c decode.c -o stego.out
Run:
bash
./stego.out -e beautiful.bmp secret.txt output.bmp
Arguments:
argv[0] → ./stego.out
argv[1] → -e
argv[2] → beautiful.bmp
argv[3] → secret.txt
argv[4] → output.bmp
================================================================================================
## Decoding Process

Run:
bash
./stego.out -d output.bmp decoded
Arguments:
argv[0] → ./stego.out
argv[1] → -d
argv[2] → output.bmp
argv[3] → decoded

Generated output:
decoded.txt

================================================================================================
## Input File
### secret.txt
My password is Secret ;
================================================================================================
## Output File
### decoded.txt
My password is Secret ;
================================================================================================

**Both files contain identical content after successful decoding.**

================================================================================================
## Features
✔ Encode text into BMP image
✔ Decode hidden text from image
✔ BMP validation
✔ Secret file validation
✔ Output file validation
✔ Magic string verification
✔ File extension handling
✔ Capacity checking
✔ Error handling
================================================================================================
## Result
The secret text is successfully hidden inside the source image and retrieved back without modifying the original message.
This is ready to save directly as `README.md`.
================================================================================================
