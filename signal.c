#include <gtk/gtk.h>
#include <pthread.h>
#include <signal.h>
#include <cairo.h>

// Structure for animation data
typedef struct {
    int width;          // Width of the drawing area
    int height;         // Height of the drawing area
    int x_pos;          // Current x-position of the rectangle
    int speed;          // Speed of the rectangle's movement
    GtkWidget *drawing_area; // Pointer to the drawing area widget
} AnimationData;

// Draw function for GtkDrawingArea
void draw_func(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    AnimationData *data = (AnimationData *)user_data;

    // Clear the background
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9); // Light gray background
    cairo_paint(cr);

    // Draw the animated rectangle
    cairo_set_source_rgb(cr, 0.2, 0.4, 0.8); // Blue rectangle
    cairo_rectangle(cr, data->x_pos, height / 4, 100, 100);
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
            // Update the x-position of the rectangle
            data->x_pos += data->speed;

            // Bounce off the edges
            if (data->x_pos + 100 > data->width || data->x_pos < 0) {
                data->speed = -data->speed;
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
    gtk_window_set_title(GTK_WINDOW(window), "GTK4 Animation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a drawing area
    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    // Initialize animation data
    AnimationData *animation_data = g_new(AnimationData, 1);
    animation_data->width = 800;
    animation_data->height = 600;
    animation_data->x_pos = 0;
    animation_data->speed = 5;
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
    // Block SIGALRM in the main thread
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    // Create a GtkApplication
    GtkApplication *app = gtk_application_new("com.example.GtkAnimation", G_APPLICATION_FLAGS_NONE);

    // Connect the activate signal
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    g_object_unref(app);
    return status;
}
