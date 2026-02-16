#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ---------- STRUCT DEFINITIONS ----------
typedef struct {
    char name[100];
    double lat;
    double lon;
} Node;

typedef struct {
    int from;
    int to;
    double distance;
    double cost;
    double time;
    int transport; // 0=car,1=metro,2=bikolpo,3=uttara
} Edge;


#define MAX_NODES 5000
#define MAX_EDGES 20000

Node nodes[MAX_NODES];
Edge edges[MAX_EDGES];

int node_count = 0;
int edge_count = 0;





double toRad(double degree) {
    return degree * M_PI / 180.0;
}

double haversine(double lat1, double lon1,
                 double lat2, double lon2) {

    double R = 6371.0; // Earth radius in km

    double dLat = toRad(lat2 - lat1);
    double dLon = toRad(lon2 - lon1);

    lat1 = toRad(lat1);
    lat2 = toRad(lat2);

    double a = sin(dLat/2)*sin(dLat/2) +
               cos(lat1)*cos(lat2) *
               sin(dLon/2)*sin(dLon/2);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R * c;
}


void addEdge(int from, int to,
    double lat1, double lon1,
    double lat2, double lon2,
    int transport) {

double distance = haversine(lat1, lon1, lat2, lon2);

double rate;
double speed;

if (transport == 0) { rate = 20; speed = 20; }
if (transport == 1) { rate = 5;  speed = 15; }
if (transport == 2) { rate = 7;  speed = 10; }
if (transport == 3) { rate = 7;  speed = 12; }

edges[edge_count].from = from;
edges[edge_count].to = to;
edges[edge_count].distance = distance;
edges[edge_count].cost = distance * rate;
edges[edge_count].time = distance / speed;
edges[edge_count].transport = transport;

edge_count++;
}



// dijkstra()
double dist[MAX_NODES];
int visited[MAX_NODES];
int parent[MAX_NODES];

void dijkstra(int source, int n, int mode) {

    for (int i = 0; i < n; i++) {
        dist[i] = 1e18;
        visited[i] = 0;
        parent[i] = -1;
    }

    dist[source] = 0;

    for (int i = 0; i < n; i++) {

        int u = -1;
        double min = 1e18;

        for (int j = 0; j < n; j++) {
            if (!visited[j] && dist[j] < min) {
                min = dist[j];
                u = j;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        for (int e = 0; e < edge_count; e++) {

            if (edges[e].from == u) {

                int v = edges[e].to;
                double weight;

                if (mode == 1)
                    weight = edges[e].distance;
                else if (mode == 2)
                    weight = edges[e].cost;
                else
                    weight = edges[e].time;

                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                }
            }
        }
    }
}


void generateKML(int destination) {

    FILE *fp = fopen("route.kml", "w");

    fprintf(fp, "<kml><Document><Placemark>");
    fprintf(fp, "<LineString><coordinates>\n");

    int path[1000];
    int count = 0;

    int current = destination;

    while (current != -1) {
        path[count++] = current;
        current = parent[current];
    }

    for (int i = count-1; i >= 0; i--) {
        fprintf(fp, "%lf,%lf,0\n",
                nodes[path[i]].lon,
                nodes[path[i]].lat);
    }

    fprintf(fp, "</coordinates></LineString>");
    fprintf(fp, "</Placemark></Document></kml>");

    fclose(fp);
}



int main() {
    int problem;
    double src_lat, src_lon, dest_lat, dest_lon;

    printf("Enter problem number (1-6): ");
    scanf("%d", &problem);

    printf("Enter source latitude and longitude: ");
    scanf("%lf %lf", &src_lat, &src_lon);

    printf("Enter destination latitude and longitude: ");
    scanf("%lf %lf", &dest_lat, &dest_lon);

    printf("Problem %d\n", problem);
    printf("Source: (%lf, %lf)\n", src_lat, src_lon);
    printf("Destination: (%lf, %lf)\n", dest_lat, dest_lon);


    


    FILE *fp = fopen("route.kml", "w"); // Create a new KML file
    if (fp == NULL) {
        printf("Error creating file!\n");
        return 1;
    }

    // Basic KML template
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n");
    fprintf(fp, "<Document>\n");
    fprintf(fp, "<Placemark>\n");
    fprintf(fp, "<name>My Route</name>\n");
    fprintf(fp, "<LineString>\n");
    fprintf(fp, "<coordinates>\n");

    // Example coordinates (longitude,latitude)
    fprintf(fp, "90.4125,23.8103,0 ");  // Dhaka example
    fprintf(fp, "90.4000,23.8050,0\n");

    fprintf(fp, "</coordinates>\n");
    fprintf(fp, "</LineString>\n");
    fprintf(fp, "</Placemark>\n");
    fprintf(fp, "</Document>\n");
    fprintf(fp, "</kml>\n");

    fclose(fp);
    printf("route.kml saved successfully!\n");
    return 0;
}


    // TODO: call your routing function here

