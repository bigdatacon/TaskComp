

#include "helper.h"
#include <iostream>
#include <cmath>
#include <limits>



Point::Point() : x(0), y(0) {}
Point::Point(double x, double y) : x(x), y(y) {}

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Point& lhs, const Point& rhs) {
    return !(lhs == rhs);
}

Point operator-(const Point& lhs, const Point& rhs) {
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

std::ostream& operator<<(std::ostream& os, const std::vector<Point>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        if (i == 0) {
            os << "This first DOT: " << "x: " << points[i].x << " y: " << points[i].y << std::endl;
        }
        else {
            os << "Next DOT " << (i + 1) << ": " << points[i].x << " y: " << points[i].y << std::endl;
        }
    }
    return os;
}

LineSegment::LineSegment(const Point& start, const Point& end) : start(start), end(end) {}

bool operator<(const LineSegment& lhs, const LineSegment& rhs) {
    if (lhs.start.x < rhs.start.x) {
        return true;
    }
    if (lhs.start.x > rhs.start.x) {
        return false;
    }
    return lhs.start.y < rhs.start.y;
}

bool isPointInsidePolygon(const Point& point, const std::vector<LineSegment>& polygon) {
    int count = 0;
    for (const LineSegment& edge : polygon) {
        if ((edge.start.y > point.y) != (edge.end.y > point.y) &&
            (point.x < (edge.end.x - edge.start.x) * (point.y - edge.start.y) / (edge.end.y - edge.start.y) + edge.start.x)) {
            count++;
        }
    }
    return count % 2 == 1;
}


bool DotOnEdge(const Point& point, const LineSegment& edge) {
    double x1 = edge.start.x;
    double y1 = edge.start.y;
    double x2 = edge.end.x;
    double y2 = edge.end.y;

    // Проверяем, что точка лежит на отрезке, используя параметрическое уравнение отрезка.
    double crossProduct = (point.x - x1) * (y2 - y1) - (point.y - y1) * (x2 - x1);

    if (fabs(crossProduct) > std::numeric_limits<double>::epsilon()) {
        // Если crossProduct не близок к нулю, точка не лежит на отрезке.
        return false;
    }

    // Далее проверяем, что точка внутри отрезка, учитывая координаты x и y.
    if (fabs(x1 - x2) > std::numeric_limits<double>::epsilon()) {
        return (x1 <= point.x && point.x <= x2) || (x2 <= point.x && point.x <= x1);
    }
    else {
        return (y1 <= point.y && point.y <= y2) || (y2 <= point.y && point.y <= y1);
    }
}

void drawPolygon(const std::vector<LineSegment>& polygon) {
    // Найти границы фигуры
    double minX = polygon[0].start.x, minY = polygon[0].start.y, maxX = polygon[0].start.x, maxY = polygon[0].start.y;
    for (const LineSegment& edge : polygon) {
        if (edge.start.x < minX) minX = edge.start.x;
        if (edge.start.y < minY) minY = edge.start.y;
        if (edge.end.x < minX) minX = edge.end.x;
        if (edge.end.y < minY) minY = edge.end.y;

        if (edge.start.x > maxX) maxX = edge.start.x;
        if (edge.start.y > maxY) maxY = edge.start.y;
        if (edge.end.x > maxX) maxX = edge.end.x;
        if (edge.end.y > maxY) maxY = edge.end.y;
    }

    const int width =5* static_cast<int>(maxX - minX) + 1;
    const int height = 5*static_cast<int>(maxY - minY) + 1;

    // Создать двумерный массив для отрисовки фигуры
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Отметить линии фигуры в массиве
    for (const LineSegment& edge : polygon) {
        int x1 =5* static_cast<int>(edge.start.x - minX);
        int y1 = 5*static_cast<int>(edge.start.y - minY);
        int x2 = 5*static_cast<int>(edge.end.x - minX);
        int y2 = 5*static_cast<int> (edge.end.y - minY);

        // Отрисовка линии в массиве
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = '*';
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * error;
            if (e2 > -dy) {
                error -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                error += dx;
                y1 += sy;
            }
        }
    }

    // Отобразить массив в консоли
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Освободить память
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}



void drawLines(const std::vector<Point>& points) {
    char sign = '.';
    if (points.size() == 0) { sign = '#'; }



    if (points.empty()) {
        std::cerr << "Error: The list of points is empty." << std::endl;
        return;
    }

    // Найти границы фигуры
    double minX = points[0].x, minY = points[0].y, maxX = points[0].x, maxY = points[0].y;
    for (const Point& point : points) {
        if (point.x < minX) minX = point.x;
        if (point.y < minY) minY = point.y;
        if (point.x > maxX) maxX = point.x;
        if (point.y > maxY) maxY = point.y;
    }

    const int width = 5*static_cast<int>(maxX - minX) + 1;
    const int height = 5*static_cast<int>(maxY - minY) + 1;

    // Создать двумерный массив для отрисовки фигуры
    char** drawing = new char* [height];
    for (int i = 0; i < height; ++i) {
        drawing[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            drawing[i][j] = ' ';
        }
    }

    // Отметить линии между точками в массиве
    for (size_t i = 1; i < points.size(); ++i) {
        const Point& point1 = points[i - 1];
        const Point& point2 = points[i];

        int x1 = 5*static_cast<int>(point1.x - minX);
        int y1 = 5*static_cast<int>(point1.y - minY);
        int x2 = 5*static_cast<int>(point2.x - minX);
        int y2 = 5*static_cast<int>(point2.y - minY);

        // Отрисовка сплошной линии в массиве
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int error = dx - dy;

        while (true) {
            drawing[y1][x1] = sign; // Используем символ '#' для сплошной линии
            if (x1 == x2 && y1 == y2) break;
            int e2 = 2 * error;
            if (e2 > -dy) {
                error -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                error += dx;
                y1 += sy;
            }
        }
    }

    // Отобразить массив в консоли
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cout << drawing[i][j];
        }
        std::cout << std::endl;
    }

    // Освободить память
    for (int i = 0; i < height; ++i) {
        delete[] drawing[i];
    }
    delete[] drawing;
}



void drawPoint(const Point& point) {
    // Определите размеры экрана или область, в которой вы хотите рисовать точку.
    const int screenWidth = 80; // Ширина экрана
    const int screenHeight = 24; // Высота экрана

    // Проверьте, что координаты точки находятся в пределах экрана.
    if (point.x >= 0 && point.x < screenWidth && point.y >= 0 && point.y < screenHeight) {
        // Отобразите точку на экране, например, используя символ 'X'.
        std::vector<std::vector<char>> screen(screenHeight, std::vector<char>(screenWidth, ' '));

        screen[point.y][point.x] = 'X'; // Разместите символ 'X' на экране в заданных координатах.

        // Выведите содержимое экрана в консоль.
        for (int y = 0; y < screenHeight; ++y) {
            for (int x = 0; x < screenWidth; ++x) {
                std::cout << screen[y][x];
            }
            std::cout << std::endl;
        }
    }
}

// Функция для определения ориентации точек a, b и c
int Orientation(const Point& a, const Point& b, const Point& c) {
    double val = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);

    if (val == 0) return 0;  // Точки коллинеарны
    return (val > 0) ? 1 : 2;  // 1 для по часовой, 2 для против часовой
}

// Функция для проверки пересечения двух отрезков
bool DoLineSegmentsIntersect(const LineSegment& seg1, const LineSegment& seg2) {
    int o1 = Orientation(seg1.start, seg1.end, seg2.start);
    int o2 = Orientation(seg1.start, seg1.end, seg2.end);
    int o3 = Orientation(seg2.start, seg2.end, seg1.start);
    int o4 = Orientation(seg2.start, seg2.end, seg1.end);

    if (o1 != o2 && o3 != o4) return true;

    if (o1 == 0 && OnSegment(seg1.start, seg2.start, seg1.end)) return true;
    if (o2 == 0 && OnSegment(seg1.start, seg2.end, seg1.end)) return true;
    if (o3 == 0 && OnSegment(seg2.start, seg1.start, seg2.end)) return true;
    if (o4 == 0 && OnSegment(seg2.start, seg1.end, seg2.end)) return true;

    return false;
}

// Функция для проверки, лежит ли точка q между p и r
bool OnSegment(const Point& p, const Point& q, const Point& r) {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
        return true;
    return false;
}

// Функция для проверки отсутствия самопересечений в многоугольнике
bool ArePolygonSidesNonIntersecting(const std::vector<LineSegment>& polygon) {
    int n = polygon.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 2; j < n; j++) {
            if (i == 0 && j == n - 1) {
                continue;
            }
            if (DoLineSegmentsIntersect(polygon[i], polygon[i + 1]) || DoLineSegmentsIntersect(polygon[i], polygon[j]) ||
                DoLineSegmentsIntersect(polygon[i + 1], polygon[j]) || DoLineSegmentsIntersect(polygon[i + 1], polygon[(j + 1) % n])) {
                return false;
            }
        }
    }
    return true;
}
