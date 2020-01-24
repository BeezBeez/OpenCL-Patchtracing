struct Ray {
    float3 origin;
    float3 direction;
};

struct Sphere {
    float radius;
    float3 position;
    float3 color;
    float3 emission;
};

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
    struct Ray ray;
    ray.origin = (float3)(0.0f, 0.0f, 40.0f); //Fixed camera position
    ray.direction = normalize(pixelPosition - ray.origin);

    return ray;
}

bool intersectSphere(const struct Sphere* sphere, const struct Ray* ray, float* t)
{
    float3 rayToCenter = sphere->position - ray->origin;

    //float a = dot(ray->dir, ray-dir); //ray direction is normalized, dot product then simplifies to 1
    float b = dot(rayToCenter, ray->direction);
    float c = dot(rayToCenter, rayToCenter) - sphere->radius * sphere->radius;
    float discriminant = b*b - c;

    //Solve distance to hit point along ray
    if (discriminant < 0.0f) return false;
    else *t = b - sqrt(discriminant);

    if (*t < 0.0f) {
        *t = b + sqrt(discriminant);
        if (*t < 0.0f) return false;
    } else return true;

}

__kernel void main(__global float3* output, int width, int height, int bufferID)
{
    const int work_item_id = get_global_id(0); //The unique global if of the work item
    int x = work_item_id % width; //x-coord of the pixel
    int y = work_item_id / width; //y-coord of the pixel

    float nY = (float)y / (float)height;

    struct Ray primaryRay = createCameraRay(x, y, width, height);

    struct Sphere mainSphere;
    mainSphere.radius = 0.25f;
    mainSphere.position = (float3)(0.0f, 0.0f, 3.0f);
    mainSphere.color = (float3)(0.0f, 0.35f, 1.0f);

    float t = 1e20;
    intersectSphere(&mainSphere, &primaryRay, &t);

    //If ray misses sphere, return background color
    if (t > 1e19) {
        if (bufferID == 1) output[work_item_id] = (float3)(.0f, .0f, .0f);
        else output[work_item_id] = (float3)(nY * 0.8f, nY * 0.2f, 0.05f);
        return;
    }
	
    float3 hitPoint = primaryRay.origin + primaryRay.direction * t;
    float3 normal = normalize(hitPoint - mainSphere.position);
    float cosine_factor = dot(normal, primaryRay.direction) * -1.0f;

    if (bufferID == 1) output[work_item_id] = normal * 0.5f + (float3)(.5f, .5f, .5f);
    else output[work_item_id] = mainSphere.color * cosine_factor;
}