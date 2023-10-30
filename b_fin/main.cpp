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
    // Инициализируйте многоугольник с вашими отрезками
    polygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    polygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    polygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    polygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));
    // Точка, которая находится внутри многоугольника
    Point insidePoint(3.0, 3.0);
    assert(isPointInsidePolygon(insidePoint, polygon) == true);

    // Точка, которая находится снаружи многоугольника
    Point outsidePoint(8.0, 8.0);
    assert(isPointInsidePolygon(outsidePoint, polygon) == false);
}

void TestArePolygonSidesNonIntersecting() {
    std::vector<LineSegment> nonIntersectingPolygon;
    // Инициализируйте многоугольник без самопересечений

    // Создайте многоугольник с четырьмя сторонами без самопересечений
    nonIntersectingPolygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 1.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(7.0, 1.0), Point(7.0, 7.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(7.0, 7.0), Point(1.0, 7.0)));
    nonIntersectingPolygon.push_back(LineSegment(Point(1.0, 7.0), Point(1.0, 1.0)));

    assert(ArePolygonSidesNonIntersecting(nonIntersectingPolygon) == true);

    std::vector<LineSegment> intersectingPolygon;
    // Инициализируйте многоугольник с самопересечениями

    // Создайте многоугольник с самопересечениями
    intersectingPolygon.push_back(LineSegment(Point(1.0, 1.0), Point(7.0, 7.0)));
    intersectingPolygon.push_back(LineSegment(Point(7.0, 1.0), Point(1.0, 7.0)));

    assert(ArePolygonSidesNonIntersecting(intersectingPolygon) == false);
}





IntersectionResult calculateIntersection(const Point& startPoint, double angle, const LineSegment& edge, std::map<LineSegment, int>& visited) {
    IntersectionResult result;
    if (DotOnEdge(startPoint, edge)) { return result; } // Начальная точка лежит на отрезке}
    result.find = false;

    // Параметры луча
    double x1 = startPoint.x;
    double y1 = startPoint.y;
    double x2 = x1 + cos(angle);
    double y2 = y1 + sin(angle);

    double x3 = edge.start.x;
    double y3 = edge.start.y;
    double x4 = edge.end.x;
    double y4 = edge.end.y;

    // Проверяем, пересекается ли луч с отрезком
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denominator != 0) {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;



        //if (t >= 0 && t <= 1 && u > 0) {
        if (u > 0 && u <= 1 && t > 0) {
            // Линии пересекаются
            if (visited[edge] == 0) { //не было до этого пересечений


                double new_x = x1 + t * (x2 - x1);
                double new_y = y1 + t * (y2 - y1);

                if (!DotOnEdge(Point(new_x, new_y), edge)) { return result; } //точка пересечения не на отрезке

                result.intersectionPoint.x = new_x;
                result.intersectionPoint.y = new_y;

                double normalAngle = atan2(y4 - y3, x4 - x3);
                result.incidentAngle = normalAngle - angle;
                result.reflectionAngle = normalAngle + result.incidentAngle; // Угол отражения
                result.find = true;

                // Ограничиваем угол в диапазоне [0, 2*Pi]
                while (result.reflectionAngle < 0) {
                    result.reflectionAngle += 2 * M_PI;
                }
                while (result.reflectionAngle >= 2 * M_PI) {
                    result.reflectionAngle -= 2 * M_PI;
                }
                visited[edge]++;

            }
            // уже было пересечение с этим реброи
            else { result.twice_more_visited = true; return result; }
        }
    }

    return result;
}



bool findIntersection(const Point& startPoint, double angle, const std::vector<LineSegment>& polygon, std::map<LineSegment, int> visited, int& reflect_q, std::vector<Point>& points) {
    if (reflect_q == polygon.size()) { drawLines(points); return true; } // найден нужный угол при прохождении стороны 1 раз

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, angle, edge, visited);
        if (result.twice_more_visited) { return false; } // если пересечений с 1 ребром более 1 раза выходим из цикла и этот угол не проверяем 

        if (result.find) {
            Point next_point(result.intersectionPoint.x, result.intersectionPoint.y);
            double reflectionAngle = result.reflectionAngle;
            reflect_q++;
            points.push_back(next_point);
            //drawLines(points); // отрисовываю линию для отладки
            //std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // небольшая пауза для отладки 
            return findIntersection(next_point, reflectionAngle, polygon, visited, reflect_q, points); // рекурсивно иду дальше от новой точки пересечения ищу следующую точку
        }

    }
    return false;
}

int main() {
    // тесты
    TestIsPointInsidePolygon();
    //TestArePolygonSidesNonIntersecting(); // функция ArePolygonSidesNonIntersecting не корректно считает поэтому этот тест не запускаю


    std::vector<LineSegment> polygon;
    std::vector<Point> test_line;


    //читаю из файла 
    std::ifstream inputFile("input.txt"); // Открываем файл input.txt

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open input.txt" << std::endl;
        return 1; // Если не удалось открыть файл, завершаем программу с ошибкой.
    }

    json::Document input_ = json::Load(inputFile);
    //json::Document input_ = json::Load(cin); // либо читаю из cin Но тоже json файл который в файлах - то есть просто копирую в cin содерж файла
    const auto& polygon_array = ((input_.GetRoot()).AsDict()).at("polygon"s).AsArray();
    const auto& point_array = ((input_.GetRoot()).AsDict()).at("startPoint"s).AsArray();


    // Проход по элементам полигона:
    for (const json::Node& edge : polygon_array) {
        double startX = edge.AsDict().at("start").AsArray()[0].AsDouble();
        double startY = edge.AsDict().at("start").AsArray()[1].AsDouble();
        double endX = edge.AsDict().at("end").AsArray()[0].AsDouble();
        double endY = edge.AsDict().at("end").AsArray()[1].AsDouble();
        polygon.push_back(LineSegment(Point(startX, startY), Point(endX, endY)));

    }


    double x = point_array[0].AsDouble(); // Получить значение X
    double y = point_array[1].AsDouble(); // Получить значение Y
    Point startPoint(x, y);


    if (!ArePolygonSidesNonIntersecting(polygon)) { cout << "FIGURE SIDES INTERESECT EACH OTHER " << endl; } // многоугольник имеет пересечения

    if (!isPointInsidePolygon(startPoint, polygon)) { // проверка что точка внутри полигона
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // Шаг угла в радианах (1/10 градуса)
    drawPolygon(polygon);  // рисую многоугольник

    std::vector<Point> test_line_start_point;
    test_line_start_point.push_back(startPoint);
    //drawLines(test_line_start_point); //рисую точку для проверки через вектор точек 
    drawPoint(startPoint);  // рисую точку для проверки через другую функцию 

    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;

        //1. запускаю итерацию по сторонам многоугольника
        int reflect_q = 0;
        std::map<LineSegment, int> visited;
        std::vector<Point> points;
        points.push_back(startPoint);
        if (findIntersection(startPoint, currentAngle, polygon, visited, reflect_q, points)) {
            std::cout << "Find ANGLE : " << currentAngle * 180.0 / M_PI << "  DEGREE" << endl;

            //drawLines(points);
            return 0;
        }


    }

    cout << "Not find need angle" << endl;

    // Для быстрой отладке по уже найденному углу без цикла
    /*
    const double angleInDegrees = 33;  // Угол в градусах
    const double angleInRadians = angleInDegrees * M_PI / 180.0;  // Преобразование в радианы
    int reflect_q = 0;
    std::map<LineSegment, int> visited;
    std::vector<Point> points;
    points.push_back(startPoint);
    if (findIntersection(startPoint, angleInRadians, polygon, visited, reflect_q, points)) {
        std::cout << "Find ANGLE : " << angleInDegrees << endl;

        //drawLines(points);
        return 0;
    }
    else
    {
    cout << "Not find need angle" << endl;
    }

    */
    return 0;



}

