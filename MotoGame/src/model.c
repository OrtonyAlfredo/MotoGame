#include <pspkernel.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "model.h"

#define MAX_BIKE   1024
#define MAX_GROUND 2048
#define PI_F       3.14159265f

static Vertex bike[MAX_BIKE]     __attribute__((aligned(16)));
static Vertex ground[MAX_GROUND] __attribute__((aligned(16)));
static int bike_n, ground_n;

/* escurece/clareia uma cor ABGR (iluminação falsa por face) */
static unsigned int shade(unsigned int c, float k)
{
    unsigned int r = (unsigned int)((c & 0xff) * k);
    unsigned int g = (unsigned int)(((c >> 8) & 0xff) * k);
    unsigned int b = (unsigned int)(((c >> 16) & 0xff) * k);
    if (r > 255) r = 255;
    if (g > 255) g = 255;
    if (b > 255) b = 255;
    return 0xff000000u | (b << 16) | (g << 8) | r;
}

static void put(Vertex *a, int *n, unsigned int c, float x, float y, float z)
{
    a[*n].color = c;
    a[*n].x = x;
    a[*n].y = y;
    a[*n].z = z;
    (*n)++;
}

static void tri(Vertex *a, int *n, unsigned int c,
                float x0, float y0, float z0,
                float x1, float y1, float z1,
                float x2, float y2, float z2)
{
    put(a, n, c, x0, y0, z0);
    put(a, n, c, x1, y1, z1);
    put(a, n, c, x2, y2, z2);
}

static void quad(Vertex *a, int *n, unsigned int c,
                 float x0, float y0, float z0,
                 float x1, float y1, float z1,
                 float x2, float y2, float z2,
                 float x3, float y3, float z3)
{
    tri(a, n, c, x0, y0, z0, x1, y1, z1, x2, y2, z2);
    tri(a, n, c, x0, y0, z0, x2, y2, z2, x3, y3, z3);
}

/* caixa centrada em (cx,cy,cz) com meias-dimensões (hx,hy,hz) */
static void add_box(Vertex *a, int *n,
                    float cx, float cy, float cz,
                    float hx, float hy, float hz, unsigned int col)
{
    float x0 = cx - hx, x1 = cx + hx;
    float y0 = cy - hy, y1 = cy + hy;
    float z0 = cz - hz, z1 = cz + hz;

    quad(a, n, shade(col, 1.0f), x0,y1,z0, x1,y1,z0, x1,y1,z1, x0,y1,z1); /* topo */
    quad(a, n, shade(col, 0.5f), x0,y0,z0, x1,y0,z0, x1,y0,z1, x0,y0,z1); /* base */
    quad(a, n, shade(col, 0.8f), x1,y0,z0, x1,y1,z0, x1,y1,z1, x1,y0,z1); /* +X */
    quad(a, n, shade(col, 0.8f), x0,y0,z0, x0,y1,z0, x0,y1,z1, x0,y0,z1); /* -X */
    quad(a, n, shade(col, 0.9f), x0,y0,z1, x1,y0,z1, x1,y1,z1, x0,y1,z1); /* +Z */
    quad(a, n, shade(col, 0.6f), x0,y0,z0, x1,y0,z0, x1,y1,z0, x0,y1,z0); /* -Z */
}

/* roda: cilindro ao longo do eixo X */
static void add_wheel(Vertex *a, int *n,
                      float cx, float cy, float cz,
                      float r, float hw, unsigned int col)
{
    const int SEG = 12;
    int i;
    for (i = 0; i < SEG; i++) {
        float a0 = 2.0f * PI_F * i / SEG;
        float a1 = 2.0f * PI_F * (i + 1) / SEG;
        float y0 = cy + r * cosf(a0), z0 = cz + r * sinf(a0);
        float y1 = cy + r * cosf(a1), z1 = cz + r * sinf(a1);
        float k = (i & 1) ? 0.85f : 1.0f;

        quad(a, n, shade(col, k),
             cx - hw, y0, z0, cx + hw, y0, z0,
             cx + hw, y1, z1, cx - hw, y1, z1);
        tri(a, n, shade(col, 0.7f), cx + hw, cy, cz, cx + hw, y0, z0, cx + hw, y1, z1);
        tri(a, n, shade(col, 0.7f), cx - hw, cy, cz, cx - hw, y1, z1, cx - hw, y0, z0);
    }
}

void model_build(void)
{
    int i, j;

    bike_n = 0;
    /* frente da mota = +Z */
    add_wheel(bike, &bike_n, 0.0f, 0.35f, -0.80f, 0.35f, 0.08f, 0xff202020); /* traseira */
    add_wheel(bike, &bike_n, 0.0f, 0.35f,  0.80f, 0.35f, 0.07f, 0xff202020); /* frente   */
    add_box(bike, &bike_n, 0.0f, 0.42f,  0.00f, 0.13f, 0.13f, 0.22f, 0xff808080); /* motor     */
    add_box(bike, &bike_n, 0.0f, 0.78f,  0.15f, 0.15f, 0.12f, 0.30f, 0xff2020d0); /* depósito  */
    add_box(bike, &bike_n, 0.0f, 0.72f, -0.40f, 0.14f, 0.06f, 0.26f, 0xff181818); /* assento   */
    add_box(bike, &bike_n, 0.0f, 0.62f,  0.78f, 0.03f, 0.30f, 0.03f, 0xffb0b0b0); /* forquilha */
    add_box(bike, &bike_n, 0.0f, 0.95f,  0.74f, 0.36f, 0.03f, 0.03f, 0xff303030); /* guiador   */
    add_box(bike, &bike_n, 0.0f, 0.82f,  0.88f, 0.08f, 0.07f, 0.04f, 0xff30e0ff); /* farol     */
    add_box(bike, &bike_n, 0.18f,0.30f, -0.55f, 0.04f, 0.04f, 0.38f, 0xff9090a0); /* escape    */

    ground_n = 0;
    for (i = -8; i < 8; i++) {
        for (j = -8; j < 8; j++) {
            unsigned int c = ((i + j) & 1) ? 0xff3a6b3a : 0xff2f5a2f;
            float x0 = i * 2.0f, x1 = x0 + 2.0f;
            float z0 = j * 2.0f, z1 = z0 + 2.0f;
            quad(ground, &ground_n, c, x0,0,z0, x1,0,z0, x1,0,z1, x0,0,z1);
        }
    }

    /* garante que os dados chegam ao GE (memória não-cacheada) */
    sceKernelDcacheWritebackAll();
}

void model_draw_bike(void)
{
    sceGumDrawArray(GU_TRIANGLES, VERTEX_FLAGS, bike_n, 0, bike);
}

void model_draw_ground(void)
{
    sceGumDrawArray(GU_TRIANGLES, VERTEX_FLAGS, ground_n, 0, ground);
}
