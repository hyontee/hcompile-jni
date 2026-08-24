// -- -- -- -- -- --
// Created by Loony-dev � 2021
// VK: https://vk.com/loonydev
// -- -- -- -

#pragma once

#include "../main.h"

class CObject;

class CSkyBox {
public:
    static void Initialise();
    static void InjectHooks();
    static void RenderSkybox();
};

class Skybox
{
public:
    bool inUse;
    RwTexture *tex;
    float rot;
    Skybox()
    {
        inUse = false;
        tex = NULL;
        rot = 0.0f;
    }
};