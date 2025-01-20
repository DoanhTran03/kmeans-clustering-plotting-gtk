#include <gtk/gtk.h>
#include <cairo.h>

static void draw_colored_points_and_line(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    // Scale to fit the window size
    cairo_scale(cr, width, height);

    // Draw the first point (Blue)
    cairo_set_source_rgb(cr, 0, 0, 1); // Blue color
    cairo_arc(cr, 0.25, 0.25, 0.01, 0, 2 * G_PI); // Draw a small circle at (0.25, 0.25)
    cairo_fill(cr);

    // Move the reference to the first point
    cairo_move_to(cr, 0.25, 0.25);

    // Draw the second point (Red)
    cairo_set_source_rgb(cr, 1, 0, 0); // Red color
    cairo_arc(cr, 0.75, 0.75, 0.01, 0, 2 * G_PI); // Draw a small circle at (0.75, 0.75)
    cairo_fill(cr);

    // Draw a line connecting the two points (Black)
    cairo_set_source_rgb(cr, 0, 0, 0); // Black color for the line
    cairo_line_to(cr, 0.75, 0.75);     // Draw line to (0.75, 0.75)
    cairo_set_line_width(cr, 0.005);   // Set line width
    cairo_stroke(cr);                 // Render the line
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Colored Points and Line");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_colored_points_and_line, NULL, NULL);

    gtk_window_set_child(GTK_WINDOW(window), drawing_area);
    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.coloredpoints", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
