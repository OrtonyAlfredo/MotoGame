# MotoGame (PSP / PPSSPP)

Passo 1: mota 3D a rodar no PPSSPP.

## Controlos
- Analógico: orbitar a câmara
- L / R: aproximar / afastar
- Círculo: ligar/desligar rotação automática
- HOME: sair

## Compilar (Docker, sem instalar nada)
```
docker run --rm -v "$PWD":/src -w /src pspdev/pspdev make
```
Gera `EBOOT.PBP`. No PPSSPP: File > Load e escolhe o `EBOOT.PBP`.

## Estrutura
```
MotoGame/
  Makefile
  src/
    main.c    loop principal, input, câmara
    gfx.c/h   inicialização do GU, frame begin/end
    model.c/h geometria procedural (mota + chão)
```

## Próximos passos
2. Física da mota (aceleração, inclinação)
3. Pista por segmentos + câmara de perseguição
4. HUD, menus, IA, multijogador ad-hoc
