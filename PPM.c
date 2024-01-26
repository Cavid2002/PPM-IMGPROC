#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define IMG_HEIGHT 500
#define IMG_WIDTH 500
#define RGB_MAX 255
#define X_MAX 250
#define Y_MAX 250

typedef struct{
    uint8_t RED;
    uint8_t  GREEN;
    uint8_t  BLUE;
} ColorB;

typedef struct{
    FILE* file;
    uint16_t header_size;
    uint16_t width;
    uint16_t height;
    uint8_t* buff;
} PPM_image;

int get_header_size(FILE* image)
{
    char buff[20];
    int size = 0;
    for(int i = 0; i < 3; i++)
    {
        fgets(buff, 20, image);
        size += strlen(buff);
    }
    return size;
}

void get_file_size(FILE* file, uint16_t* width, uint16_t* height)
{
    fseek(file, 3, SEEK_SET);
    fscanf(file, "%d %d", width, height);
}

PPM_image* readPPm_image(const char* filename)
{
    PPM_image* image = (PPM_image*)malloc(sizeof(PPM_image));
    image->file = fopen(filename, "r+");
    image->header_size = get_header_size(image->file);
    get_file_size(image->file, &image->width, &image->height);


    fseek(image->file, image->header_size, SEEK_SET);
    size_t file_size = image->height * image->width * 3;
    image->buff = (uint8_t*)malloc(file_size);
    int temp;
    for(int i = 0; i < file_size; i++)
    {
        fscanf(image->file, "%d", &temp);
        image->buff[i] = temp;
    }
    
    return image;
}

void writePPM_image(PPM_image* image)
{
    fseek(image->file, image->header_size, SEEK_SET);
    int size = image->height * image->width * 3;
    for(int i = 0; i < size; i++)
    {
        fprintf(image->file, "%d ", image->buff[i]);
    }
}


PPM_image* createPPM_image(const char* filename, uint16_t width, uint16_t height)
{
    PPM_image* image = (PPM_image*)malloc(sizeof(PPM_image));
    image->file = fopen(filename, "w+");
    image->height = height; image->width = width;
    fprintf(image->file, "P3\n%d %d\n255\n", width, height);
    image->header_size = ftell(image->file);
    
    int size = width * height * 3;
    image->buff = (uint8_t*)malloc(size);
    for(int i = 0; i < size; i++)
    {
        image->buff[i] = 0;
    }   

    return image;
}

void closePPM_image(PPM_image* image)
{
    writePPM_image(image);
    fclose(image->file);
    free(image->buff);
    free(image);
    return;
}

void put_pixel(PPM_image* image, uint16_t x, uint16_t y, ColorB* color)
{
    image->buff[3 * (y * image->width + x)] = color->RED;
    image->buff[3 * (y * image->width + x) + 1] = color->GREEN;
    image->buff[3 * (y * image->width + x) + 2] = color->BLUE;
}