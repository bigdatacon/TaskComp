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



IntersectionResult calculateIntersection(const Point& first_startp, const Point& startPoint, double angle, const LineSegment& edge, int& reflect_q) {
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
            double new_x = x1 + t * (x2 - x1);
            double new_y = y1 + t * (y2 - y1);

            if (!DotOnEdge(Point(new_x, new_y), edge)) { return result; } //точка пересечения не на отрезке

            // проверяю лежит ли первоначальная точка на новом отрезке
            LineSegment new_edge(startPoint, Point(new_x, new_y));
            if (DotOnEdge(first_startp, new_edge) && reflect_q > 0) { // начальная точка на отрезке и это как минимум второй луч выпущенный внутри фигуры - возвращаю true , угол найден
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
            result.reflectionAngle = normalAngle + result.incidentAngle; // Угол отражения

            // второй вариант расчета угла отражения 
            /*
            double normalAngle = atan2(edge.end.y - edge.start.y, edge.end.x - edge.start.x);
            double reflectionAngle = normalAngle - angle; // Угол отражения
            result.incidentAngle = angle;
            result.reflectionAngle = reflectionAngle; // Угол отражения
            */


            result.find = true;

            // Ограничиваем угол в диапазоне [0, 2*Pi]
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
    if (reflect_q > 1000) { /*cout << "By 1000 iterations cant find desired degree" << endl;*/  return false; } // не найден нужный угол при 1000 касаний сторон фигуры

    for (const LineSegment& edge : polygon) {
        IntersectionResult result = calculateIntersection(startPoint, startPointcopy, angle, edge, reflect_q);
        if (result.twice_more_visited) {
            points.push_back(Point(result.intersectionPoint.x, result.intersectionPoint.y));
            drawLines(points);
            reflect_q++;
            cout << "Find angle : " << "size.points : " << points.size() << " reflect_q: " << reflect_q << endl;
            cout << points << endl;

            return true;
        } // попал 2 раза в точку из которой изнчально вышел луч 

        if (result.find) {
            Point next_point(result.intersectionPoint.x, result.intersectionPoint.y);
            double reflectionAngle = result.reflectionAngle;
            reflect_q++;
            points.push_back(next_point);
            //drawLines(points); // отрисовываю линию для отладки
            //std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // небольшая пауза для отладки 
            return findIntersection(startPoint, next_point, reflectionAngle, polygon, reflect_q, points); // рекурсивно иду дальше от новой точки пересечения ищу следующую точку
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
    const Point startPoint(x, y); // первончальная точка - константа 
    Point startPointcopy(x, y); // дублирую начальну точку для 1 итерации 


    if (!ArePolygonSidesNonIntersecting(polygon)) { cout << "FIGURE SIDES INTERESECT EACH OTHER " << endl; } // многоугольник имеет пересечения

    if (!isPointInsidePolygon(startPoint, polygon)) { // проверка что точка внутри полигона
        std::cout << "start dot is out of figure" << std::endl;
        return 0;
    }

    double angleStep = M_PI / 1800.0; // Шаг угла в радианах (1/10 градуса)
    drawPolygon(polygon);  // рисую многоугольник

    std::vector<Point> test_line_start_point;
    test_line_start_point.push_back(startPoint);
    //drawLines(test_line_start_point); //рисую точку для проверки через вектор точек -- не корректно работает 
    drawPoint(startPoint);  // рисую точку для проверки через другую функцию 





    for (int i = 0; i < 3600; ++i) {
        double currentAngle = i * angleStep;
        if (currentAngle == 0) { currentAngle += 1; }

        //1. запускаю итерацию по сторонам многоугольника
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
        //1. Быстрая проверка на найденном угле
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

