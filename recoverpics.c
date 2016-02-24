/**
 * Seab C. Jackson
 
 * This program is an attempt to prove that deleting files from a HDD doesn't erase
 * the actual bits of a program. The computer merely "forgets" where those bits are located 
 *
 *
 * Recovers JPEGs from a forensic image. (adapted from Harvard CS50)
 * Assumption: digital camera stored photographs contiguously, whereby each photo is stored immediately after the previously taken photo
 */

 
 
 #include <stdint.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <string.h>
 #include <stdlib.h>
 
 #define blockSize 512 // FAT file system has block size of 512 Bytes
 
 typedef uint8_t BYTE;
 
 // function prototypes
 bool findNewImage(void);
 void writeToImage(int jpegCount);
 

 BYTE* buffer = NULL;

int main(int argc, char* argv[])
{

    // check for valid inputs
    if (argc != 2)
    {
        printf("usage: ./recoverpics jpeg\n");
        return 1;
    }
    
    // the user should type jpeg after the program's name
    if (strcmp(argv[1], "jpeg") != 0)
    {
        printf("Type 'jpeg' in lowercase\n");
        return 1;
    }
    
    char* jpeg = argv[1];
    
    // create counter to track how many images received and use as counter.jpg as file name (first image is 001.jpg)
    int jpegCount = 1;
    
    //  starts off initially with no images found
    bool foundJpeg = false;
    
   
    // open the memory card in read mode to scan it for jpeg signatures
    jpeg = "card.raw";
    
    FILE* inptr = fopen(jpeg, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s\n", jpeg);
        return 1;
    }
    
    // create buffer which we will read to 
    buffer = malloc(sizeof(BYTE) * blockSize);
    
    // we read 512 bytes at a time and only check the first 4 bytes of the buffer for jpeg signatures
    while(fread(buffer, blockSize, 1, inptr) == 1)
    {
            /* when already writing data to build a jpeg image file keep writing to that file until we find a new image
            note the assumption that the jpegs are saved contiguously in memory such that the beginning of the jpeg 
            is an indication of the previous image's completion.*/
            
            if (foundJpeg == true)
            {
                if (findNewImage() == true)
                {
                    jpegCount++;
                }
                writeToImage(jpegCount);
            }
            else if (findNewImage() == true) // keep reading the card until we find the first image
            {
                foundJpeg = true;
                writeToImage(jpegCount);
            }

    }
    // free buffer since we used malloc to allocate memory for it in the heap
    free(buffer);
    fclose(inptr);      
}

// a jpegs first 3 bytes always start with 0xff 0xd8 0xff
// the fourth byte of the jpeg can be either 0xe0 or 0xe1
// function memcmp() is well suitable to compare the buffer data for jpeg signatures
// note we only check the first 4 bytes of the buffer
bool findNewImage(void)
{ 
    
    // return (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2]== 0xff && (buffer[3]== 0xe0 || buffer[3]==0xe1) ) ? true : false;
    
    return  (memcmp(buffer,"\xFF\xD8\xFF\xE0", 4) == 0 || memcmp(buffer, "\xFF\xD8\xFF\xE1", 4) == 0) ? true : false;
}

void writeToImage(int jpegCount)
{
     // name found files as ###.jpg so need an extra for null character
    char imageName[8];
    sprintf(imageName, "%03d.jpg", jpegCount); // ###.jpg
    
     // open image file to write to
                 
     FILE* outptr = fopen(imageName, "a");
     if (outptr == NULL)
     {
          printf("couldn't open the outfile\n");
          return;
     }
     fwrite(&buffer[0], blockSize, 1, outptr); // write contents from buffer to the outfile
     fclose(outptr);
                 
}