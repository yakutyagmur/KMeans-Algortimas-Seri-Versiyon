#include <iostream>
#include <cmath>
#include <fstream>
#include <chrono>
#include "Point.h"
#include "Cluster.h"
#include <omp.h>

using namespace std;
using namespace std::chrono;

double max_range = 100000;
int num_point = 100000;
int num_cluster = 10;
int max_iterations = 20;

vector<Point> init_point(int num_point);
vector<Cluster> init_cluster(int num_cluster);
void compute_distance(vector<Point>& points, vector<Cluster>& clusters);
double euclidean_dist(Point point, Cluster cluster);
bool update_clusters(vector<Cluster>& clusters);
void draw_chart_gnu(vector<Point>& points);


int main() {

    srand(time(NULL));

    printf("Nokta sayisi %d\n", num_point);
    printf("Kume sayisi %d\n", num_cluster);

    double time_point1 = omp_get_wtime();

    printf("Olusturmalar basliyor..\n");

    printf("Nokta olusturuldu..\n");
    vector<Point> points = init_point(num_point);
    printf("Noktalar basliyor.. \n");

    printf("Cluster(Kume) olusturuldu..\n");
    vector<Cluster> clusters = init_cluster(num_cluster);
    printf("Kume baslatiliyor.. \n");

    double time_point2 = omp_get_wtime();
    auto duration = time_point2 - time_point1;

    printf("Noktalar ve kumeler: %f saniyede olusturuldu.\n", duration);

    bool conv = true;
    int iterations = 0;

    printf("Iterasyonlar..\n");


    //Algoritma, iterasyonlar maks_iterasyondan b�y�k oldu�unda veya k�meler hareket etmedi�inde durur
    while (conv && iterations < max_iterations) {

        iterations++;

        compute_distance(points, clusters);

        conv = update_clusters(clusters);

        printf("Iterasyon %d bitti\n", iterations);

    }

    double time_point3 = omp_get_wtime();
    duration = time_point3 - time_point2;

    printf("Iterasyon sayisi: %d, toplam zaman: %f saniye, iterasyon basina dusen sure: %f saniye\n",
        iterations, duration, duration / iterations);

    try {
        printf("Grafik ciziliyor...\n");
        draw_chart_gnu(points);
    }
    catch (int e) {
        printf("Grafik cizilemedi,gnuplot bulunamadi");
    }

 
    return 0;

}

//num_point ile belirtilen noktalar� ba�lat�yor
vector<Point> init_point(int num_point) {

    vector<Point> points(num_point);
    Point* ptr = &points[0];

    for (int i = 0; i < num_point; i++) {

        Point* point = new Point(rand() % (int)max_range, rand() % (int)max_range);

        ptr[i] = *point;

    }

    return points;

}

//num_cluster ile belirtilen k�meleri ba�lat�yor
vector<Cluster> init_cluster(int num_cluster) {

    vector<Cluster> clusters(num_cluster);
    Cluster* ptr = &clusters[0];

    for (int i = 0; i < num_cluster; i++) {

        Cluster* cluster = new Cluster(rand() % (int)max_range, rand() % (int)max_range);

        ptr[i] = *cluster;

    }

    return clusters;
}

//Her nokta i�in, her k�me aras�ndaki mesafeyi hesaplay�p  noktay� en yak�n k�meye at�yor
//Mesafeyi �klid mesafesi ile hesapl�yor

void compute_distance(vector<Point>& points, vector<Cluster>& clusters) {

    unsigned long points_size = points.size();
    unsigned long clusters_size = clusters.size();

    double min_distance;
    int min_index;

    for (int i = 0; i < points_size; i++) {

        Point& point = points[i];

        min_distance = euclidean_dist(point, clusters[0]);
        min_index = 0;

        for (int j = 1; j < clusters_size; j++) {

            Cluster& cluster = clusters[j];

            double distance = euclidean_dist(point, cluster);

            if (distance < min_distance) {

                min_distance = distance;
                min_index = j;
            }
        }
        points[i].set_cluster_id(min_index);

        clusters[min_index].add_point(points[i]);
    }
}

double euclidean_dist(Point point, Cluster cluster) {

    double distance = sqrt(pow(point.get_x_coord() - cluster.get_x_coord(), 2) +
        pow(point.get_y_coord() - cluster.get_y_coord(), 2));

    return distance;
}

//Her k�me i�in koordinatlar� g�ncelliyor. Yaln�zca bir k�me hareket ederse d�ng�den ��k�l�r d�n�� de�eri true olur
bool update_clusters(vector<Cluster>& clusters) {

    bool conv = false;

    for (int i = 0; i < clusters.size(); i++) {

        conv = clusters[i].update_coords();
        clusters[i].free_point();
    }

    return conv;
}


//Gnuplot ile nokta grafi�i �izme
void draw_chart_gnu(vector<Point>& points) {

    ofstream outfile("data.txt");

    for (int i = 0; i < points.size(); i++) {

        Point point = points[i];
        outfile << point.get_x_coord() << " " << point.get_y_coord() << " " << point.get_cluster_id() << std::endl;

    }

    outfile.close();
    system("gnuplot -p -e \"plot 'data.txt' using 1:2:3 with points palette notitle\"");
    remove("data.txt");

}