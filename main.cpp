#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <SFML/Graphics.hpp>
#include <set>


#include <unistd.h>

#include "dbscan.h"
using namespace std;
#define MINIMUM_POINTS 1    // minimum number of cluster
#define EPSILON (50)  // distance for clustering, metre^2





//функция чтения csv
void csvReader(std::string filename, std::vector<std::vector<std::string>>& data);


void identifyTimestamp(std::vector<std::string>& index, std::vector<std::vector<std::string>>& data);

//функциям визуализации
void DataVisualization(std::vector<std::string>& index, std::vector<std::vector<std::string>>& data, sf::Font font,sf::RenderWindow& window, std::vector<sf::Vector2f>& points, std::vector<Point>& ClusterPts);
//oid DataVisualization(std::vector<std::string>& index,std::vector<std::vector<std::string>>& data, sf::Font font,sf::RenderWindow& window, std::vector<sf::Vector2f>& points);


void drawGrid(sf::RenderWindow& win, int rows, int cols);



int main() {
    //Уникальные значения timestamp
    std::vector<std::string> identify;

    // Создание вектора, содержащего наборы точек
    std::vector<sf::Vector2f> points;
    //std::vector<sf::Vector2f> SfmlClusterPoints;


    std::vector<Point> ClusteredPts;

    // Создание окна
    sf::RenderWindow window(sf::VideoMode(800, 600), "Обновление наборов точек");
    
    // Создание объекта текста
    sf::Font font;
    if (!font.loadFromFile("font/Arial.ttf")) {
        // Обработка ошибки загрузки шрифта
        return EXIT_FAILURE;
    }

    // Имя файла CSV
    std::string filename = "data/outPointSorted.csv";
    std::vector<std::vector<std::string>> data;

    csvReader(filename, data);
    identifyTimestamp(identify,data);


    DataVisualization(identify,data, font,window,points,ClusteredPts);


}




void csvReader(std::string filename, std::vector<std::vector<std::string>>& data)
{
    // Открыть файл для чтения
    std::ifstream file(filename);

    // Проверка, удалось ли открыть файл
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
    }


    // Чтение данных из файла
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string value;

        while (std::getline(ss, value, ',')) {
            row.push_back(value);
        }

        data.push_back(row);
    }

    // Закрыть файл после чтения
    file.close();
}

void DataVisualization(std::vector<std::string>& index,std::vector<std::vector<std::string>>& data, sf::Font font,sf::RenderWindow& window, std::vector<sf::Vector2f>& points, std::vector<Point>& ClusterPts)
{
    //std::vector<Point> uniqueClusterPoints;
    //std::unordered_set<int> uniqueClusters;
        // Создаем вектор, в котором будут по одной точке из каждого кластера
    std::vector<Point> onePointPerCluster;

    // Создаем множество для отслеживания уникальных меток кластеров
    std::set<int> uniqueLabels;
    vector<int> labels;
        while (window.isOpen())
    {

        for (const auto& identify : index){
            
            points.clear();
            ClusterPts.clear();


            onePointPerCluster.clear();
            uniqueLabels.clear();
            labels.clear();
            //uniqueClusters.clear();
            //uniqueClusterPoints.clear();
            //SfmlClusterPoints.clear();


       
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }


            for (const auto& row : data)
            {
                if (identify == row[2])
                {  
                    
                    points.push_back(sf::Vector2f(std::stof(row[4])/0.5*100+300, std::stof(row[3])/10*400)); //нужно будет дописать функцию трансформации , пока данные нормированны относительно разрешения экрана
                    sf::Text text(row[0], font, 24);
                    Point pt;
                    pt.x = std::stof(row[3])/10*400;
                    pt.y = std::stof(row[4])/0.5*100+300;

                    ClusterPts.push_back(pt);

                }
                

            }


            int num = dbscan(ClusterPts, labels, 20.0, 1);

            // Итерируем по вектору labels и clusters
            for (std::size_t i = 0; i < labels.size(); ++i) {
                int label = labels[i];
        
                // Проверяем, встречается ли текущая метка кластера в уникальных метках
                if (uniqueLabels.find(label) == uniqueLabels.end()) {
                    // Если не встречается, добавляем точку в вектор onePointPerCluster
                    onePointPerCluster.push_back(ClusterPts[i]);
            
                    // Добавляем текущую метку кластера в уникальные метки
                    uniqueLabels.insert(label);
        }
    }
            // Вектор для хранения одной точки из каждого кластера
/*
            // Проходим по всем точкам
        for (const auto& point : ClusterPts) {
            // Если кластер еще не добавлен в uniqueClusters, добавляем точку в uniqueClusterPoints
            if (uniqueClusters.find(point.cluster) == uniqueClusters.end()) {
                uniqueClusters.insert(point.cluster);
                uniqueClusterPoints.push_back(point);
            }
    }
*/





        
        sf::Text text(identify, font, 24); // 24 - размер шрифта
        text.setPosition(10.0f, 10.0f); // Установка позиции текста
        text.setFillColor(sf::Color::White); // Установка цвета текста
        

        // Очистка окна
            window.clear();

        // Рисование всех точек
            sf::CircleShape circle(5.0f); // радиус круга (точки)
            circle.setFillColor(sf::Color::Red); // цвет точек

            for (auto& plot : points) {
                circle.setPosition(plot.x, plot.y); // установка позиции точки
                window.draw(circle);
            
            }


        // Рисование всех точек
        sf::CircleShape Circle(10.0f); // радиус круга (точки)
        Circle.setFillColor(sf::Color::Green); // цвет точек

        for (auto& plt : onePointPerCluster) {

                Circle.setPosition(plt.y, plt.x); // установка позиции точки
                window.draw(Circle);
            
        }


            drawGrid(window, 10, 10);
            window.draw(text);
            window.display();
            usleep(500000);
        // Обновление окна
        //window.display();


        }
    
    }
}

void drawGrid(sf::RenderWindow& win, int rows, int cols)
{
    // initialize values
    int numLines = rows + cols - 2;
    sf::VertexArray grid(sf::Lines, 2 * (numLines));
    win.setView(win.getDefaultView());
    auto size = win.getView().getSize();
    float rowH = size.y / rows;
    float colW = size.x / cols;

    // Set the color with alpha (transparency)
    sf::Color gridColor(128, 128, 128, 128); // semi-transparent gray

    // row separators
    for (int i = 0; i < rows - 1; i++)
    {
        int r = i + 1;
        float rowY = rowH * r;
        grid[i * 2].position = {0, rowY};
        grid[i * 2 + 1].position = {size.x, rowY};
        grid[i * 2].color = gridColor;
        grid[i * 2 + 1].color = gridColor;
    }

    // column separators
    for (int i = rows - 1; i < numLines; i++)
    {
        int c = i - rows + 2;
        float colX = colW * c;
        grid[i * 2].position = {colX, 0};
        grid[i * 2 + 1].position = {colX, size.y};
        grid[i * 2].color = gridColor;
        grid[i * 2 + 1].color = gridColor;
    }

    // draw it
    win.draw(grid);
}



void identifyTimestamp(std::vector<std::string>& index, std::vector<std::vector<std::string>>& data)
{
    for (const auto& raw : data)
    {
        if (raw[3] == "x")
        {
            continue;
        }
        index.push_back(raw[2]);
    }
    auto last = std::unique(index.begin(), index.end()); // Удаляем дубликаты
    index.erase(last, index.end()); // Удаляем "лишние" элементы после unique
}

