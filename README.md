# FirstRaytracer

# C++ Ray Tracer

A from-scratch implementation of a ray tracing renderer in C++ with no external dependencies. This project demonstrates the fundamentals of physically-based rendering, from basic ray-sphere intersection to realistic lighting and shadows.

![Ray Tracing Output](https://img.shields.io/badge/output-PPM-blue) ![C++](https://img.shields.io/badge/C%2B%2B-11-00599C?logo=cplusplus) ![License](https://img.shields.io/badge/license-MIT-green)

## 🎨 Features

- **Pure C++ Implementation** - No external libraries required
- **Progressive Rendering Pipeline** - Four distinct rendering stages showcasing ray tracing fundamentals
- **Ray-Sphere Intersection** - Efficient geometric intersection testing
- **Phong-style Lighting** - Diffuse (Lambertian) shading model
- **Shadow Casting** - Realistic shadows through shadow ray testing
- **Multiple Light Sources** - Support for colored lights with varying intensities
- **PPM Image Output** - Simple, portable image format

## 📋 Table of Contents

- [Quick Start](#-quick-start)
- [Rendering Pipeline](#-rendering-pipeline)
- [Architecture](#-architecture)
- [Usage Examples](#-usage-examples)
- [Output Gallery](#-output-gallery)
- [Technical Details](#-technical-details)
- [Customization](#-customization)
- [Future Enhancements](#-future-enhancements)

## 🚀 Quick Start

### Prerequisites

- C++ compiler with C++11 support (g++, clang++, MSVC)
- No external libraries needed!

### Compilation

```bash
# Linux/macOS
g++ -o raytracer raytracer.cpp -std=c++11 -O2

# Windows (MSVC)
cl /EHsc /O2 raytracer.cpp

# Windows (MinGW)
g++ -o raytracer.exe raytracer.cpp -std=c++11 -O2
```

### Running

```bash
./raytracer
```

This will generate four PPM images:
- `output_distance.ppm` - Distance-based visualization
- `output_materials.ppm` - Flat-shaded materials
- `output_diffuse.ppm` - Diffuse lighting
- `output_final.ppm` - Complete render with shadows

### Viewing Output

PPM files can be viewed with most image viewers. To convert to PNG:

```bash
# Using ImageMagick
convert output_final.ppm output_final.png

# Using GIMP, Photoshop, or online converters
# PPM files are also directly viewable in most browsers
```

## 🎬 Rendering Pipeline

The ray tracer implements a progressive rendering pipeline, with each stage building on the previous:

### Stage 1: Distance Rendering (Step b)
```
Ray → Intersection Test → Distance → Grayscale Color
```
Visualizes the distance from the camera to the nearest object. Closer objects appear brighter.

### Stage 2: Material Rendering (Step c)
```
Ray → Intersection Test → Material Color → Output
```
Displays objects with their assigned colors (flat shading, no lighting).

### Stage 3: Diffuse Shading (Step d)
```
Ray → Hit Point → Normal → Light Direction → N·L → Shaded Color
```
Implements Lambertian (diffuse) shading using the formula:
```
I = I_ambient + Σ(I_light × max(0, N · L))
```

### Stage 4: Shadow Rays (Step e)
```
Ray → Hit Point → Shadow Ray → Occlusion Test → Final Color
```
Casts rays toward light sources to determine if points are in shadow.

## 🏗️ Architecture

### Core Classes

#### `Vec3`
3D vector mathematics with standard operations:
- Arithmetic operators (+, -, *, /)
- Dot product
- Length and normalization
- Component-wise multiplication (for colors)

#### `Ray`
Represents a ray with origin and direction:
```cpp
Point = Origin + t × Direction
```

#### `Sphere`
Geometric primitive with:
- Center position and radius
- Material properties
- Ray-sphere intersection algorithm (quadratic formula)
- Surface normal calculation

#### `Material`
Stores surface properties:
- Diffuse color (RGB)
- Extensible for future properties (specular, roughness, etc.)

#### `Light`
Point light source with:
- Position in 3D space
- Color (RGB)
- Intensity multiplier

#### `Camera`
Virtual camera with:
- Position, look-at target, and up vector
- Field of view (FOV) in degrees
- Ray generation for each pixel

#### `Scene`
Container for all scene objects:
- Collection of spheres
- Collection of lights
- Ambient light color
- Intersection testing
- Shadow ray testing

#### `Image`
Framebuffer with:
- Pixel storage (RGB floating point)
- PPM file export
- Pixel access methods

#### `Renderer`
Static rendering methods for each pipeline stage:
- `renderDistance()` - Step b
- `renderMaterials()` - Step c
- `renderDiffuse()` - Step d
- `renderWithShadows()` - Step e

## 💡 Usage Examples

### Creating a Custom Scene

```cpp
// Initialize image and camera
Image img(1920, 1080);
Camera camera(Vec3(0, 2, 10), Vec3(0, 0, 0), Vec3(0, 1, 0), 45);

// Create scene
Scene scene;

// Add objects
scene.addSphere(Sphere(
    Vec3(0, 0, 0),              // Center
    1.5,                         // Radius
    Material(Vec3(1, 0, 0))     // Red material
));

// Add lights
scene.addLight(Light(
    Vec3(10, 10, 10),           // Position
    Vec3(1, 1, 1),              // White light
    1.0                          // Full intensity
));

// Render
Renderer::renderWithShadows(img, camera, scene);
img.savePPM("my_render.ppm");
```

### Adjusting Camera

```cpp
// Wide angle shot
Camera wideAngle(Vec3(0, 5, 15), Vec3(0, 0, 0), Vec3(0, 1, 0), 90);

// Telephoto shot
Camera telephoto(Vec3(0, 0, 20), Vec3(0, 0, 0), Vec3(0, 1, 0), 30);

// Top-down view
Camera topDown(Vec3(0, 20, 0), Vec3(0, 0, 0), Vec3(0, 0, -1), 60);
```

### Creating Different Materials

```cpp
Material red(Vec3(1.0, 0.2, 0.2));      // Bright red
Material green(Vec3(0.2, 1.0, 0.2));    // Bright green
Material blue(Vec3(0.2, 0.2, 1.0));     // Bright blue
Material white(Vec3(1.0, 1.0, 1.0));    // White
Material gray(Vec3(0.5, 0.5, 0.5));     // 50% gray
```

### Lighting Setups

```cpp
// Three-point lighting
scene.addLight(Light(Vec3(5, 5, 5), Vec3(1, 1, 1), 1.0));      // Key light
scene.addLight(Light(Vec3(-5, 3, 5), Vec3(0.8, 0.9, 1), 0.5)); // Fill light
scene.addLight(Light(Vec3(0, 0, 10), Vec3(1, 0.9, 0.8), 0.3)); // Back light

// Colored mood lighting
scene.addLight(Light(Vec3(10, 5, 0), Vec3(1, 0.5, 0), 0.8));   // Orange
scene.addLight(Light(Vec3(-10, 5, 0), Vec3(0, 0.5, 1), 0.6));  // Blue
```

## 🖼️ Output Gallery

The default scene produces the following outputs:

**Distance Rendering** - Objects encoded by depth
- Bright = Close to camera
- Dark = Far from camera

**Material Rendering** - Flat colored objects
- Pure material colors
- No lighting or shadows

**Diffuse Shading** - Lambertian lighting model
- Surfaces brighten when facing lights
- Ambient light prevents pure black

**Final Render with Shadows** - Complete lighting
- Realistic shadow casting
- Occlusion testing
- Multiple light sources

## 🔬 Technical Details

### Ray-Sphere Intersection

Uses the analytical solution to the quadratic equation:

```
(O + tD - C) · (O + tD - C) = r²

Expanding:
at² + bt + c = 0

Where:
a = D · D
b = 2(O - C) · D
c = (O - C) · (O - C) - r²

Solutions:
t = (-b ± √(b² - 4ac)) / 2a
```

### Diffuse Lighting (Lambertian)

```cpp
// Surface normal
Vec3 N = (hitPoint - sphereCenter).normalize();

// Direction to light
Vec3 L = (lightPos - hitPoint).normalize();

// Diffuse intensity
float intensity = max(0.0, N · L);

// Final color
Color = ambient + material × lightColor × intensity
```

### Shadow Ray Testing

```cpp
// Create ray from surface point to light
Vec3 shadowRayDir = (lightPos - hitPoint).normalize();
Ray shadowRay(hitPoint + ε × normal, shadowRayDir);

// Check for occlusion
if (scene.intersect(shadowRay) && t < distToLight) {
    // Point is in shadow - use only ambient light
}
```

The small epsilon offset (`ε`) prevents self-intersection artifacts.

### Coordinate System

- **X-axis**: Right
- **Y-axis**: Up  
- **Z-axis**: Forward (toward camera)
- Right-handed coordinate system

## 🎨 Customization

### Changing Image Resolution

```cpp
const int width = 1920;   // Default: 800
const int height = 1080;  // Default: 600
```

Higher resolutions produce better quality but take longer to render.

### Adjusting Ambient Light

```cpp
scene.ambientLight = Vec3(0.05, 0.05, 0.05);  // Darker ambient
scene.ambientLight = Vec3(0.3, 0.3, 0.3);     // Brighter ambient
```

### Creating Complex Scenes

```cpp
// Create a ring of spheres
for (int i = 0; i < 8; i++) {
    double angle = i * 2.0 * M_PI / 8.0;
    Vec3 pos(cos(angle) * 5, 0, sin(angle) * 5);
    scene.addSphere(Sphere(pos, 0.8, Material(Vec3(
        (i % 3 == 0) ? 1.0 : 0.3,
        (i % 3 == 1) ? 1.0 : 0.3,
        (i % 3 == 2) ? 1.0 : 0.3
    ))));
}
```

## 🚧 Future Enhancements

Potential features to add:

- [ ] **Specular highlights** - Phong/Blinn-Phong shading
- [ ] **Reflections** - Recursive ray tracing for mirrors
- [ ] **Refractions** - Glass and transparency
- [ ] **Anti-aliasing** - Multi-sampling for smoother edges
- [ ] **Additional primitives** - Planes, triangles, meshes
- [ ] **Textures** - UV mapping and image textures
- [ ] **Area lights** - Soft shadows
- [ ] **Global illumination** - Path tracing for realistic lighting
- [ ] **Acceleration structures** - BVH for complex scenes
- [ ] **Parallel rendering** - Multi-threading support
- [ ] **PNG/JPEG export** - Using stb_image_write
- [ ] **Interactive preview** - Real-time rendering with OpenGL

## 📚 Learning Resources

If you're new to ray tracing, check out these resources:

- **Ray Tracing in One Weekend** by Peter Shirley - Excellent introduction
- **Physically Based Rendering** by Pharr, Jakob & Humphreys - Advanced techniques
- **Scratchapixel** - Free online ray tracing tutorials
- **PBRT Book** - Comprehensive rendering reference

## 📝 License

This project is released under the MIT License. Feel free to use, modify, and distribute.

## 🤝 Contributing

Contributions are welcome! Some ideas:
- Add new geometric primitives
- Implement new shading models
- Optimize rendering performance
- Add more example scenes
- Improve documentation

## ⚡ Performance Notes

Current performance (800×600 resolution):
- ~0.1s per frame (distance/material stages)
- ~0.5s per frame (lighting stages)
- Linear complexity: O(pixels × spheres × lights)

For larger scenes, consider implementing:
- Spatial acceleration (BVH, k-d tree)
- Multi-threading
- SIMD optimizations

## 🐛 Known Issues

- Shadow acne can occur with very small epsilon values
- No acceleration structure (slow for many objects)
- Limited to sphere primitives
- PPM format produces large files

## 📧 Contact

Questions? Suggestions? Open an issue or submit a pull request!

---

**Happy Ray Tracing! 🎨✨**
