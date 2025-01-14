#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure to store a 2D point
typedef struct {
    double x;
    double y;
} Point;

// Global Variables
Point *data_points = NULL;
int n_points;
Point *centroids = NULL;
int n_centroids;
int *assignments = NULL;
int iteration = 0;
int x_unit = 0;
int y_unit = 0;
int cell_unit_length = 30;
int origin_cord[2] = {425, 425}; //Real coordinate of the origin

double* cal_rel_origin(int x, int y) {
    static double result[2]; // Static array to hold results
  //  result[0] = (double)((x*cell_unit_length)/x_unit) + origin_cord[1];
   // result[1] = (double)(origin_cord[0] - (double)((y*cell_unit_length)/y_unit));
   result[0] = origin_cord[0] + (x * cell_unit_length) / x_unit;
   result[1] = origin_cord[1] - (y * cell_unit_length) / y_unit;

    return result;
}

void cal_axises_unit()
{
    int biggest_x = 0;
    int biggest_y = 0;
    for (int i = 0; i < n_points; i++)
    {
        if (data_points[i].x > biggest_x)
        {
            biggest_x = data_points[i].x;
        }
        if (data_points[i].y > biggest_y)
        {
            biggest_y = data_points[i].y;
        }
    }

    for (int i = 0; i < n_centroids; i++)
    {
        if (centroids[i].x > biggest_x)
        {
            biggest_x = centroids[i].x;
        }
        if (centroids[i].y > biggest_y)
        {
            biggest_y = centroids[i].y;
        }
    }

   // x_unit = biggest_x / 10;
   // y_unit = biggest_y / 10;
   x_unit = ceil(abs((double)biggest_x / 10));
   y_unit = ceil(abs((double)biggest_y / 10));
   if (x_unit == 0) x_unit = 1;
   if (y_unit == 0) y_unit = 1;


    printf("%d\n", x_unit);
    printf("%d\n", y_unit);
}


// Function to read data from file
void read_data(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d", &n_points);
    data_points = (Point *)malloc(n_points * sizeof(Point));
    assignments = (int *)malloc(n_points * sizeof(int));

    for (int i = 0; i < n_points; i++) {
        fscanf(file, "%lf %lf", &data_points[i].x, &data_points[i].y);
    }

    fscanf(file, "%d", &n_centroids);
    centroids = (Point *)malloc(n_centroids * sizeof(Point));

    for (int i = 0; i < n_centroids; i++) {
        fscanf(file, "%lf %lf", &centroids[i].x, &centroids[i].y);
    }

    fclose(file);
}

void set_cluster_color(cairo_t *cr, int cluster_id) {
    switch (cluster_id % 5) {
        case 0: cairo_set_source_rgb(cr, 0.5, 0.0, 0.5); break; // Purple
        case 1: cairo_set_source_rgb(cr, 0.0, 1.0, 1.0); break; // Cyan
        case 2: cairo_set_source_rgb(cr, 1.0, 0.5, 0.0); break; // Orange
        case 3: cairo_set_source_rgb(cr, 0.0, 0.5, 0.0); break; // Dark Green
        case 4: cairo_set_source_rgb(cr, 0.5, 0.5, 0.0); break; // Olive
    }
}

void draw_legend(cairo_t *cr) {
    cairo_set_font_size(cr, 20.0);

    // Cluster colors and labels
    const char *labels[] = {"Cluster 0", "Cluster 1", "Cluster 2", "Cluster 3", "Cluster 4", "Centroids"};
    for (int i = 0; i < 5; i++) {
        set_cluster_color(cr, i);
        cairo_rectangle(cr, 800, 50 + i * 30, 15, 15);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, 820, 65 + i * 30);
        cairo_show_text(cr, labels[i]);
    }

    // Centroid label
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black
    cairo_rectangle(cr, 800, 50 + 5 * 30, 15, 15);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);
    cairo_move_to(cr, 820, 65 + 5 * 30);
    cairo_show_text(cr, labels[5]);
}

void draw_grid_lines(cairo_t *cr) {
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.8); // Light gray with transparency
    cairo_set_line_width(cr, 1.0);

    // Draw vertical grid lines
    for (int i = 0; i < 25; i++) {
        double x = 15 + 50 + i * 30; // Adjusted scaling factor
        cairo_move_to(cr, x, 50);
        cairo_line_to(cr, x, 800);
        cairo_stroke(cr);
    }

    // Draw horizontal grid lines
    for (int i = 0; i < 25; i++) {
        double y = 15 + 50 + i * 30; // Adjusted scaling factor
        cairo_move_to(cr, 50, y);
        cairo_line_to(cr, 800, y);
        cairo_stroke(cr);
    }
}

void draw_visualization(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data) {
    // Set the background to white
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    // Draw grid lines
    draw_grid_lines(cr);

    // Draw X and Y axes
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black color
    cairo_set_line_width(cr, 2.0);

    // X-axis
    cairo_move_to(cr, 50, 425);
    cairo_line_to(cr, 800, 425);
    cairo_stroke(cr);

    // Y-axis
    cairo_move_to(cr, 425, 50);
    cairo_line_to(cr, 425, 800);
    cairo_stroke(cr);

    // Mark points on the X-axis
     /*for (int i = -12; i <= 12; i++) { // X-axis range with integer values
        double* pos = cal_rel_origin(i*x_unit, 0);
        double x = pos[0]; // Adjusted scaling factor

        cairo_move_to(cr, x, pos[1] - 5);
        cairo_line_to(cr, x, pos[1] + 5);
        cairo_stroke(cr);

        // Draw the label
        cairo_set_font_size(cr, 12.0);
        char label[10];
        sprintf(label, "%d", i*x_unit);
        cairo_move_to(cr, x - 10, pos[1] + 20);
        cairo_show_text(cr, label);
    } */

    // Mark points on the Y-axis
  /*  for (int i = -12; i <= 12; i++) { // Y-axis range with integer values
        if (i == 0) {
            continue;
        }

        double* pos = cal_rel_origin(0, i*y_unit);
        double y = pos[1]; // Adjusted scaling factor
        
        cairo_move_to(cr, pos[0] -5, y);
        cairo_line_to(cr, pos[0] + 5, y);
        cairo_stroke(cr);

        // Draw the label
        cairo_set_font_size(cr, 12.0);
        char label[10];
        sprintf(label, "%d", i*y_unit);
        cairo_move_to(cr, pos[0] + 10, y + 5);
        cairo_show_text(cr, label);
    } */

    // Dynamic X-axis grid and labels
for (int i = -12; i <= 12; i++) { // Change range dynamically based on your requirements
    double* pos = cal_rel_origin(i * x_unit, 0);
    double x = pos[0];

    cairo_move_to(cr, x, origin_cord[1] - 5);
    cairo_line_to(cr, x, origin_cord[1] + 5);
    cairo_stroke(cr);

    // Draw the label
    cairo_set_font_size(cr, 12.0);
    char label[10];
    sprintf(label, "%d", i * x_unit);
    cairo_move_to(cr, x - 10, origin_cord[1] + 20);
    cairo_show_text(cr, label);
}

// Dynamic Y-axis grid and labels
for (int i = -12; i <= 12; i++) {
    if (i == 0) continue;

    double* pos = cal_rel_origin(0, i * y_unit);
    double y = pos[1];

    cairo_move_to(cr, origin_cord[0] - 5, y);
    cairo_line_to(cr, origin_cord[0] + 5, y);
    cairo_stroke(cr);

    // Draw the label
    cairo_set_font_size(cr, 12.0);
    char label[10];
    sprintf(label, "%d", i * y_unit);
    cairo_move_to(cr, origin_cord[0] + 10, y + 5);
    cairo_show_text(cr, label);
}


    


    // Draw data points with cluster colors
    /* for (int i = 0; i < n_points; i++) {
        set_cluster_color(cr, assignments[i]);
        double x = 50 + data_points[i].x * 30; // Adjusted scaling factor
        double y = 425 - data_points[i].y * 30; // Adjusted scaling factor
        cairo_arc(cr, x, y, 5.0, 0, 2 * M_PI);
        cairo_fill(cr);
    } */

    for (int i = 0; i < n_points; i++) {
    set_cluster_color(cr, assignments[i]); // Set the color for the cluster
    double *pos = cal_rel_origin(data_points[i].x, data_points[i].y);
    cairo_arc(cr, pos[0], pos[1], 5.0, 0, 2 * M_PI); // Draw circle for data point
    cairo_fill(cr); // Fill the circle
}


    double* pos = cal_rel_origin(20, 12);
    cairo_arc(cr, pos[0], pos[1], 5.0, 0, 2 * M_PI);
    cairo_fill(cr);

    // Draw centroids
    /* cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black color for centroids
    cairo_set_line_width(cr, 3.0);
    for (int i = 0; i < n_centroids; i++) {
        double x = 50 + centroids[i].x * 30; // Adjusted scaling factor
        double y = 425 - centroids[i].y * 30; // Adjusted scaling factor
        cairo_move_to(cr, x - 10, y);
        cairo_line_to(cr, x + 10, y);
        cairo_move_to(cr, x, y - 10);
        cairo_line_to(cr, x, y + 10);
        cairo_stroke(cr);
    } */

   /* cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black color for centroids
for (int i = 0; i < n_centroids; i++) {
    double *pos = cal_rel_origin(centroids[i].x, centroids[i].y);
    cairo_rectangle(cr, pos[0] - 5, pos[1] - 5, 10, 10); // Draw square for centroid
    cairo_fill(cr); // Fill the square
}*/

// Draw centroids as "X" markers
cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black color for centroids
cairo_set_line_width(cr, 2.0);
for (int i = 0; i < n_centroids; i++) {
    double *pos = cal_rel_origin(centroids[i].x, centroids[i].y);
    // Draw the "X" marker
    cairo_move_to(cr, pos[0] - 5, pos[1] - 5); // Top-left to bottom-right
    cairo_line_to(cr, pos[0] + 5, pos[1] + 5);
    cairo_move_to(cr, pos[0] - 5, pos[1] + 5); // Bottom-left to top-right
    cairo_line_to(cr, pos[0] + 5, pos[1] - 5);
    cairo_stroke(cr); // Stroke the lines
}



   /* // Display iteration number
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_set_font_size(cr, 20.0);
    char iteration_label[50];
    sprintf(iteration_label, "Iteration: %d", iteration);
    cairo_move_to(cr, 10, 30);
    cairo_show_text(cr, iteration_label); */

    // Draw the legend
    draw_legend(cr); 
} 

void update_clusters() {
    // Assign data points to the nearest centroid
    for (int i = 0; i < n_points; i++) {
        double min_distance = INFINITY;
        int closest_centroid = -1;

        for (int j = 0; j < n_centroids; j++) {
            double dx = data_points[i].x - centroids[j].x;
            double dy = data_points[i].y - centroids[j].y;
            double distance = sqrt(dx * dx + dy * dy);

            if (distance < min_distance) {
                min_distance = distance;
                closest_centroid = j;
            }
        }

        assignments[i] = closest_centroid;
    }

    // Update centroids based on assigned points
    Point *new_centroids = (Point *)calloc(n_centroids, sizeof(Point));
    int *counts = (int *)calloc(n_centroids, sizeof(int));

    for (int i = 0; i < n_points; i++) {
        int cluster_id = assignments[i];
        new_centroids[cluster_id].x += data_points[i].x;
        new_centroids[cluster_id].y += data_points[i].y;
        counts[cluster_id]++;
    }

    for (int i = 0; i < n_centroids; i++) {
        if (counts[i] > 0) {
            new_centroids[i].x /= counts[i];
            new_centroids[i].y /= counts[i];
        }
    }

    for (int i = 0; i < n_centroids; i++) {
        centroids[i] = new_centroids[i];
    }

    free(new_centroids);
    free(counts);

    iteration++;
}

gboolean on_timer(gpointer user_data) {
    GtkWidget *drawing_area = GTK_WIDGET(user_data);
    update_clusters();
    gtk_widget_queue_draw(drawing_area);
    return G_SOURCE_CONTINUE;
}

// Signal handler for opening files
static void open_app(GApplication *app, GFile **files, int n_files, const char *hint) {
    if (n_files > 0) {
        const char *filename = g_file_get_path(files[0]);
        read_data(filename);  // Call the existing read_data function
        cal_axises_unit();
    }
    g_application_activate(app);  // Activate the application
}

// Signal handler for application activation
static void activate(GtkApplication *app, gpointer user_data) {
    if (!data_points || !centroids) {
        fprintf(stderr, "Error: No data loaded. Please provide a valid data file.\n");
        return;
    }

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 850, 850);
    gtk_window_set_title(GTK_WINDOW(window), "K-Means Visualization");

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_area), 950);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_area), 800);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_visualization, NULL, NULL);
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    g_timeout_add(1000, on_timer, drawing_area);

    gtk_window_present(GTK_WINDOW(window));
}

// Main function
int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.kmeans", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "open", G_CALLBACK(open_app), NULL);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    g_object_unref(app);
    free(data_points);
    free(centroids);
    free(assignments);

    return status;
}
