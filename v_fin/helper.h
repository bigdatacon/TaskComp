#pragma once

#include <vector>
#include <iostream>



struct Point {
    Point();
    Point(double x, double y);
    double x;
    double y;
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

bool operator==(const Point& lhs, const Point& rhs);
bool operator!=(const Point& lhs, const Point& rhs);
Point operator-(const Point& lhs, const Point& rhs);

std::ostream& operator<<(std::ostream& os, const std::vector<Point>& points);

struct IntersectionResult {
    Point intersectionPoint;
    double incidentAngle;
    double reflectionAngle;
    bool find=false;
    bool twice_more_visited=false;
};

struct LineSegment {
    Point start;
    Point end;
    LineSegment(const Point& start, const Point& end);
};

bool operator<(const LineSegment& lhs, const LineSegment& rhs);

bool isPointInsidePolygon(const Point& point, const std::vector<LineSegment>& polygon);
bool DotOnEdge(const Point& point, const LineSegment& edge);

void drawPolygon(const std::vector<LineSegment>& polygon);
void drawLines(const std::vector<Point>& points);
void drawPoint(const Point& point);

// ������� ��� ����������� ���������� ����� a, b � c
int Orientation(const Point& a, const Point& b, const Point& c);
// ������� ��� �������� ����������� ���� ��������
bool DoLineSegmentsIntersect(const LineSegment& seg1, const LineSegment& seg2);

// ������� ��� ��������, ����� �� ����� q ����� p � r
bool OnSegment(const Point& p, const Point& q, const Point& r);

// ������� ��� �������� ���������� ��������������� � ��������������
bool ArePolygonSidesNonIntersecting(const std::vector<LineSegment>& polygon);