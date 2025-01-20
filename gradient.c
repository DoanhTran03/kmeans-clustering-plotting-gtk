#include <gtk/gtk.h>
#include <cairo.h>

static void draw_patterns(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    int i, j;

    // Scale to fit the window size
    cairo_scale(cr, width, height);

    // Create a radial gradient pattern
    cairo_pattern_t *radpat = cairo_pattern_create_radial(0.25, 0.25, 0.1, 0.5, 0.5, 0.5);
    cairo_pattern_add_color_stop_rgb(radpat, 0, 1.0, 0.8, 0.8); // Light red
    cairo_pattern_add_color_stop_rgb(radpat, 1, 0.9, 0.0, 0.0); // Dark red

    // Draw rectangles filled with the radial gradient
    for (i = 1; i < 10; i++) {
        for (j = 1; j < 10; j++) {
            cairo_rectangle(cr, i / 10.0 - 0.04, j / 10.0 - 0.04, 0.08, 0.08);
        }
    }
    cairo_set_source(cr, radpat);
    cairo_fill(cr);
    cairo_pattern_destroy(radpat);

    // Create a linear gradient pattern
    cairo_pattern_t *linpat = cairo_pattern_create_linear(0.25, 0.35, 0.75, 0.65);
    cairo_pattern_add_color_stop_rgba(linpat, 0.00, 1, 1, 1, 0);   // Transparent white
    cairo_pattern_add_color_stop_rgba(linpat, 0.25, 0, 1, 0, 0.5); // Semi-transparent green
    cairo_pattern_add_color_stop_rgba(linpat, 0.50, 1, 1, 1, 0);   // Transparent white
    cairo_pattern_add_color_stop_rgba(linpat, 0.75, 0, 0, 1, 0.5); // Semi-transparent blue
    cairo_pattern_add_color_stop_rgba(linpat, 1.00, 1, 1, 1, 0);   // Transparent white

    // Draw a rectangle filled with the linear gradient
    cairo_rectangle(cr, 0.0, 0.0, 1, 1);
    cairo_set_source(cr, linpat);
    cairo_fill(cr);
    cairo_pattern_destroy(linpat);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cairo Patterns");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_patterns, NULL, NULL);

    gtk_window_set_child(GTK_WINDOW(window), drawing_area);
    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.patterns", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
