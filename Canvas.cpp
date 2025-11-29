/*========================================================================
 * File: Canvas.cpp
 * Purpose: implementation of Canvas class using GLFW with 3D support
 *======================================================================*/
#include "Canvas.h"
#include <cmath>
#include <iostream>

Canvas::Canvas(int w, int h)
    : width(w), height(h), curPosX(w/2), curPosY(h/2),
      curColorR(1.0f), curColorG(1.0f), curColorB(1.0f),
      curLineWidth(1), curLineStyle(SOLID), window(nullptr),
      rotationX(0.0), rotationY(0.0)
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    window = glfwCreateWindow(width, height, "Boom - 3D Recursive Tree", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwMakeContextCurrent(window);
    
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Set up perspective projection for 3D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = (double)width / (double)height;
    double fov = 45.0;
    double nearPlane = 1.0;
    double farPlane = 1000.0;
    double top = nearPlane * tan(fov * M_PI / 360.0);
    double bottom = -top;
    double right = top * aspect;
    double left = -right;
    glFrustum(left, right, bottom, top, nearPlane, farPlane);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set background to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Canvas::~Canvas()
{
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void Canvas::SetColor(Color color)
{
    switch (color) {
        case BLACK:   curColorR = 0.0f; curColorG = 0.0f; curColorB = 0.0f; break;
        case RED:     curColorR = 1.0f; curColorG = 0.0f; curColorB = 0.0f; break;
        case GREEN:   curColorR = 0.0f; curColorG = 1.0f; curColorB = 0.0f; break;
        case YELLOW:  curColorR = 1.0f; curColorG = 1.0f; curColorB = 0.0f; break;
        case BLUE:    curColorR = 0.0f; curColorG = 0.0f; curColorB = 1.0f; break;
        case MAGENTA: curColorR = 1.0f; curColorG = 0.0f; curColorB = 1.0f; break;
        case CYAN:    curColorR = 0.0f; curColorG = 1.0f; curColorB = 1.0f; break;
        case WHITE:   curColorR = 1.0f; curColorG = 1.0f; curColorB = 1.0f; break;
    }
}

void Canvas::Line3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
    LineSegment3D line;
    line.x1 = x1;
    line.y1 = y1;
    line.z1 = z1;
    line.x2 = x2;
    line.y2 = y2;
    line.z2 = z2;
    line.r = curColorR;
    line.g = curColorG;
    line.b = curColorB;
    line.width = curLineWidth;
    lines3D.push_back(line);
}

void Canvas::SetRotation(double angleX, double angleY)
{
    rotationX = angleX;
    rotationY = angleY;
}

void Canvas::addLine(int x1, int y1, int x2, int y2)
{
    LineSegment line;
    line.x1 = x1;
    line.y1 = y1;
    line.x2 = x2;
    line.y2 = y2;
    line.r = curColorR;
    line.g = curColorG;
    line.b = curColorB;
    line.width = curLineWidth;
    lines.push_back(line);
}

void Canvas::Line(int x1, int y1, int x2, int y2)
{
    // Clip to canvas bounds
    if (x1 < 0) x1 = 0; if (x1 >= width) x1 = width - 1;
    if (y1 < 0) y1 = 0; if (y1 >= height) y1 = height - 1;
    if (x2 < 0) x2 = 0; if (x2 >= width) x2 = width - 1;
    if (y2 < 0) y2 = 0; if (y2 >= height) y2 = height - 1;
    
    addLine(x1, y1, x2, y2);
    curPosX = x2;
    curPosY = y2;
}

void Canvas::Line(int x, int y)
{
    Line(curPosX, curPosY, x, y);
}

void Canvas::LineRel(int dx, int dy)
{
    Line(curPosX, curPosY, curPosX + dx, curPosY + dy);
}

void Canvas::MoveTo(int x, int y)
{
    if (x < 0) x = 0; if (x >= width) x = width - 1;
    if (y < 0) y = 0; if (y >= height) y = height - 1;
    curPosX = x;
    curPosY = y;
}

void Canvas::MoveRel(int dx, int dy)
{
    MoveTo(curPosX + dx, curPosY + dy);
}

void Canvas::Point(int x, int y)
{
    MoveTo(x, y);
    addLine(x, y, x, y);
}

void Canvas::PointRel(int dx, int dy)
{
    Point(curPosX + dx, curPosY + dy);
}

void Canvas::Rectangle(int upLeftX, int upLeftY, int w, int h, int filled)
{
    MoveTo(upLeftX, upLeftY);
    if (filled) {
        Line(upLeftX, upLeftY, upLeftX + w, upLeftY);
        Line(upLeftX + w, upLeftY, upLeftX + w, upLeftY + h);
        Line(upLeftX + w, upLeftY + h, upLeftX, upLeftY + h);
        Line(upLeftX, upLeftY + h, upLeftX, upLeftY);
    } else {
        Line(upLeftX, upLeftY, upLeftX + w, upLeftY);
        Line(upLeftX + w, upLeftY, upLeftX + w, upLeftY + h);
        Line(upLeftX + w, upLeftY + h, upLeftX, upLeftY + h);
        Line(upLeftX, upLeftY + h, upLeftX, upLeftY);
    }
}

void Canvas::Rectangle(int w, int h, int filled)
{
    Rectangle(curPosX, curPosY, w, h, filled);
}

void Canvas::RectangleRel(int dx, int dy, int w, int h, int filled)
{
    Rectangle(curPosX + dx, curPosY + dy, w, h, filled);
}

void Canvas::Arc(int centreX, int centreY, int w, int h, int startAngle, int angle, int filled)
{
    MoveTo(centreX, centreY);
    int segments = 36;
    double angleStep = (angle * M_PI / 180.0) / segments;
    double startRad = startAngle * M_PI / 180.0;
    
    for (int i = 0; i < segments; i++) {
        double a1 = startRad + i * angleStep;
        double a2 = startRad + (i + 1) * angleStep;
        int x1 = centreX + (int)(w * cos(a1));
        int y1 = centreY + (int)(h * sin(a1));
        int x2 = centreX + (int)(w * cos(a2));
        int y2 = centreY + (int)(h * sin(a2));
        Line(x1, y1, x2, y2);
    }
}

void Canvas::Arc(int w, int h, int startAngle, int angle, int filled)
{
    Arc(curPosX, curPosY, w, h, startAngle, angle, filled);
}

void Canvas::ArcRel(int dx, int dy, int w, int h, int startAngle, int angle, int filled)
{
    Arc(curPosX + dx, curPosY + dy, w, h, startAngle, angle, filled);
}

void Canvas::Text(int x, int y, const char* text)
{
    MoveTo(x, y);
}

void Canvas::SetLineWidth(int width)
{
    curLineWidth = width;
}

void Canvas::SetLineStyle(LineStyle style)
{
    curLineStyle = style;
}

void Canvas::SetFont(Font fontID)
{
    // Font handling skipped for basic implementation
}

void Canvas::Clear()
{
    lines.clear();
    lines3D.clear();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Canvas::drawStoredLines()
{
    for (const auto& line : lines) {
        glColor3f(line.r, line.g, line.b);
        glLineWidth((GLfloat)line.width);
        
        glBegin(GL_LINES);
        glVertex2i(line.x1, line.y1);
        glVertex2i(line.x2, line.y2);
        glEnd();
    }
}

void Canvas::drawStoredLines3D()
{
    for (const auto& line : lines3D) {
        glColor3f(line.r, line.g, line.b);
        glLineWidth((GLfloat)line.width);
        
        glBegin(GL_LINES);
        glVertex3d(line.x1, line.y1, line.z1);
        glVertex3d(line.x2, line.y2, line.z2);
        glEnd();
    }
}

void Canvas::Show()
{
    glfwMakeContextCurrent(window);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -300.0);
    glRotated(rotationX, 1.0, 0.0, 0.0);
    glRotated(rotationY, 0.0, 1.0, 0.0);
    
    drawStoredLines3D();
    glfwSwapBuffers(window);
}

bool Canvas::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Canvas::Update()
{
    glfwMakeContextCurrent(window);
    glfwPollEvents();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -300.0);
    glRotated(rotationX, 1.0, 0.0, 0.0);
    glRotated(rotationY, 0.0, 1.0, 0.0);
    
    drawStoredLines3D();
    glfwSwapBuffers(window);
}

void Canvas::ClearLines()
{
    lines.clear();
    lines3D.clear();
}
