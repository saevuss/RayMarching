#ifndef NOISE_H
#define NOISE_H

#include <glm.hpp>

double fade(double t);
double lerp(double t, double a, double b);
double grad(int hash, double x, double y, double z);
double noise(glm::vec3 pSample);
#endif
