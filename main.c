#include "PPM.c"
#include <math.h>



typedef struct{
    int16_t x, y;
} PointI2D;

typedef struct{
    int16_t x, y, z;
} PointI3D;

void swap_points(PointI2D* p1, PointI2D* p2)
{
    PointI2D temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}



PointI2D conver2D_to_screen(PointI2D* p)
{
    PointI2D res;
    res.x = (p->x + X_MAX) * IMG_WIDTH /  (2 * X_MAX);
    res.y = (Y_MAX - p->y) * IMG_HEIGHT / (2 * Y_MAX);
    return res;
}


PointI2D convert3D_to_2D(PointI3D* p, int z_near)
{
    PointI2D res;
    res.x = (p->x * z_near) / p->z;
    res.y = (p->y * z_near) / p->z;
    return res;
}

void DDALine(PPM_image* image, PointI2D* point1, PointI2D* point2, ColorB* color)
{
    PointI2D p1 = conver2D_to_screen(point1);
    PointI2D p2 = conver2D_to_screen(point2); 
    printf("P1: %d %d\n", p1.x, p1.y);
    printf("P2: %d %d\n", p2.x, p2.y);
       
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    int steps = abs(dx);
    if(abs(dy) > abs(dx)) steps = abs(dy);
    
    float xinc = dx / (float)steps;
    float yinc = dy / (float)steps; 
    float px = p1.x, py = p1.y;
    for(int i = 0; i < steps; i++)
    {
        put_pixel(image ,round(px), round(py), color);
        px += xinc;
        py += yinc;
    }
}



void drawCube_wire(PPM_image* image, PointI3D* p_arr, ColorB* color)
{
    PointI2D res_arr[8];
    for(int i = 0; i < 8; i++)
    {
        res_arr[i] = convert3D_to_2D(&p_arr[i], 1);
    }
    for(int i = 0; i < 8; i++)
    {
        for(int j = i + 1; j < 8; j++)
        {
            DDALine(image, &res_arr[i], &res_arr[j], color);
        }
    }
}


void drawTriangle_wire(PPM_image* image, PointI2D* p_arr, ColorB* color)
{
    DDALine(image, &p_arr[0], &p_arr[1], color);
    DDALine(image, &p_arr[0], &p_arr[2], color);
    DDALine(image, &p_arr[1], &p_arr[2], color);
}

int* interpolate(PointI2D* p1, PointI2D* p2)
{
    int dx = p2->x - p1->x;
    int dy = p2->y - p1->y;
    int steps = abs(dy);
    
    float xinc = dx / (float)steps;
    float px = p1->x;

    int* res_arr = (int*)malloc(sizeof(int) * (steps + 1));
    res_arr[0] = steps;
    
    for(int i = 0; i < steps; i++)
    {
        res_arr[i + 1] = round(px);
        px += xinc;
    }

    return res_arr;
}

void drawTriangle_fill(PPM_image* image, PointI2D* p_arr, ColorB* color)
{
    if(p_arr[0].y > p_arr[1].y) swap_points(&p_arr[0], &p_arr[1]);
    if(p_arr[0].y > p_arr[2].y) swap_points(&p_arr[0], &p_arr[2]);
    if(p_arr[1].y > p_arr[2].y) swap_points(&p_arr[1], &p_arr[2]);

    PointI2D p_arr2[3];
    for(int i = 0; i < 3; i++)
    {
        p_arr2[i] = conver2D_to_screen(&p_arr[i]); 
    }

    int* inter_p[3];
    inter_p[0] = interpolate(&p_arr2[0], &p_arr2[1]);
    inter_p[1] = interpolate(&p_arr2[1], &p_arr2[2]);
    inter_p[2] = interpolate(&p_arr2[0], &p_arr2[2]);

    printf("%d %d %d\n", inter_p[0][0],inter_p[1][0],inter_p[2][0]);

    for(int i = 0; i < 3; i++)
    {
        fprintf(stdout, "%d %d\n", p_arr2[i].x, p_arr2[i].y);
    }

    int k = 1, y = p_arr2[0].y, size;
    for(int i = 0; i < 2; i++)
    {
        size = inter_p[i][0];
        for(int j = 1; j < size; j++, k++)
        {
            if(inter_p[i][size / 2] < inter_p[2][size / 2])
            {
                for(int x = inter_p[i][j]; x < inter_p[2][k]; x++)
                {
                    put_pixel(image, x, y, color);
                }
            }
            else
            {
                for(int x = inter_p[2][k]; x < inter_p[i][j]; x++)
                {
                    put_pixel(image, x, y, color);
                }
            }
            y--;
        }

    }

    for(int i = 0; i < 3; i++)
    {
        free(inter_p[i]);
    }
    
}

int main(int argc, char* argv[])
{
        
    PPM_image* image = createPPM_image("test2.ppm", 500, 500);
    // PointI2D arr[3] = {{-200, -200}, {150, 60}, {200, 0}};
    // PointI2D arr2[3] = {{200, 100}, {0, 0}, {-150, 200}};
    ColorB color = {.BLUE=255, .GREEN=255, .RED=0};
    // ColorB color2 = {.BLUE=0, .GREEN=0, .RED=255};
    // drawTriangle_fill(image, arr2, &color2);
    // drawTriangle_fill(image, arr, &color);
    PointI3D arr[8] = {{-200, -200, 1}, {0, -200, 1}, 
                        {0, 0, 1}, {-200, 0, 1},
                        {-200, -200, 2}, {0, -200, 2}, 
                        {0, 0, 2}, {-200, 0, 2}};

    drawCube_wire(image, arr, &color);

    closePPM_image(image); 
    
}