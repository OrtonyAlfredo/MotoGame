#include <pspkernel.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "gfx.h"
#include "model.h"

PSP_MODULE_INFO("MotoGame", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* ---- botão HOME: sair do jogo ---- */
static int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

static int callback_thread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static void setup_callbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0)
        sceKernelStartThread(thid, 0, 0);
}

static float clampf(float v, float lo, float hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

int main(void)
{
    SceCtrlData pad;
    unsigned int prev = 0;
    float yaw = 0.6f, pitch = 0.25f, dist = 5.0f, spin = 0.0f;
    int auto_spin = 1;

    setup_callbacks();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    gfx_init();
    model_build();

    for (;;) {
        /* ---- input ---- */
        sceCtrlPeekBufferPositive(&pad, 1);
        float lx = (pad.Lx - 128) / 128.0f;
        float ly = (pad.Ly - 128) / 128.0f;
        if (fabsf(lx) < 0.15f) lx = 0.0f;
        if (fabsf(ly) < 0.15f) ly = 0.0f;

        yaw   += lx * 0.04f;
        pitch  = clampf(pitch - ly * 0.03f, 0.05f, 1.3f);

        if (pad.Buttons & PSP_CTRL_LTRIGGER) dist -= 0.08f;
        if (pad.Buttons & PSP_CTRL_RTRIGGER) dist += 0.08f;
        dist = clampf(dist, 2.5f, 12.0f);

        if ((pad.Buttons & PSP_CTRL_CIRCLE) && !(prev & PSP_CTRL_CIRCLE))
            auto_spin = !auto_spin;
        prev = pad.Buttons;

        if (auto_spin) spin += 0.01f;

        /* ---- render ---- */
        gfx_begin();

        ScePspFVector3 eye = {
            dist * cosf(pitch) * sinf(yaw),
            dist * sinf(pitch) + 0.8f,
            dist * cosf(pitch) * cosf(yaw)
        };
        ScePspFVector3 center = { 0.0f, 0.6f, 0.0f };
        ScePspFVector3 up     = { 0.0f, 1.0f, 0.0f };

        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        sceGumLookAt(&eye, &center, &up);

        sceGumMatrixMode(GU_MODEL);
        sceGumLoadIdentity();
        model_draw_ground();

        sceGumLoadIdentity();
        sceGumRotateY(spin);
        model_draw_bike();

        gfx_end();
    }

    return 0;
}
