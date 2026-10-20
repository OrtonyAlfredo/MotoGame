#ifndef GFX_H
#define GFX_H

#define SCR_W 480
#define SCR_H 272

void gfx_init(void);   /* inicia o GU (ecrã, depth buffer, estados) */
void gfx_begin(void);  /* começa o frame: limpa e define a projeção */
void gfx_end(void);    /* termina o frame: sincroniza e troca buffers */

#endif
