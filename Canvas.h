/*========================================================================
* File: Canvas.h
 * Purpose: header file to Canvas class (GLFW implementation)
 * Modernized for GLFW with 3D support
 *======================================================================*/
#ifndef CANVAS_H
#define CANVAS_H

#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Canvas
{
    public:
        enum Color {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE};
        enum LineStyle {SOLID, DASHED};
        enum Font {SMALL, NORMAL, BIG};

        Canvas(int, int);
        ~Canvas();

        void Clear(void);
        void SetColor(Color);
        void SetColorRGB(float r, float g, float b);
        void LineRel(int, int);
        void Line(int, int, int, int);
        void Line(int, int);
        void MoveTo(int, int);
        void MoveRel(int, int);
        void Point(int, int);
        void PointRel(int, int);
        void Rectangle(int, int, int, int, int=0);
        void Rectangle(int, int, int=0);
        void RectangleRel(int, int, int, int, int=0);
        void Arc(int, int, int, int, int=0, int=360, int=0);
        void Arc(int, int, int=0, int=360, int=0);
        void ArcRel(int, int, int, int, int=0, int=360, int=0);
        void Text(int, int, const char*);
        void SetLineWidth(int=1);
        void SetLineStyle(LineStyle=SOLID);
        void SetFont(Font=NORMAL);
        void Show(void);

        // Animation methods
        bool ShouldClose();
        void Update();
        void ClearLines();

        // 3D methods
        void Line3D(double x1, double y1, double z1, double x2, double y2, double z2);
        void Line3DColored(double x1, double y1, double z1, double x2, double y2, double z2,
                          float r, float g, float b);
        void SetRotation(double angleX, double angleY);

    private:
        struct LineSegment {
            int x1, y1, x2, y2;
            float r, g, b;
            int width;
        };
        
        struct LineSegment3D {
            double x1, y1, z1, x2, y2, z2;
            float r, g, b;
            int width;
        };

        int width;
        int height;
        int curPosX;
        int curPosY;
        float curColorR, curColorG, curColorB;
        int curLineWidth;
        int curLineStyle;

        GLFWwindow* window;
        std::vector<LineSegment> lines;
        std::vector<LineSegment3D> lines3D;
        
        double rotationX;
        double rotationY;

        void addLine(int x1, int y1, int x2, int y2);
        void drawStoredLines();
        void drawStoredLines3D();
};

#endif // CANVAS_H
