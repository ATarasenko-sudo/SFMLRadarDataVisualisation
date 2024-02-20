#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <SFML/Graphics.hpp>


#include <unistd.h>

using namespace std;
#define MINIMUM_POINTS 1    // minimum number of cluster
#define EPSILON (50)  // distance for clustering, metre^2


struct Point {
    double x, y;     // coordinates
    int cluster;     // no default cluster
    double minDist;  // default infinite distance to nearest cluster

    Point() : x(0.0), y(0.0), cluster(-1), minDist(__DBL_MAX__) {}
    Point(double x, double y) : x(x), y(y), cluster(-1), minDist(__DBL_MAX__) {}

    /**
     * Computes the (square) euclidean distance between this point and another
     */
    double distance(Point p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};


void kMeansClustering(vector<Point>* points, int epochs, int k); 

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
    std::vector<Point> uniqueClusterPoints;
    std::unordered_set<int> uniqueClusters;
    
        while (window.isOpen())
    {

        for (const auto& identify : index){
            
            points.clear();
            ClusterPts.clear();
            uniqueClusters.clear();
            uniqueClusterPoints.clear();
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
            kMeansClustering(&ClusterPts, 1, 10);
            // Вектор для хранения одной точки из каждого кластера

            // Проходим по всем точкам
        for (const auto& point : ClusterPts) {
            // Если кластер еще не добавлен в uniqueClusters, добавляем точку в uniqueClusterPoints
            if (uniqueClusters.find(point.cluster) == uniqueClusters.end()) {
                uniqueClusters.insert(point.cluster);
                uniqueClusterPoints.push_back(point);
            }
    }






        
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

        for (auto& plt : uniqueClusterPoints) {

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


void kMeansClustering(vector<Point>* points, int epochs, int k) {
    int n = points->size();

    // Randomly initialise centroids
    // The index of the centroid within the centroids vector
    // represents the cluster label.
    vector<Point> centroids;
    srand(time(0));
    for (int i = 0; i < k; ++i) {
        centroids.push_back(points->at(rand() % n));
    }

    for (int i = 0; i < epochs; ++i) {
        // For each centroid, compute distance from centroid to each point
        // and update point's cluster if necessary
        for (vector<Point>::iterator c = begin(centroids); c != end(centroids);
             ++c) {
            int clusterId = c - begin(centroids);

            for (vector<Point>::iterator it = points->begin();
                 it != points->end(); ++it) {
                Point p = *it;
                double dist = c->distance(p);
                if (dist < p.minDist) {
                    p.minDist = dist;
                    p.cluster = clusterId;
                }
                *it = p;
            }
        }

        // Create vectors to keep track of data needed to compute means
        vector<int> nPoints;
        vector<double> sumX, sumY;
        for (int j = 0; j < k; ++j) {
            nPoints.push_back(0);
            sumX.push_back(0.0);
            sumY.push_back(0.0);
        }

        // Iterate over points to append data to centroids
        for (vector<Point>::iterator it = points->begin(); it != points->end();
             ++it) {
            int clusterId = it->cluster;
            nPoints[clusterId] += 1;
            sumX[clusterId] += it->x;
            sumY[clusterId] += it->y;

            it->minDist = __DBL_MAX__;  // reset distance
        }
        // Compute the new centroids
        for (vector<Point>::iterator c = begin(centroids); c != end(centroids);
             ++c) {
            int clusterId = c - begin(centroids);
            c->x = sumX[clusterId] / nPoints[clusterId];
            c->y = sumY[clusterId] / nPoints[clusterId];
        }
    }

    // Write to csv
    ofstream myfile;
    myfile.open("output.csv");
    myfile << "x,y,c" << endl;

    for (vector<Point>::iterator it = points->begin(); it != points->end();
         ++it) {
        myfile << it->x << "," << it->y << "," << it->cluster << endl;
    }
    myfile.close();
}