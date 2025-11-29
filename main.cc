#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include "Canvas.h"

Canvas canvas(800, 800); // global window creation

// Animation parameters
struct TreeParams {
    double lambda;      // length reduction factor
    double angle;           // branch angle from trunk
    double factor;          // branch position along trunk
    double rotationSpeed;   // rotation speed
    int numBranches;        // number of branches per node
};

// Color structure
struct Color {
    float r, g, b;
    Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}
};

// 3D vector helper
struct Vec3 {
    double x, y, z;
    Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

// Normalize vector
Vec3 normalize(const Vec3& v) {
    double len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 0.0001) return Vec3(0, 1, 0);
    return Vec3(v.x / len, v.y / len, v.z / len);
}

// Interpolate between two colors
Color lerpColor(const Color& c1, const Color& c2, float t) {
    return Color(
        c1.r + (c2.r - c1.r) * t,
        c1.g + (c2.g - c1.g) * t,
        c1.b + (c2.b - c1.b) * t
    );
}

// Get color based on depth (brown to green gradient)
Color getColorForDepth(int currentDepth, int maxDepth) {
    // Brown for trunk (deeper levels)
    Color brown(0.55f, 0.27f, 0.07f);
    // Green for tips (shallow levels)
    Color green(0.13f, 0.55f, 0.13f);
    
    // Calculate interpolation factor (0 = trunk, 1 = tips)
    float t = 1.0f - (float)currentDepth / (float)maxDepth;
    
    return lerpColor(brown, green, t);
}

// Calculate number of branches based on depth (more at bottom, fewer at top)
int getBranchCountForDepth(int currentDepth, int maxDepth, int maxBranches) {
    // At trunk (maxDepth): use maxBranches
    // At tips (depth 1): use 2 or 3 branches
    int minBranches = 2;
    
    // Linear interpolation based on depth
    float t = (float)currentDepth / (float)maxDepth;
    int branches = minBranches + (int)((maxBranches - minBranches) * t);
    
    // Ensure at least minBranches
    if (branches < minBranches) branches = minBranches;
    
    return branches;
}

// Draw a branch in 3D with color
void drawBranch(double x1, double y1, double z1, 
                double x2, double y2, double z2, 
                double length, const Color& color)
{
    canvas.SetLineWidth(int(0.03 * length + 1));
    canvas.SetColorRGB(color.r, color.g, color.b);
    canvas.Line3D(x1, y1, z1, x2, y2, z2);
}

// Recursive 3D tree generation with variable branches and colors
void generateTree3D(double x, double y, double z,
                    double dirX, double dirY, double dirZ,
                    double length, int depth, const TreeParams& params,
                    int maxDepth)
{
    if (depth <= 0 || length < 0.5) return;
    
    // Get color for current depth
    Color branchColor = getColorForDepth(depth, maxDepth);
    
    // Calculate end point of current branch
    double x2 = x + dirX * length;
    double y2 = y + dirY * length;
    double z2 = z + dirZ * length;
    
    // Draw current branch with color
    drawBranch(x, y, z, x2, y2, z2, length, branchColor);
    
    // Calculate branch point position along trunk
    double branchX = x + dirX * length * params.factor;
    double branchY = y + dirY * length * params.factor;
    double branchZ = z + dirZ * length * params.factor;
    
    // Determine number of branches for this depth level
    int numBranches = getBranchCountForDepth(depth, maxDepth, params.numBranches);
    
    // Create branches around the trunk
    for (int i = 0; i < numBranches; i++) {
        double rotAngle = (2.0 * M_PI * i) / numBranches;
        
        // Calculate perpendicular vectors for branch direction
        Vec3 dir(dirX, dirY, dirZ);
        Vec3 up(0, 1, 0);
        
        // If direction is too close to up, use different reference
        if (fabs(dirY) > 0.99) {
            up = Vec3(1, 0, 0);
        }
        
        // Create perpendicular vector (cross product)
        Vec3 perp1(
            dir.y * up.z - dir.z * up.y,
            dir.z * up.x - dir.x * up.z,
            dir.x * up.y - dir.y * up.x
        );
        perp1 = normalize(perp1);
        
        // Create second perpendicular (cross product of dir and perp1)
        Vec3 perp2(
            dir.y * perp1.z - dir.z * perp1.y,
            dir.z * perp1.x - dir.x * perp1.z,
            dir.x * perp1.y - dir.y * perp1.x
        );
        perp2 = normalize(perp2);
        
        // Rotate around the trunk to get branch direction
        double cosRot = cos(rotAngle);
        double sinRot = sin(rotAngle);
        Vec3 radial(
            perp1.x * cosRot + perp2.x * sinRot,
            perp1.y * cosRot + perp2.y * sinRot,
            perp1.z * cosRot + perp2.z * sinRot
        );
        
        // Branch direction: mix of upward (trunk direction) and outward (radial)
        double branchAngle = params.angle * M_PI / 180.0;
        Vec3 branchDir(
            dirX * cos(branchAngle) + radial.x * sin(branchAngle),
            dirY * cos(branchAngle) + radial.y * sin(branchAngle),
            dirZ * cos(branchAngle) + radial.z * sin(branchAngle)
        );
        branchDir = normalize(branchDir);
        
        // Recursively generate sub-branches
        generateTree3D(branchX, branchY, branchZ,
                      branchDir.x, branchDir.y, branchDir.z,
                      length * params.lambda, depth - 1, params, maxDepth);
    }
}

void drawTree(int maxDepth, const TreeParams& params, double rotation)
{
    canvas.ClearLines();
    canvas.SetRotation(20.0, rotation); // Tilt view and rotate
    
    // Start tree at origin, growing upward (positive Y)
    generateTree3D(0.0, -80.0, 0.0,  // start position
                   0.0, 1.0, 0.0,    // direction (upward)
                   60.0,              // initial length
                   maxDepth,          // depth
                   params,            // parameters
                   maxDepth);         // max depth
}

int main()
{
    // Default balanced tree parameters
    int maxDepth = 7;
    TreeParams baseParams;
    baseParams.lambda = 0.65;       // Length reduction per level
    baseParams.angle = 35.0;        // Branch angle from trunk (degrees)
    baseParams.factor = 0.7;        // Where along trunk branches emerge
    baseParams.rotationSpeed = 0.5; // Initial rotation speed
    baseParams.numBranches = 5;     // Maximum branches at trunk level
    
    std::cout << "=== Living 3D Recursive Tree ===" << std::endl;
    std::cout << "Depth: " << maxDepth << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "  - Variable branch count (more at bottom, fewer at top)" << std::endl;
    std::cout << "  - Color gradient (brown trunk -> green tips)" << std::endl;
    std::cout << "  - Dynamic rotation speed" << std::endl;
    std::cout << "  - Organic swaying and breathing" << std::endl;
    std::cout << "Close the window to exit." << std::endl << std::endl;
    
    // Animation variables
    double time = 0.0;
    double windPhase = 0.0;
    double growthPhase = 0.0;
    double rotationAngle = 0.0;
    double branchCountPhase = 0.0;
    double speedPhase = 0.0;
    
    // Animation loop
    while (!canvas.ShouldClose()) {
        // Update time
        time += 0.016; // approximately 60 FPS
        windPhase += 0.02;
        growthPhase += 0.01;
        branchCountPhase += 0.005;
        speedPhase += 0.008;
        
        // Create animated parameters
        TreeParams animParams = baseParams;
        
        // Dynamic rotation speed (oscillates between slow and fast)
        animParams.rotationSpeed = baseParams.rotationSpeed + 0.3 * sin(speedPhase);
        rotationAngle += animParams.rotationSpeed;
        
        // Dynamic branch count (oscillates between 3 and 7)
        animParams.numBranches = 5 + (int)(2.0 * sin(branchCountPhase));
        if (animParams.numBranches < 3) animParams.numBranches = 3;
        if (animParams.numBranches > 7) animParams.numBranches = 7;
        
        // Breathing/growing effect
        double breathe = 0.03 * sin(growthPhase * 0.7);
        animParams.lambda = baseParams.lambda + breathe;
        
        // Swaying - angle variation
        animParams.angle = baseParams.angle + 5.0 * sin(windPhase * 1.3);
        
        // Branch position shimmer
        animParams.factor = baseParams.factor + 0.05 * sin(time * 0.9);
        
        // Additional wobble
        animParams.angle += 2.0 * sin(time * 2.1);
        
        // Draw the tree with current parameters and rotation
        drawTree(maxDepth, animParams, rotationAngle);
        
        // Update display
        canvas.Update();
        
        // Frame rate control
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "Tree animation ended. Goodbye!" << std::endl;
    
    return 0;
}
