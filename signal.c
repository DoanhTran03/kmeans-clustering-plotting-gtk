#include <gtk/gtk.h>
#include <pthread.h>
#include <signal.h>
#include <cairo.h>
#include <stdlib.h>

// Structure for animation data
typedef struct {
    int width;          // Width of the drawing area
    int height;         // Height of the drawing area
    int x_pos;          // Current x-position of the circle
    int y_pos;          // Current y-position of the circle
    int x_speed;        // Speed of the circle's horizontal movement
    int y_speed;        // Speed of the circle's vertical movement
    double r, g, b;     // Current color (red, green, blue) of the circle
    GtkWidget *drawing_area; // Pointer to the drawing area widget
} AnimationData;

// Function to generate a random color
void generate_random_color(double *r, double *g, double *b) {
    *r = (double)rand() / RAND_MAX;
    *g = (double)rand() / RAND_MAX;
    *b = (double)rand() / RAND_MAX;
}

// Draw function for GtkDrawingArea
void draw_func(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    AnimationData *data = (AnimationData *)user_data;

    // Clear the background to black
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // Black background
    cairo_paint(cr);

    // Draw the animated circle with the current color
    cairo_set_source_rgb(cr, data->r, data->g, data->b); // Circle color
    cairo_arc(cr, data->x_pos + 50, data->y_pos + 50, 50, 0, 2 * G_PI); // Circle
    cairo_fill(cr);
}

// Thread function for animation updates
void *animation_thread(void *ptr) {
    AnimationData *data = (AnimationData *)ptr;

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);

    while (1) {
        siginfo_t info;
        if (sigwaitinfo(&sigset, &info) > 0) {
            // Update the x-position and y-position of the circle
            data->x_pos += data->x_speed;
            data->y_pos += data->y_speed;

            // Bounce off horizontal edges
            if (data->x_pos + 100 > data->width || data->x_pos < 0) {
                data->x_speed = -data->x_speed;
                // Change color
                generate_random_color(&data->r, &data->g, &data->b);
            }

            // Bounce off vertical edges
            if (data->y_pos + 100 > data->height || data->y_pos < 0) {
                data->y_speed = -data->y_speed;
                // Change color
                generate_random_color(&data->r, &data->g, &data->b);
            }

            // Queue a redraw on the main thread
            g_idle_add((GSourceFunc)gtk_widget_queue_draw, data->drawing_area);
        }
    }
    return NULL;
}

// Timer callback to send SIGALRM to the thread
gboolean timer_callback(gpointer user_data) {
    static pthread_t thread_info;
    static int thread_started = 0;

    if (!thread_started) {
        pthread_create(&thread_info, NULL, animation_thread, user_data);
        thread_started = 1;
    }

    pthread_kill(thread_info, SIGALRM);
    return G_SOURCE_CONTINUE;
}

// Application activate callback
static void on_activate(GtkApplication *app, gpointer user_data) {
    // Create the main application window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "DVD Logo Animation with Color Change");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a drawing area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Initialize animation data
    AnimationData *animation_data = g_new(AnimationData, 1);
    animation_data->width = 800;
    animation_data->height = 600;
    animation_data->x_pos = 0;
    animation_data->y_pos = 0;
    animation_data->x_speed = 5;
    animation_data->y_speed = 3; // Vertical speed for the bouncing effect
    generate_random_color(&animation_data->r, &animation_data->g, &animation_data->b); // Initial random color
    animation_data->drawing_area = drawing_area;

    // Set the draw function
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_func, animation_data, NULL);

    // Add a timer for periodic updates
    g_timeout_add(16, timer_callback, animation_data); // ~60 FPS

    // Show the window
    gtk_window_present(GTK_WINDOW(window));
}

// Main function
int main(int argc, char *argv[]) {
    // Seed the random number generator
    srand(time(NULL));

    // Block SIGALRM in the main thread
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    // Create a GtkApplication
    GtkApplication *app = gtk_application_new("com.example.DVDLogoAnimation", G_APPLICATION_FLAGS_NONE);

    // Connect the activate signal
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    g_object_unref(app);
    return status;
}
