#include "light.h"

Light::Light(LightType type, nlohmann::json config)
{
    switch (type)
    {
    case LightType::POINT_LIGHT:
        this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
        break;
    case LightType::DIRECTIONAL_LIGHT:
        this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
        break;
    case LightType::AREA_LIGHT:
        this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
        this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
        this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
        this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
        break;
    default:
        std::cout << "WARNING: Invalid light type detected";
        break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

// returns the radiance and the light sample from the light source given the interaction point
std::pair<Vector3f, LightSample> Light::sample(Interaction *si)
{
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type)
    {
    case LightType::POINT_LIGHT:
        ls.wo = (position - si->p);
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        radiance = (1.f / (ls.d * ls.d)) * this->radiance;
        break;
    case LightType::DIRECTIONAL_LIGHT:
        ls.wo = Normalize(direction);
        ls.d = 1e10;
        radiance = this->radiance;
        break;
    case LightType::AREA_LIGHT:
        // break;
        Vector3f rnd_point = center + (2 * next_float() - 1) * vx + (2 * next_float() - 1) * vy;
        ls.wo = rnd_point - si->p;
        ls.d = ls.wo.Length();
        ls.wo = Normalize(ls.wo);
        float area = Cross(vx, vy).Length() * 4;
        // bool isShadow = false;
        // Check if the point is in shadow
        if (Dot(normal, ls.wo) > 0)
        {
            // isShadow = true;
            ls.d = 1e30;
        }

        radiance = this->radiance * area * (1 / (ls.d * ls.d)); 
        ls.area = area;
        ls.p = rnd_point;
        ls.n = Normalize(normal);
        break;
    }
    return {radiance, ls};
}

Interaction Light::intersectLight(Ray *ray)
{
    /*
    Check if the ray intersects the light source and return the interaction
    */
    Interaction si;
    memset(&si, 0, sizeof(si));
    switch (type)
    {
    case AREA_LIGHT:
    { 
        // Check if the ray intersects the area light
        // First check if the ray intersects the plane of the area light and
        // then check if the point of intersection lies inside the area light
        float t = Dot((center - ray->o), normal) / Dot(ray->d, normal);
        if (t < 0)
        {
            // std::cout << "WARNING: Invalid intersection detected" << std::endl;
            si.didIntersect = false;
            break;
        }
        Vector3f p = ray->o + t * ray->d;
        Vector3f d = p - center;
        float dx = Dot(d, Normalize(vx));
        float dy = Dot(d, Normalize(vy));
        if (dx >= -vx.Length() && dx <= vx.Length() && dy >= -vy.Length() && dy <= vy.Length())
        {
            si.didIntersect = true;
            si.t = t;
        }
        else
        {
            si.didIntersect = false;
        }
        float area = Cross(vx, vy).Length() * 4;
        si.emissiveColor = radiance;
        break;
    }
    case POINT_LIGHT:
    {
        int x_scale = (position - ray->o)[0] / ray->d[0];
        int y_scale = (position - ray->o)[1] / ray->d[1];
        int z_scale = (position - ray->o)[2] / ray->d[2];
        if (x_scale == y_scale && y_scale == z_scale)
        {
            si.didIntersect = true;
            si.t = x_scale;
            si.p = ray->o + si.t * ray->d;
            si.n = Normalize(si.p - position);
        }
        else
        {
            si.didIntersect = false;
        }
        si.emissiveColor = radiance * (1.f / (si.t * si.t));
        break;
    }
    case DIRECTIONAL_LIGHT:
    {
        if (ray->d == direction)
        {
            si.didIntersect = true;
            si.t = 1e10;
            si.p = ray->o + si.t * ray->d;
            si.n = -direction;
        }
        else
        {
            si.didIntersect = false;
        }
        si.emissiveColor = radiance;
        break;
    }
    default:
    {
        std::cout << "WARNING: Invalid light type detected" << std::endl;
        exit(1);
    }
    }
    return si;
}