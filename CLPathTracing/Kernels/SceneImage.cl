__constant float EPSILON = 0.00003f; //Compensate limited float precision
__constant float PI = 3.14159265359f;

typedef struct Ray {
    float3 origin;
    float3 direction;
} Ray;

typedef struct Sphere {
    float radius;
    float3 position;
    float3 color;
    float3 emission;
} Sphere;

static float getRandom(unsigned int *seed0, unsigned int *seed1) {
    //hash the seeds using bitwise AND operations and bitshifts
    *seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);
    *seed1 = 18000 * ((*seed0) & 65535) + ((*seed0) >> 16);

    unsigned int ires = ((*seed0) << 16) + (*seed1);

    union {
        float f;
        unsigned int ui;
    } res;

    res.ui = (ires & 0x007fffff) | 0x40000000; //Bitwise AND -> OR
    return (res.f - 2.0f) / 2.0f;
}

Ray createCameraRay(const int x, const int y, const int width, const int height)
{
    //Convert int to float in a normalized range [0..1]
    float nX = (float)x / (float)width;
    float nY = (float)y / (float)height;

    //Calculate image aspect ratio
    float aspectRatio = (float)width / (float)height;

    //Calculate pixel position on screen
    float3 pixelPosition = (float3)((nX - 0.5f) * aspectRatio, -(nY - 0.5f), 0.0f);

    //SHOOT THE RAY !
    Ray ray;
    ray.origin = (float3)(0.0f, 0.2f, 2.0f); //Fixed camera position
    ray.direction = normalize(pixelPosition - ray.origin);

    return ray;
}

float intersectSphere(const Sphere* sphere, const Ray* ray) {
    float3 rayToCenter = sphere->position - ray->origin;
    float b = dot(rayToCenter, ray->direction);
    float c = dot(rayToCenter, rayToCenter) - sphere->radius * sphere->radius;
    float discriminant = b * b - c;

    if (discriminant < 0.0f) return 0.0f;
    else discriminant = sqrt(discriminant);

    if ((b - discriminant) > EPSILON) return b - discriminant;
    if ((b + discriminant) > EPSILON) return b + discriminant;

    return 0.0f;
}

bool intersectScene(__constant Sphere* spheres, const Ray* ray, float* t, int* sphere_id, const int sphereCount) {
    float TURBOMEGABIGNUMBERWOW = 1e20f;
    *t = TURBOMEGABIGNUMBERWOW;

    for (int i = 0; i < sphereCount;  i++) {
        Sphere sphere = spheres[i]; //Create a local copy of a sphere

        float hitDistance = intersectSphere(&sphere, ray);

        if (hitDistance != 0.0f && hitDistance < *t) {
            *t = hitDistance;
            *sphere_id = i;
        }
    }

    return *t < TURBOMEGABIGNUMBERWOW; //Then return true of ray interstects the scene
}

float3 pathTrace(__constant Sphere* spheres, const Ray* cameraRay, const int bounceCount, const int sphereCount, const int* s0, const int* s1, const int width) {
    Ray ray = *cameraRay;

    float3 accumulatedColor = (float3)(0.0f, 0.0f, 0.0f);
    float3 maskColor = (float3)(1.0f, 1.0f, 1.0f);

    for (int bounces = 0; bounces < bounceCount; bounces++) {
        float t; //Distance to intersection
        int hitSphereID = 0; //Index of the intersected sphere

        if (!intersectScene(spheres, &ray, &t, &hitSphereID, sphereCount)) {
            return accumulatedColor += maskColor * (float3)(0.15f, 0.15f, 0.25f);
        }

        Sphere hitSphere = spheres[hitSphereID];

        //compute the hit point using the ray equation
        float3 hitPoint = ray.origin + ray.direction * t;

        //Compute the surface normal and flip it if necessary to face the incoming ray
        float3 normal = normalize(hitPoint - hitSphere.position);
        float3 normalFacing = dot(normal, ray.direction) < 0.0f ? normal : normal * (-1.0f);

        float random1 = 2.0f * PI * getRandom(s0, s1);
        float random2 = getRandom(s0, s1);
        float random2s = sqrt(random2);

        //Create a local orthogonal coordinate frame centered at the hit point
        float3 w = normalFacing;
        float3 axis = fabs(w.x) > 0.1f ? (float3)(0.0f, 1.0f, 0.0f) : (float3)(1.0f, 0.0f, 0.0f);
        float3 u = normalize(cross(axis, w));
        float3 v = cross(w, u);

        //Use the coordinate frame and random numbers to compute the next ray direction
        float3 nextDirection = normalize(u*cos(random1)*random2s + v*sin(random1)*random2s + w*sqrt(1.0f - random2));

        ray.origin = hitPoint + normalFacing * EPSILON; //Prevent self intersection
        ray.direction = nextDirection;

        accumulatedColor += maskColor * hitSphere.emission / 2.5f;
        maskColor *= hitSphere.color * 2.5f; //Mask color picks up the surface color at each bounce
        maskColor *= dot(nextDirection, normalFacing); //Cosine-weighted importance sampling for diffuse surfaces
    }

    return accumulatedColor;
}

union Color{ float c; uchar4 components; };

__kernel void main(__constant Sphere* spheres, const int width, const int height, const int sphereCount, const int bounceCount, const int sampleCount, __global float3* output, const int hxFrameNumber) {
    unsigned int workItemId = get_global_id(0);
    unsigned int x_coord = workItemId % width;
    unsigned int y_coord = workItemId / width;

    unsigned int seed0 = x_coord + hxFrameNumber;
    unsigned int seed1 = y_coord + hxFrameNumber;

    Ray primaryRay = createCameraRay(x_coord, y_coord, width, height);

    float3 finalColor = (float3)(0.0f, 0.0f, 0.0f);
    float inversedSamples = 1.0f / sampleCount;

    for (int i = 0; i < sampleCount; i++) {
        finalColor += pathTrace(spheres, &primaryRay, bounceCount, sphereCount, &seed0, &seed1, width) * inversedSamples;
    }

    finalColor = (float3)(clamp(finalColor.x, 0.0f, 1.0f), clamp(finalColor.y, 0.0f, 1.0f), clamp(finalColor.z, 0.0f, 1.0f));

    union Color fColor;
    fColor.components = (uchar4)(
        (unsigned char)(finalColor.x * 255),
        (unsigned char)(finalColor.y * 255),
        (unsigned char)(finalColor.z * 255),
    1);
    
    output[workItemId] = (float3)(x_coord, y_coord, fColor.c);
}