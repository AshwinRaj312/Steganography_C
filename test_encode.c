/*
Name        :   Ashwin Raj K
Description :   Steganography - Test_Encoding
Date        :   06/06/2023
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char **argv)
{
    if(argc == 1)                                                           // Checks whether no command-line argument given
        
        printf("Pass arguments as:\nUsage for encoding:\n./a.out -e beautiful.bmp secret.txt [stego.bmp]\nUsage for decoding:\n./a.out -d stego.bmp [decode.txt]\n");
    
    else
    {   

        if(check_operation_type(argv) == e_encode)                          // Check if the operation type is encoding
        {
            printf("Selected encoding\n");
        
            EncodeInfo encInfo;                                             // Declare a variable of type EncodeInfo to store encoding information

            if(read_and_validate_encode_args(argv,&encInfo) == e_success)  // Checks whether reading and validating encoding arguments is successful
            {
                printf("Read and validate encode arguments is success\n");

                if(do_encoding(&encInfo) == e_success)                      // Checks whether the encoding operation is successful
                {
                    printf("Encoding is successful\n");               
                }
                else
                {
                    printf("Encoding is unsuccessful\n");                                                        
                    return 1;                                                 // If encoding is unsuccessful, return 1
                }

            }
            else
            {
                printf("Read and validate encode arguments is not success\n");  //prints not success and return 1
                return 1;
            }
        }
        else if(check_operation_type(argv) == e_decode)                         // Check if the operation type is decoding
        {
            printf("Selected decoding\n");

            DecodeInfo decInfo;                                                 // Declare a variable of type DecodeInfo to store decoding information

            if(read_and_validate_decode_args(argv, &decInfo) == d_success)
            {
                printf("Read and validate encode arguments is success\n");      // Checks whether reading and validating decoding arguments is successful

                if(do_decoding(&decInfo) == d_success)                          // Check whether the decoding operation is successful
                {

                    printf("Decoding is Successful\n");

                }
                else
                {
                    printf("Decoding is unsuccessful");
                    return e_failure;
                }
            }
                else
                {
                    printf("Read and vadidate decode arguments is not success\n");
                    return e_failure;
                }
            }

        else
        {
            printf("Invalid input!!\nUsage for encoding: .\a.out -e beautiful.bmp secret.txt [stego.bmp]\nUsage for decoding: .\a.out -d stego.bmp [decode.txt]\n");                                // If the operation type is neither encoding nor decoding, prints an invalid input message
        }
    }
    return 0;
}
