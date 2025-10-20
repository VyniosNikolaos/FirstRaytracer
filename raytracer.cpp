#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

// ====================================
// Vector3 Class - Basic 3D vector math
// ====================================
class Vec3 {
public:
    double x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(double t) const { return Vec3(x * t, y * t, z * t); }
    Vec3 operator/(double t) const { return Vec3(x / t, y / t, z / t); }
    
    // Component-wise multiplication (for colors)
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    
    double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    
    double length() const { return std::sqrt(x * x + y * y + z * z); }
    
    Vec3 normalize() const {
        double len = length();
        return len > 0 ? (*this) / len : Vec3(0, 0, 0);
    }
};

// =========
// Ray Class
// =========
class Ray {
public:
    Vec3 origin;
    Vec3 direction;
    
    Ray(const Vec3& origin, const Vec3& direction) 
        : origin(origin), direction(direction.normalize()) {}
    
    Vec3 at(double t) const { return origin + direction * t; }
};

// ==============
// Material Class
// ==============
class Material {
public:
    Vec3 color;
    
    Material() : color(1, 1, 1) {}
    Material(const Vec3& color) : color(color) {}
};

// ============
// Sphere Class
// ============
class Sphere {
public:
    Vec3 center;
    double radius;
    Material material;
    
    Sphere(const Vec3& center, double radius, const Material& material)
        : center(center), radius(radius), material(material) {}
    
    // Ray-sphere intersection
    // Returns true if intersection exists, fills t with closest intersection distance
    bool intersect(const Ray& ray, double& t, double tMin = 0.001, double tMax = std::numeric_limits<double>::infinity()) const {
        Vec3 oc = ray.origin - center;
        double a = ray.direction.dot(ray.direction);
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;
        
        if (discriminant < 0) return false;
        
        double sqrtd = std::sqrt(discriminant);
        double t0 = (-b - sqrtd) / (2.0 * a);
        double t1 = (-b + sqrtd) / (2.0 * a);
        
        // Find the closest valid intersection
        if (t0 >= tMin && t0 <= tMax) {
            t = t0;
            return true;
        }
        if (t1 >= tMin && t1 <= tMax) {
            t = t1;
            return true;
        }
        
        return false;
    }
    
    Vec3 getNormal(const Vec3& point) const {
        return (point - center).normalize();
    }
};

// ===========
// Light Class
// ===========
class Light {
public:
    Vec3 position;
    Vec3 color;
    double intensity;
    
    Light(const Vec3& position, const Vec3& color = Vec3(1, 1, 1), double intensity = 1.0)
        : position(position), color(color), intensity(intensity) {}
};

// ===========
// Scene Class
// ===========
class Scene {
public:
    std::vector<Sphere> spheres;
    std::vector<Light> lights;
    Vec3 ambientLight;
    
    Scene() : ambientLight(0.1, 0.1, 0.1) {}
    
    void addSphere(const Sphere& sphere) { spheres.push_back(sphere); }
    void addLight(const Light& light) { lights.push_back(light); }
    
    // Find closest intersection with any sphere
    bool intersect(const Ray& ray, double& tClosest, int& sphereIndex, double tMin = 0.001) const {
        tClosest = std::numeric_limits<double>::infinity();
        sphereIndex = -1;
        
        for (size_t i = 0; i < spheres.size(); ++i) {
            double t;
            if (spheres[i].intersect(ray, t, tMin, tClosest)) {
                tClosest = t;
                sphereIndex = i;
            }
        }
        
        return sphereIndex != -1;
    }
    
    // Check if point is in shadow
    bool isInShadow(const Vec3& point, const Vec3& lightPos) const {
        Vec3 toLight = lightPos - point;
        double distToLight = toLight.length();
        Ray shadowRay(point, toLight);
        
        double t;
        int sphereIdx;
        // Check if any object blocks the light
        if (intersect(shadowRay, t, sphereIdx, 0.001)) {
            // If intersection is closer than light, we're in shadow
            return t < distToLight;
        }
        
        return false;
    }
};

// ============
// Camera Class
// ============
class Camera {
public:
    Vec3 position;
    Vec3 lookAt;
    Vec3 up;
    double fov; // Field of view in degrees
    
    Camera(const Vec3& position, const Vec3& lookAt, const Vec3& up, double fov)
        : position(position), lookAt(lookAt), up(up), fov(fov) {}
    
    Ray getRay(double u, double v, int width, int height) const {
        // Calculate camera basis vectors
        Vec3 forward = (lookAt - position).normalize();
        Vec3 right = Vec3(forward.y * up.z - forward.z * up.y,
                          forward.z * up.x - forward.x * up.z,
                          forward.x * up.y - forward.y * up.x).normalize(); // cross product
        Vec3 upVec = Vec3(right.y * forward.z - right.z * forward.y,
                          right.z * forward.x - right.x * forward.z,
                          right.x * forward.y - right.y * forward.x); // cross product
        
        double aspectRatio = (double)width / height;
        double scale = std::tan(fov * 0.5 * M_PI / 180.0);
        
        // Map pixel coordinates to [-1, 1] range
        double x = (2.0 * u / width - 1.0) * aspectRatio * scale;
        double y = (1.0 - 2.0 * v / height) * scale;
        
        Vec3 direction = (forward + right * x + upVec * y).normalize();
        return Ray(position, direction);
    }
};

// ===========
// Image Class
// ===========
class Image {
public:
    int width, height;
    std::vector<Vec3> pixels;
    
    Image(int width, int height) : width(width), height(height) {
        pixels.resize(width * height);
    }
    
    void setPixel(int x, int y, const Vec3& color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * width + x] = color;
        }
    }
    
    Vec3 getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return pixels[y * width + x];
        }
        return Vec3(0, 0, 0);
    }
    
    // Save as PPM format (simple, no library needed)
    void savePPM(const std::string& filename) const {
        std::ofstream file(filename);
        file << "P3\n" << width << " " << height << "\n255\n";
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vec3 color = getPixel(x, y);
                // Clamp values to [0, 1] and convert to [0, 255]
                int r = std::min(255, std::max(0, (int)(color.x * 255)));
                int g = std::min(255, std::max(0, (int)(color.y * 255)));
                int b = std::min(255, std::max(0, (int)(color.z * 255)));
                file << r << " " << g << " " << b << " ";
            }
            file << "\n";
        }
        
        file.close();
    }
};

// ==============
// Renderer Class
// ==============
class Renderer {
public:
    // Step b: Render distance to closest sphere
    static void renderDistance(Image& img, const Camera& camera, const Scene& scene) {
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                Ray ray = camera.getRay(x, y, img.width, img.height);
                
                double t;
                int sphereIdx;
                if (scene.intersect(ray, t, sphereIdx)) {
                    // Encode distance as color (normalize to reasonable range)
                    double normalizedDist = 1.0 - std::min(1.0, t / 20.0);
                    Vec3 color(normalizedDist, normalizedDist, normalizedDist);
                    img.setPixel(x, y, color);
                } else {
                    // Background color (sky blue)
                    img.setPixel(x, y, Vec3(0.5, 0.7, 1.0));
                }
            }
        }
    }
    
    // Step c: Render with material colors
    static void renderMaterials(Image& img, const Camera& camera, const Scene& scene) {
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                Ray ray = camera.getRay(x, y, img.width, img.height);
                
                double t;
                int sphereIdx;
                if (scene.intersect(ray, t, sphereIdx)) {
                    Vec3 color = scene.spheres[sphereIdx].material.color;
                    img.setPixel(x, y, color);
                } else {
                    img.setPixel(x, y, Vec3(0.5, 0.7, 1.0));
                }
            }
        }
    }
    
    // Step d: Render with basic diffuse shading (N · L)
    static void renderDiffuse(Image& img, const Camera& camera, const Scene& scene) {
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                Ray ray = camera.getRay(x, y, img.width, img.height);
                
                double t;
                int sphereIdx;
                if (scene.intersect(ray, t, sphereIdx)) {
                    Vec3 hitPoint = ray.at(t);
                    Vec3 normal = scene.spheres[sphereIdx].getNormal(hitPoint);
                    Vec3 materialColor = scene.spheres[sphereIdx].material.color;
                    
                    Vec3 finalColor = scene.ambientLight * materialColor;
                    
                    // Add contribution from each light
                    for (const Light& light : scene.lights) {
                        Vec3 toLight = (light.position - hitPoint).normalize();
                        double diffuse = std::max(0.0, normal.dot(toLight));
                        finalColor = finalColor + materialColor * light.color * (diffuse * light.intensity);
                    }
                    
                    img.setPixel(x, y, finalColor);
                } else {
                    img.setPixel(x, y, Vec3(0.5, 0.7, 1.0));
                }
            }
        }
    }
    
    // Step e: Render with shadows
    static void renderWithShadows(Image& img, const Camera& camera, const Scene& scene) {
        for (int y = 0; y < img.height; ++y) {
            for (int x = 0; x < img.width; ++x) {
                Ray ray = camera.getRay(x, y, img.width, img.height);
                
                double t;
                int sphereIdx;
                if (scene.intersect(ray, t, sphereIdx)) {
                    Vec3 hitPoint = ray.at(t);
                    Vec3 normal = scene.spheres[sphereIdx].getNormal(hitPoint);
                    Vec3 materialColor = scene.spheres[sphereIdx].material.color;
                    
                    // Start with ambient light
                    Vec3 finalColor = scene.ambientLight * materialColor;
                    
                    // Add contribution from each light (if not in shadow)
                    for (const Light& light : scene.lights) {
                        if (!scene.isInShadow(hitPoint, light.position)) {
                            Vec3 toLight = (light.position - hitPoint).normalize();
                            double diffuse = std::max(0.0, normal.dot(toLight));
                            finalColor = finalColor + materialColor * light.color * (diffuse * light.intensity);
                        }
                    }
                    
                    img.setPixel(x, y, finalColor);
                } else {
                    img.setPixel(x, y, Vec3(0.5, 0.7, 1.0));
                }
            }
        }
    }
};

// ============
// Main Program
// ============
int main() {
    // Image settings
    const int width = 800;
    const int height = 600;
    
    // Step a: Create image
    Image img(width, height);
    
    // Setup camera
    Camera camera(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0), 60);
    
    // Setup scene
    Scene scene;
    
    // Add spheres with different materials
    scene.addSphere(Sphere(Vec3(0, 0, 0), 1.0, Material(Vec3(1.0, 0.3, 0.3)))); // Red
    scene.addSphere(Sphere(Vec3(-2.5, 0, -1), 1.0, Material(Vec3(0.3, 1.0, 0.3)))); // Green
    scene.addSphere(Sphere(Vec3(2.5, 0, -1), 1.0, Material(Vec3(0.3, 0.3, 1.0)))); // Blue
    scene.addSphere(Sphere(Vec3(0, -101, 0), 100, Material(Vec3(0.8, 0.8, 0.8)))); // Ground
    
    // Add lights
    scene.addLight(Light(Vec3(5, 5, 5), Vec3(1, 1, 1), 0.8));
    scene.addLight(Light(Vec3(-5, 3, 3), Vec3(1, 0.9, 0.8), 0.4));
    
    std::cout << "Rendering images..." << std::endl;
    
    // Step b: Render distance
    std::cout << "  Step b: Distance rendering..." << std::endl;
    Renderer::renderDistance(img, camera, scene);
    img.savePPM("output_distance.ppm");
    
    // Step c: Render materials
    std::cout << "  Step c: Material rendering..." << std::endl;
    Renderer::renderMaterials(img, camera, scene);
    img.savePPM("output_materials.ppm");
    
    // Step d: Render with diffuse shading
    std::cout << "  Step d: Diffuse shading..." << std::endl;
    Renderer::renderDiffuse(img, camera, scene);
    img.savePPM("output_diffuse.ppm");
    
    // Step e: Render with shadows
    std::cout << "  Step e: Rendering with shadows..." << std::endl;
    Renderer::renderWithShadows(img, camera, scene);
    img.savePPM("output_final.ppm");
    
    std::cout << "Done! Generated images:" << std::endl;
    std::cout << "  - output_distance.ppm (step b)" << std::endl;
    std::cout << "  - output_materials.ppm (step c)" << std::endl;
    std::cout << "  - output_diffuse.ppm (step d)" << std::endl;
    std::cout << "  - output_final.ppm (step e)" << std::endl;
    
    return 0;
}
