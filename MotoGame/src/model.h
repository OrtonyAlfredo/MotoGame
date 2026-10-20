#ifndef MODEL_H
#define MODEL_H

#include <pspgu.h>

typedef struct {
    unsigned int color;   /* ABGR */
    float x, y, z;
} Vertex;

#define VERTEX_FLAGS (GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D)

void model_build(void);       /* gera a geometria (uma vez, no arranque) */
void model_draw_bike(void);   /* desenha a mota na origem */
void model_draw_ground(void); /* desenha o chão quadriculado */

#endif
