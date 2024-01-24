/*
Name        :   Ashwin Raj K
Description :   Steganography - Encoding
Date        :   06/06/2023
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)                  
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Function definition for check operation type 
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
       return e_encode;
    else if(strcmp(argv[1], "-d") == 0)
       return e_decode;
    else
       return e_unsupported; 
}

// Function definition for read and validate encode args
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(strchr(argv[2], '.'), ".bmp") == 0 )              //comparing whether the argument is same as the required input
        encInfo -> src_image_fname = argv[2];
    else
       return e_failure;

    if(strcmp(strchr(argv[3], '.'), ".txt") == 0 )              //comparing whether the argument is same as the required input       
        encInfo -> secret_fname = argv[3];
    else
        return e_failure;

    if(argv[4] == NULL)                                         //if argument is Null
        encInfo -> stego_image_fname = "stego.bmp";
    else
        encInfo -> stego_image_fname = argv[4];
    return e_success;
}

// Function definition for getting file size
long get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

// Function definition for check capacity
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    if(encInfo -> image_capacity > (16 + 32 + 32 + 32 +  (encInfo -> size_secret_file * 8))) 
        return e_success;
    else
        return e_failure;
}

// Function definition for copying 1st 54 bytes header file

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{  
    char str[54];
    
    // Setting pointer to point to 0th position
    fseek(fptr_src_image, 0,SEEK_SET);
    
    // Reading 54 bytes from beautiful.bmp
    fread(str, 54, 1, fptr_src_image);

    // Writing 54 bytes to str
    fwrite(str, 54, 1, fptr_dest_image);

    return e_success;
}

// Function definition for encode byte to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE | ((data >> (7 - i)) & 1));
    }
    return e_success;
}

// Function definition for Encode data to image
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image);

        encode_byte_to_lsb(data[i], encInfo -> image_data);

        fwrite(encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

// Function definition for encoding magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);
   
    return e_success;
}    

// Function definition to encode size to lsb
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char str[32];

    fread(str, 32, 1,encInfo -> fptr_src_image);
    for(int i = 0;i < 32; i++)
    {
        str[i] = (str[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }

    fwrite(str, 32, 1, encInfo -> fptr_stego_image);
    return e_success;
}

// Function definition for encode extn size
Status encode_extn_size(int size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(size, encInfo);
  
    return e_success;
}

// Function definition for encode secret file extn size
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    encode_data_to_image(encInfo -> extn_secret_file, strlen(encInfo -> extn_secret_file), encInfo);
    
    return e_success;
}

// Function definition for encode secret file size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size, encInfo);

    return e_success;
}

// Function definition for encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);

    char str[encInfo -> size_secret_file];
    fread(str,encInfo -> size_secret_file, 1, encInfo -> fptr_secret);

    encode_data_to_image(str, encInfo -> size_secret_file, encInfo);
    return e_success;
}           

// Function definition for copying remaining data as it is
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;
    while(fread(&ch ,1 ,1 ,fptr_src) > 0)
            fwrite(&ch ,1 ,1 ,fptr_stego);

    return e_success;
}

// Function definition for do encoding called in main function
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)                        //checks if file can be opened
    {
       printf("Opened files successfully\n");

       if(check_capacity(encInfo) == e_success)                 //checks the capacity
       {
           printf("Check capacity is success\n");

           if(copy_bmp_header(encInfo -> fptr_src_image,encInfo ->fptr_stego_image) == e_success)  //checks bmp header copied     
           {
               printf("Copied bmp header successfully\n");

               if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)          //checks whether magic string encoded
               {
                   printf("Encoded magic string successfully\n");

                   strcpy(encInfo -> extn_secret_file, strchr(encInfo -> secret_fname, '.'));       //copying secret filename to extn_secret_file

                   if(encode_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)  //checks whether file extension encoded 
                   {
                       printf("Encoded file extension successfully\n");

                       if(encode_secret_file_extn(encInfo) == e_success)                //checks whether secret file extension encoded
                       {
                           printf("Encoded secret file extension successfully\n");

                           if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)       //checks whether secret file size encoded
                           {
                               printf("Encoded secret file size successfully\n");
                               
                               if(encode_secret_file_data(encInfo) == e_success)            //checks whether secret data encoded
                               {
                                   printf("Encoded secret file data successfully\n");

                                   if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo ->fptr_stego_image) == e_success)              //checks whether remaining data copied
                                   {
                                       printf("Copied remaining data\n");
                                   }
                                   else
                                   {
                                       printf("Failed to copy remaining data\n");   //prints Failed to copy remaining data and returns e_failure
                                       return e_failure;
                                   }
                               }
                               else
                               {
                                   printf("Failed to encode secret file data\n");   //prints Failed to encode secret file data and returns e_failure
                                   return e_failure;
                               }
                           }
                           else
                           {
                               printf("Failed to encode secret file size\n");   //prints Failed to encode secret file size and returns e_failure
                               return e_failure;
                           }

                       }
                       else
                       {
                           printf("Failed to encode secret file extension\n");  //prints Failed to encode secret file extension and returns e_failure
                           return e_failure;
                       }
                   }
                   else
                   {
                       printf("Failed to encode file extension\n");         //prints Failed to encode file extension and returns e_failure
                       return e_failure;
                   }
               }
               else
               {
                   printf("Failed to encode magic string\n");              //prints Failed to encode magic string and returns e_failure     
                   return e_failure;
               }
           }
           else
           {
               printf("Failed to copy bmp header\n");                      //prints Failed to copy bmp header and returns e_failure
               return e_failure;
           }
       }
       else
       {
           printf("Check capacity is a failure\n");                        //prints check capacity is a failure and returns e_failure
           return e_failure;
       }
    }
    else
    {
       printf("Failed to open files\n");                                    //prints failed to open files and returns e_failure
      return e_failure;
    } 
    return e_success;
}
