#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f total_color = Vector3f(0, 0, 0);
            for (int s = 0; s < spp; s++)
            {
                Ray cameraRay = this->scene.camera.generateRay(x, y);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);

                Interaction siLight = this->scene.rayEmitterIntersect(cameraRay);
                if (siLight.didIntersect)
                    result += siLight.emissiveColor;

                /*
                if (si.didIntersect)
                {

                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        }
                    }


                    // Sample Dirfferent Directions


                }
                */

                
                if (si.didIntersect)
                { // Monte Carlo Integration

                    Vector3f total_sample = Vector3f(0, 0, 0);
                    switch (samplingStrategy)
                    {
                        case 0: // Uniform Hemisphere Sampling
                        {
                            for (int i = 0; i < Monte_Carlo_Itter; ++i)
                            {
                            // std::cout << "Uniform Hemisphere Sampling" << std::endl;
                                Ray SampleRay = UniformSampleHemisphere(&si);

                                Interaction siSample = this->scene.rayIntersect(SampleRay);
                                Interaction siLightSample = this->scene.rayEmitterIntersect(SampleRay);
                                if (siLightSample.didIntersect && siSample.t > siLightSample.t)
                                {
                                    total_sample += si.bsdf->eval(&si, SampleRay.d) * siLightSample.emissiveColor * std::abs(Dot(Normalize(si.n), Normalize(SampleRay.d)));
                                }
                            }
                            result += (total_sample * (2 * M_PI) * (1 /(float) Monte_Carlo_Itter));                            
                            break;
                        }
                        case 1: // Cosine Hemisphere Sampling
                        {
                            for (int i = 0; i < Monte_Carlo_Itter; ++i)
                            {
                                Ray SampleRay = UniformCosineSampleHemisphere(&si);

                                Interaction siSample = this->scene.rayIntersect(SampleRay);
                                Interaction siLightSample = this->scene.rayEmitterIntersect(SampleRay);
                                if (siLightSample.didIntersect && siSample.t > siLightSample.t)
                                {
                                    total_sample += si.bsdf->eval(&si, SampleRay.d) * siLightSample.emissiveColor;
                                }
                            }
                            result += (total_sample * M_PI * (1 /(float) Monte_Carlo_Itter));
                            break;
                        }
                        case 2: // Light Sampling
                        {
                            // itterate over all light samples
                            for (Light &light : this->scene.lights)
                            {
                                Vector3f radiance;
                                LightSample ls;

                                std::tie(radiance, ls) = light.sample(&si);

                                Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                                Interaction siShadow = this->scene.rayIntersect(shadowRay);
                                // Interaction siLightSample = this->scene.rayEmitterIntersect(shadowRay);

                                if ((!siShadow.didIntersect || siShadow.t > ls.d))
                                {
                                    total_sample += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(Normalize(si.n), Normalize(ls.wo))) * std::abs(Dot(Normalize(ls.n), Normalize(ls.wo)));
                                }
                            }
                            int num_lights = this->scene.lights.size();
                            result += (total_sample * (1 / (float) num_lights));
                            break;
                        }
                        default:
                        {
                            std::cout << "Invalid Sampling Strategy: " << samplingStrategy << std::endl;
                            exit(1);
                        }
                    }
                }
                
                total_color += result;
            }

            this->outputImage.writePixelColor(total_color * (1 /(float) spp), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    spp = atoi(argv[3]);
    std::cout << "Using Samples per pixel: " << spp << std::endl;

    samplingStrategy = atoi(argv[4]);

    switch (samplingStrategy)
    {
    case 0:
        std::cout << "Using Uniform Hemisphere Sampling" << std::endl;
        break;
    case 1:
        std::cout << "Using Cosine Hemisphere Sampling" << std::endl;
        break;
    case 2:
        std::cout << "Using Light Sampling" << std::endl;
        break;
    default:
        std::cout << "Invalid Sampling Strategy." << std::endl;
        return 1;
    }

    auto renderTime = rayTracer.render();

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}

Ray UniformSampleHemisphere(Interaction *it)
{
    // Generate a random ray in the hemisphere
    // Choose a random theta and phi
    float theta = 2 * M_PI * next_float();
    float phi = M_PI * next_float();

    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    // Convert to world coordinates
    Vector3f w = it->n;                                  // normal
    Vector3f u = Normalize(Cross(w, Vector3f(x, y, z))); // tangent
    Vector3f v = Cross(w, u);                            // bitangent

    float world_x = x * u.x + y * v.x + z * w.x;
    float world_y = x * u.y + y * v.y + z * w.y;
    float world_z = x * u.z + y * v.z + z * w.z;

    return Ray(it->p + 1e-3f * it->n, Normalize(Vector3f(world_x, world_y, world_z)));
}

Ray UniformCosineSampleHemisphere(Interaction *it)
{
    // Generate a ray in the hemisphere with a cosine distribution
    // Chooose a random point on the unit disk and project it to the hemisphere
    float theta = acos(sqrt(next_float()));
    float phi = 2 * M_PI * next_float();

    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);

    // Convert to world coordinates
    Vector3f w = it->n;                                  // normal
    Vector3f u = Normalize(Cross(w, Vector3f(x, y, z))); // tangent
    Vector3f v = Cross(w, u);                            // bitangent

    float world_x = x * u.x + y * v.x + z * w.x;
    float world_y = x * u.y + y * v.y + z * w.y;
    float world_z = x * u.z + y * v.z + z * w.z;

    return Ray(it->p + 1e-3f * it->n, Normalize(Vector3f(world_x, world_y, world_z)));
}