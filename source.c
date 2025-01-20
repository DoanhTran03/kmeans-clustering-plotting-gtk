#include <gtk/gtk.h>
#include <cairo.h>

static void draw_shapes(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    // Scale coordinates to fit the window size
    cairo_scale(cr, width, height);

    // Draw black diagonal lines
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 0.2);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, 1, 1);
    cairo_move_to(cr, 1, 0);
    cairo_line_to(cr, 0, 1);
    cairo_stroke(cr);

    // Draw red semi-transparent rectangle in top-left corner
    cairo_rectangle(cr, 0, 0, 0.5, 0.5);
    cairo_set_source_rgba(cr, 1, 0, 0, 0.80); // Red with 80% opacity
    cairo_fill(cr);

    // Draw green semi-transparent rectangle in bottom-left corner
    cairo_rectangle(cr, 0, 0.5, 0.5, 0.5);
    cairo_set_source_rgba(cr, 0, 1, 0, 0.60); // Green with 60% opacity
    cairo_fill(cr);

    // Draw blue semi-transparent rectangle in top-right corner
    cairo_rectangle(cr, 0.5, 0, 0.5, 0.5);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.40); // Blue with 40% opacity
    cairo_fill(cr);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Cairo Drawing");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_shapes, NULL, NULL);

    gtk_window_set_child(GTK_WINDOW(window), drawing_area);
    gtk_widget_show(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.cairo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}