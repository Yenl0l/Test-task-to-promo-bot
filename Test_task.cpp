#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <mpi.h>

// Абстрактный класс для геометрических фигур
class Shape {
public:
    virtual double area() const = 0; // Метод для расчета площади
    virtual void print() const = 0;  // Метод для вывода информации о фигуре
    virtual ~Shape() = default;
};

// Класс для круга
class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() const override {
        return 3.1415 * radius * radius;
    }
    void print() const override {
        std::cout << "Circle (radius: " << radius << ", area: " << area() << ")";
    }
};

// Класс для треугольника
class Triangle : public Shape {
private:
    double a, b, c;
public:
    Triangle(double side1, double side2, double side3) : a(side1), b(side2), c(side3) {}
    double area() const override {
        double p = (a + b + c) / 2; // Полупериметр
        return sqrt(p * (p - a) * (p - b) * (p - c)); // Формула Герона
    }
    void print() const override {
        std::cout << "Triangle (sides: " << a << ", " << b << ", " << c << ", area: " << area() << ")";
    }
};

// Класс для прямоугольника
class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override {
        return width * height;
    }
    void print() const override {
        std::cout << "Rectangle (width: " << width << ", height: " << height << ", area: " << area() << ")";
    }
};

// Функция для вывода массива фигур
void printShapes(const std::vector<Shape*>& shapes) {
    for (const auto& shape : shapes) {
        shape->print();
        std::cout << std::endl;
    }
}

// Быстрая сортировка (Quick Sort)
void quickSort(std::vector<Shape*>& shapes, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    Shape* pivot = shapes[(left + right) / 2];
    while (i <= j) {
        while (shapes[i]->area() < pivot->area()) i++;
        while (shapes[j]->area() > pivot->area()) j--;
        if (i <= j) {
            std::swap(shapes[i], shapes[j]);
            i++;
            j--;
        }
    }
    quickSort(shapes, left, j);
    quickSort(shapes, i, right);
}

// Сортировка пузырьком (Bubble Sort)
void bubbleSort(std::vector<Shape*>& shapes) {
    int n = shapes.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (shapes[j]->area() > shapes[j + 1]->area()) {
                std::swap(shapes[j], shapes[j + 1]);
            }
        }
    }
}

// Сортировка вставками (Insertion Sort)
void insertionSort(std::vector<Shape*>& shapes) {
    int n = shapes.size();
    for (int i = 1; i < n; i++) {
        Shape* key = shapes[i];
        int j = i - 1;
        while (j >= 0 && shapes[j]->area() > key->area()) {
            shapes[j + 1] = shapes[j];
            j--;
        }
        shapes[j + 1] = key;
    }
}

// Сортировка выбором (Selection Sort)
void selectionSort(std::vector<Shape*>& shapes) {
    int n = shapes.size();
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (shapes[j]->area() < shapes[minIndex]->area()) {
                minIndex = j;
            }
        }
        std::swap(shapes[i], shapes[minIndex]);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Создаем массив фигур
    std::vector<Shape*> shapes = {
        new Circle(2.0),
        new Triangle(3.0, 4.0, 5.0),
        new Rectangle(4.0, 5.0),
        new Circle(1.0),
        new Triangle(6.0, 8.0, 10.0),
        new Rectangle(2.0, 3.0)
    };

    std::string sortMethod;

    switch (rank) {
    case 0:
        quickSort(shapes, 0, shapes.size() - 1);
        sortMethod = "быстрая";
        break;
    case 1:
        bubbleSort(shapes);
        sortMethod = "пузырьком";
        break;
    case 2:
        insertionSort(shapes);
        sortMethod = "вставками";
        break;
    case 3:
        selectionSort(shapes);
        sortMethod = "выбором";
        break;
    default:
        std::cout << "Некорректный ранг сортировки" << std::endl;
    }

    // Выводим результат
    std::cout << "Сортировка: " << sortMethod << std::endl;
    std::cout << "Результат: ";
    for (const auto& shape : shapes) {
        std::cout << shape->area() << " ";
    }
    std::cout << std::endl;

    // Освобождаем память
    for (auto& shape : shapes) {
        delete shape;
    }

    MPI_Finalize();
    return 0;
}