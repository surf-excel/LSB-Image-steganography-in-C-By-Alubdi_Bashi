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

// File header structure: Handles the BMP file with more care than handling your heart.
typedef struct {
    unsigned char bfType[2];
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} BITMAPFILEHEADER;

// Info header structure: Because BMP files have secrets, just like your ex.
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

// Function to decrypt ciphertext using multiplicative cipher with key 7
// Because we like to multiply the confusion before we make it disappear.
void multiplicativeDecrypt(char *ciphertext) {
    int key = 7;
    int inverse = 15; // Precomputed inverse of 7 mod 26. No magic, just math.

    for (int i = 0; ciphertext[i] != '\0'; ++i) {
        if (isalpha(ciphertext[i])) {
            if (islower(ciphertext[i])) {
                ciphertext[i] = ((ciphertext[i] - 'a') * inverse) % 26 + 'a';
            } else if (isupper(ciphertext[i])) {
                ciphertext[i] = ((ciphertext[i] - 'A') * inverse) % 26 + 'A';
            }
        }
    }
}

void extractMessage(const char *inputFilename) {
    // Open the input BMP file: Let's open this pixel party!
    FILE *image = fopen(inputFilename, "rb");
    if (image == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // Read BMP file header: The part where we learn about the BMP's family history.
    BITMAPFILEHEADER fileHeader;
    if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, image) != 1) {
        perror("Error reading file header");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Read BMP info header: More about the BMP's personal life.
    BITMAPINFOHEADER infoHeader;
    if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, image) != 1) {
        perror("Error reading info header");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Check if the file is a BMP file: The DNA test to confirm it's a BMP.
    //Not By CID forensic Dept..
    if (fileHeader.bfType[0] != 'B' || fileHeader.bfType[1] != 'M') {
        fprintf(stderr, "Not a BMP file.\n");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Check if it's a 24-bit BMP file: We don't do anything less. It's 24-bit or bust!
    if (infoHeader.biBitCount != 24) {
        fprintf(stderr, "Not a 24-bit BMP file.\n");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Move file pointer to the beginning of the pixel data: Pixels, here we come!
    if (fseek(image, fileHeader.bfOffBits, SEEK_SET) != 0) {
        perror("Error seeking to pixel data");
        fclose(image);
        exit(EXIT_FAILURE);
    }

    int bytesPerPixel = 3;
    int width = abs(infoHeader.biWidth); // Just like good advice, we keep the width positive.
    int height = abs(infoHeader.biHeight); // And the height too. Stay positive!
    int imageSize = width * height * bytesPerPixel;

    // Allocate memory for the image data: Let's make some room for our pixel pals.
    unsigned char *imageData = (unsigned char *)malloc(imageSize);
    if (imageData == NULL) {
        fprintf(stderr, "Memory allocation failed for image data (%d bytes).\n", imageSize);
        fclose(image);
        exit(EXIT_FAILURE);
    }

    // Read the image data: Time to get pixelated!
    if (fread(imageData, imageSize, 1, image) != 1) {
        fprintf(stderr, "Error reading pixel data.\n");
        free(imageData);
        fclose(image);
        exit(EXIT_FAILURE);
    }
    fclose(image);

    // Extract the message length from the first byte: How long is this secret?
    int messageLength = imageData[0];

    // Allocate memory for the hidden message: Now, a cozy place for our secret.
    char *message = (char *)malloc(messageLength + 1);
    if (message == NULL) {
        fprintf(stderr, "Memory allocation failed for message.\n");
        free(imageData);
        exit(EXIT_FAILURE);
    }

    // Extract the hidden message: Picking out the secret, one bit at a time.
    for (int i = 0; i < messageLength; ++i) {
        message[i] = 0;
        for (int bit = 0; bit < 8; ++bit) {
            int pixelIndex = 3 + i * 8 + bit;
            message[i] |= (imageData[pixelIndex] & 0x01) << bit;
        }
    }
    message[messageLength] = '\0'; // Null-terminate the string, because we're polite.

    // Decrypt the hidden message: Time to decode this digital enigma.
    multiplicativeDecrypt(message);

    // Print the decrypted hidden message: The moment of truth!
    printf("Hidden message: %s\n", message);

    // Free allocated memory: Clean up after ourselves like responsible adults.
    free(imageData);
    free(message);
}

int main() {
    const char *inputFilename = "output.bmp";
    extractMessage(inputFilename);

    // Wait for the user to press a key before closing: Don't leave us hanging!
    printf("Press any key to exit...");
    getchar();

    return 0;
}
//Special Thanks To
//Professor K M Akkas Ali
//Institute of Information Technology,Jahangirnagar University
