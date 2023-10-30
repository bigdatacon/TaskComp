#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <fstream>

#include <cassert>
#include "helper.h"
#include "json.h"

using namespace std;



void TestIsPointInsidePolygon() {
    std::vector<LineSegment> polygon;
    // ��������������� ������������� � ������ ���������
    polygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    polygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    polygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    polygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));
    // �����, ������� ��������� ������ ��������������
    Point insidePoint(3.0, 3.0);
    assert(isPointInsidePolygon(insidePoint, polygon) == true);

    // �����, ������� ��������� ������� ��������������
    Point outsidePoint(8.0, 8.0);
    assert(isPointInsidePolygon(outsidePoint, polygon) == false);
}

void TestArePolygonSidesNonIntersecting() {
    std::vector<LineSegment> nonIntersectingPolygon;
    // ��������������� ������������� ��� ���������������

    // �������� ������������� � �������� ��������� ��� ���������������
    nonIntersectingPolygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));

    assert(ArePolygonSidesNonIntersecting(nonIntersectingPolygon) == true);

    std::vector<LineSegment> intersectingPolygon;
    // ��������������� ������������� � �����������������

    // �������� ������������� � �����������������
    intersectingPolygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 7.0)));
    intersectingPolygon.push_back(LineSegment(Point(7.0, 1.0), Point(1.0, 7.0)));

    assert(ArePolygonSidesNonIntersecting(intersectingPolygon) == false);
}



IntersectionResult calculateIntersection(const Point& first_startp, const Point& startPoint, double angle, const LineSegment& edge, int& reflect_q) {
    IntersectionResult result;
    if (DotOnEdge(startPoint, edge)) { return result; } // ��������� ����� ����� �� �������}
    result.find = false;

    // ��������� ����
    double x1 = startPoint.x;
    double y1 = startPoint.y;
    double x2 = x1 + cos(angle);
    double y2 = y1 + sin(angle);

    double x3 = edge.start.x;
    double y3 = edge.start.y;
    double x4 = edge.end.x;
    double y4 = edge.end.y;

    // ���������, ������������ �� ��� � ��������
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator != 0) {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

        //if (t >= 0 && t <= 1 && u > 0) {
        if (u > 0 && u <= 1 && t > 0) {
            // ����� ������������
            double new_x = x1 + t * (x2 - x1);
            double new_y = y1 + t * (y2 - y1);

            if (!DotOnEdge(Point(new_x, new_y), edge)) { return result; } //����� ����������� �� �� �������

            // �������� ����� �� �������������� ����� �� ����� �������
            LineSegment new_edge(startPoint, Point(new_x, new_y));
            if (DotOnEdge(first_startp, new_edge) && reflect_q > 0) { // ��������� ����� �� ������� � ��� ��� ������� ������ ��� ���������� ������ ������ - ��������� true , ���� ������
                result.find = true;
                result.twice_more_visited = true;
                result.intersectionPoint.x = new_x;
                result.intersectionPoint.y = new_y;

                return result;
            }

            result.intersectionPoint.x = new_x;
            result.intersectionPoint.y = new_y;

            double normalAngle = atan2(y4 - y3, x4 - x3);
            result.incidentAngle = normalAngle - angle;
            result.reflectionAngle = normalAngle + result.incidentAngle; // ���� ���������

            // ������ ������� ������� ���� ��������� 
            /*
            double normalAngle = atan2(edge.end.y - edge.start.y, edge.end.x - edge.start.x);
            double reflectionAngle = normalAngle - angle; // ���� ���������
            result.incidentAngle = angle;
            result.reflectionAngle = reflectionAngle; // ���� ���������
            */


            result.find = true;

            // ������������ ���� � ��������� [0, 2*Pi]
            while (result.reflectionAngle < 0) {
                result.reflectionAngle += 2 * M_PI;
            }
            while (result.reflectionAngle >= 2 * M_PI) {
                result.reflectionAngle -= 2 * M_PI;
            }


        }
    }

    return result;
}




bool findIntersection(const Point& startPoint, const Point& startPointcopy, double angle, const std::vector<LineSegment>& polygon, int& reflect_q, std::vector<Point>& points) {
    if (reflect_q > 1000) { /*cout << "By 1000 iterations cant find desired degree" << endl;*/  return false; } // �� ������ ������ ���� ��� 1000 ������� ������ ������

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, startPointcopy, angle, edge, reflect_q);
        if (result.twice_more_visited) {
            points.push_back(Point(result.intersectionPoint.x, result.intersectionPoint.y));
            drawLines(points);
            reflect_q++;
            cout << "Find angle : " << "size.points : " << points.size() << " reflect_q: " << reflect_q << endl;
            cout << points << endl;

            return true;
        } // ����� 2 ���� � ����� �� ������� ��������� ����� ��� 

        if (result.find) {
            Point next_point(result.intersectionPoint.x, result.intersectionPoint.y);
            double reflectionAngle = result.reflectionAngle;
            reflect_q++;
            points.push_back(next_point);
            //drawLines(points); // ����������� ����� ��� �������
            //std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // ��������� ����� ��� ������� 
            return findIntersection(startPoint, next_point, reflectionAngle, polygon, reflect_q, points); // ���������� ��� ������ �� ����� ����� ����������� ��� ��������� �����
        }

    }
    return false;
}

int main() {
    // �����
    TestIsPointInsidePolygon();
    //TestArePolygonSidesNonIntersecting(); // ������� ArePolygonSidesNonIntersecting �� ��������� ������� ������� ���� ���� �� ��������


    std::vector<LineSegment> polygon;
    std::vector<Point> test_line;


    //����� �� ����� 
    std::ifstream inputFile("input.txt"); // ��������� ���� input.txt

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input.txt" << std::endl;
        return 1; // ���� �� ������� ������� ����, ��������� ��������� � �������.
    }

    json::Document input_ = json::Load(inputFile);
    //json::Document input_ = json::Load(cin); // ���� ����� �� cin �� ���� json ���� ������� � ������ - �� ���� ������ ������� � cin ������ �����
    const auto& polygon_array = ((input_.GetRoot()).AsDict()).at("polygon"s).AsArray();
    const auto& point_array = ((input_.GetRoot()).AsDict()).at("startPoint"s).AsArray();


    // ������ �� ��������� ��������:
    for (const json::Node& edge : polygon_array) {
        double startX = edge.AsDict().at("start").AsArray()[0].AsDouble();
        double startY = edge.AsDict().at("start").AsArray()[1].AsDouble();
        double endX = edge.AsDict().at("end").AsArray()[0].AsDouble();
        double endY = edge.AsDict().at("end").AsArray()[1].AsDouble();
        polygon.push_back(LineSegment(Point(startX, startY), Point(endX, endY)));

    }


    double x = point_array[0].AsDouble(); // �������� �������� X
    double y = point_array[1].AsDouble(); // �������� �������� Y
    const Point startPoint(x, y); // ������������� ����� - ��������� 
    Point startPointcopy(x, y); // �������� �������� ����� ��� 1 �������� 


    if (!ArePolygonSidesNonIntersecting(polygon)) { cout << "FIGURE SIDES INTERESECT EACH OTHER " << endl; } // ������������� ����� �����������

    if (!isPointInsidePolygon(startPoint, polygon)) { // �������� ��� ����� ������ ��������
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // ��� ���� � �������� (1/10 �������)
    drawPolygon(polygon);  // ����� �������������

    std::vector<Point> test_line_start_point;
    test_line_start_point.push_back(startPoint);
    //drawLines(test_line_start_point); //����� ����� ��� �������� ����� ������ ����� -- �� ��������� �������� 
    drawPoint(startPoint);  // ����� ����� ��� �������� ����� ������ ������� 





    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;
        if (currentAngle == 0) { currentAngle += 1; }

        //1. �������� �������� �� �������� ��������������
        int reflect_q = 0;
        std::map<LineSegment, int> visited;
        std::vector<Point> points;
        points.push_back(startPoint);
        if (findIntersection(startPoint, startPointcopy, currentAngle, polygon, reflect_q, points)) {
            std::cout << "Find ANGLE : " << currentAngle * 180.0 / M_PI << "  DEGREE" << endl;

            //drawLines(points);
            return 0;
        }


    }

    /*
        //1. ������� �������� �� ��������� ����
    double currentAngle = 45.00 * M_PI / 180.0;
    int reflect_q = 0;
    std::map<LineSegment, int> visited;
    std::vector<Point> points;
    points.push_back(startPoint);
    if (findIntersection(startPoint, startPointcopy, currentAngle, polygon, reflect_q, points)) {
        std::cout << "Find ANGLE : " << currentAngle * 180.0 / M_PI << "  DEGREE" << endl;

        //drawLines(points);
        return 0;
    }

    */

    cout << "Not find need angle" << endl;


    return 0;



}

