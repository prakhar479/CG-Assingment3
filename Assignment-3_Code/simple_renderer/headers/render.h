#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render();

    Scene scene;
    Texture outputImage;
};

int spp; // samples per pixel

Ray UniformSampleHemisphere(Interaction *it);
Ray UniformCosineSampleHemisphere(Interaction *it);

int Monte_Carlo_Itter = 100;

int samplingStrategy; // 0: Systematic Light Sampling; 1: Uniform Hemisphere Sampling; 2: Cosine Hemisphere Sampling; 3: Light Importance Sampling; 