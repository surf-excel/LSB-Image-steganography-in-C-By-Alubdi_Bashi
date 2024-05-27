#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#pragma pack(push, 1)
//This code was made By Alubdi Bashi..
//1.Mihir The Vung_vang
//2.Mainul Islam
//3.Sarowar Hossain Rony
//4.Shahriar Emon
//5.Kibria Hemel

typedef struct {
    unsigned char bfType[2];
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

// Function to encrypt plaintext using multiplicative cipher with key 7
void multiplicativeEncrypt(char *plaintext) {
    int key = 7; // Multiplicative cipher key
    int i;
    for (i = 0; plaintext[i] != '\0'; ++i) {
        if (isalpha(plaintext[i])) {
            if (islower(plaintext[i])) {
                plaintext[i] = ((plaintext[i] - 'a') * key) % 26 + 'a';
            } else if (isupper(plaintext[i])) {
                plaintext[i] = ((plaintext[i] - 'A') * key) % 26 + 'A';
            }
        }
    }
}

void hideMessage(const char *inputFilename, const char *outputFilename) {
    // Open the input BMP file
    FILE *image = fopen(inputFilename, "rb");
    if (image == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Read BMP file header
    BITMAPFILEHEADER fileHeader;
    if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, image) != 1) {
        perror("Error reading file header");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Read BMP info header
    BITMAPINFOHEADER infoHeader;
    if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, image) != 1){
        perror("Error reading info header");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Check if the file is a BMP file
    //The DNA test to confirm it's a BMP.
    if (fileHeader.bfType[0] != 'B' || fileHeader.bfType[1] != 'M'){
        fprintf(stderr, "Not a BMP file.\n");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Check if it's a 24-bit BMP file
    if (infoHeader.biBitCount != 24) {
        fprintf(stderr, "Not a 24-bit BMP file.\n");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Move file pointer to the beginning of the pixel data
    if (fseek(image, fileHeader.bfOffBits, SEEK_SET) != 0) {
        perror("Error seeking to pixel data");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Calculate the size of the image data
    //Just like good advice, we keep the width height positive.
    int bytesPerPixel = 3; // 24-bit BMP, 8 bits for each of the RGB components
    int width = abs(infoHeader.biWidth); // Ensure width is positive
    int height = abs(infoHeader.biHeight); // Ensure height is positive
    int imageSize = width * height * bytesPerPixel;

    // Allocate memory for the image data
    //Let's make some room for our pixel pals
    unsigned char *imageData = (unsigned char *)malloc(imageSize);
    if (imageData == NULL) {
        fprintf(stderr, "Memory allocation failed for image data (%d bytes).\n", imageSize);
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Read the image data
    //Time to get pixelated!
    if (fread(imageData, imageSize, 1, image) != 1) {
        fprintf(stderr, "Error reading pixel data.\n");
        free(imageData);
        fclose(image);
        exit(EXIT_FAILURE);
    }
    fclose(image);

    // Ask user for the message to hide
    char message[256]; // Maximum message length
    printf("Enter the message to hide: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove newline character

    // Encrypt the message
    multiplicativeEncrypt(message);

    // Embed the message length in the first byte
    int messageLength = strlen(message);
    if (messageLength > 255) {
        fprintf(stderr, "Message too long to embed.\n");
        free(imageData);
        exit(EXIT_FAILURE);
    }
    imageData[0] = (unsigned char)messageLength;

    // Embed the message
    for (int i = 0; i < messageLength; ++i) {
        for (int bit = 0; bit < 8; ++bit) {
            int pixelIndex = 3 + i * 8 + bit;
            if (pixelIndex >= imageSize) {
                fprintf(stderr, "Image too small to hold the entire message.\n");
                free(imageData);
                exit(EXIT_FAILURE);
            }
            imageData[pixelIndex] = (imageData[pixelIndex] & 0xFE) | ((message[i] >> bit) & 0x01);
        }
    }

    // Write the modified data to a new file
    FILE *output = fopen(outputFilename, "wb");
    if (output == NULL) {
        perror("Error opening output file");
        free(imageData);
        exit(EXIT_FAILURE);
    }

    if (fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, output) != 1 ||
        fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, output) != 1 ||
        fwrite(imageData, imageSize, 1, output) != 1) {
        fprintf(stderr, "Error writing to output file.\n");
        free(imageData);
        fclose(output);
        exit(EXIT_FAILURE);
    }

    fclose(output);
    free(imageData);

    printf("Message embedded successfully.\n");
}

int main() {
    const char *inputFilename = "input.bmp";  // Input BMP file
    const char *outputFilename = "output.bmp";  // Output BMP file

    hideMessage(inputFilename, outputFilename);
    // Wait for the user to press a key before closing
    //Don't leave us hanging!
    printf("Press any key to exit...");
    getchar();


    return 0;
}
//Special Thanks To
//Professor K M Akkas Ali
//Institute of Information Technology,Jahangirnagar University
