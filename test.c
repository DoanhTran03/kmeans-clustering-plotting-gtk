// gcc anim2.c `pkg-config --cflags --libs gtk+-3.0` -Wall -o anim2 -lm 
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
  
#define PI 3.14159265359
  
static cairo_surface_t *surface = NULL;
static cairo_surface_t *surface2 = NULL;
 
 
  
    static gboolean
on_configure_event(
    GtkWidget         *widget,
    GdkEventConfigure *event,
    gpointer           data)
{
    static int save_w = 0, save_h = 0;
  
    if (save_w == event->width && save_h == event->height)
        return TRUE;
  
    save_w = event->width;
    save_h = event->height;
  
    if (surface)
        cairo_surface_destroy(surface);
  
    surface = gdk_window_create_similar_surface(
        gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
        save_w, save_h);
  
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 0, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
  
    gtk_widget_queue_draw(widget);
  
    return TRUE;
}
  
  
    static gboolean
on_configure_event2(
    GtkWidget         *widget,
    GdkEventConfigure *event,
    gpointer           data)
{
    static int save_w = 0, save_h = 0;
  
    if (save_w == event->width && save_h == event->height)
        return TRUE;
  
    save_w = event->width;
    save_h = event->height;
  
    if (surface2)
        cairo_surface_destroy(surface2);
  
    surface2 = gdk_window_create_similar_surface(
        gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR,
        save_w, save_h);
  
    cairo_t *cr = cairo_create(surface2);
    cairo_set_source_rgb(cr, 0, 1, 1);
    cairo_paint(cr);
    cairo_destroy(cr);
  
    gtk_widget_queue_draw(widget);
  
    return TRUE;
}
  
  
static int ang[3] = {270, 270, 270};
static int ang2[3] = {270, 270, 270};
static gboolean show_line[3] = {TRUE, TRUE, TRUE};
  
  
    static gboolean
on_draw(
    GtkWidget *widget,
    cairo_t   *cr,
    gpointer   data)
{
    int half_width = gtk_widget_get_allocated_width(widget) / 2;
    int half_height = gtk_widget_get_allocated_height(widget) / 2;
  
    cairo_t *cr2 = cairo_create(surface);
    cairo_set_source_rgb(cr2, 0.1, 1, 1);
    cairo_paint(cr2);
  
    for (int i = 0; i < 3; i++) {
        if (!show_line[i]) continue;
        int x = (int)(half_width  * (cos(PI/180*ang[i]) + 1));
        int y = (int)(half_height * (sin(PI/180*ang[i]) + 1));
        cairo_move_to(cr2, half_width, half_height);
        cairo_line_to(cr2, x, y);
        cairo_set_source_rgb(cr2, i==0, i==1, i==2);
        cairo_stroke(cr2);
    }
  
    cairo_destroy(cr2);
  
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);
  
    return TRUE;
}
  
  
  
    static gboolean
on_draw2(
    GtkWidget *widget,
    cairo_t   *cr,
    gpointer   data)
{
    int half_width = gtk_widget_get_allocated_width(widget) / 2;
    int half_height = gtk_widget_get_allocated_height(widget) / 2;
  
    cairo_t *cr3 = cairo_create(surface2);
    cairo_set_source_rgb(cr3, 1, 1, 0);
    cairo_paint(cr3);
  
    for (int i = 0; i < 3; i++) {
        if (!show_line[i]) continue;
        int x = (int)(half_width  * (cos(PI/180*ang2[i]) + 1));
        int y = (int)(half_height * (sin(PI/180*ang2[i]) + 1));
        cairo_move_to(cr3, half_width, half_height);
        cairo_line_to(cr3, x, y);
        cairo_set_source_rgb(cr3, i==0, i==1, i==2);
        cairo_stroke(cr3);
    }
  
    cairo_destroy(cr3);
  
    cairo_set_source_surface(cr, surface2, 0, 0);
    cairo_paint(cr);
  
    return TRUE;
}
  
  
static gboolean stop_timeout = FALSE;
static gboolean stop_timeout2 = FALSE; 
  
    static gboolean
on_timeout(
    gpointer data)
{
    if (stop_timeout) return FALSE;
    for (int i = 0; i < 3; i++)
        ang[i] = (ang[i] + 2*(i+1)) % 360;
    gtk_widget_queue_draw(GTK_WIDGET(data));
    return TRUE;
}
 
 
 
 
    static gboolean
on_timeout2(
    gpointer data)
{
    if (stop_timeout2) return FALSE;
    for (int i = 0; i < 3; i++)
        ang2[i] = (ang2[i] - 2*(i+1)) % 360;
    gtk_widget_queue_draw(GTK_WIDGET(data));
    return TRUE;
}
 
 
 
 
  
    static void
on_destroy(
    void)
{
    if (surface) cairo_surface_destroy(surface);
}
  
    static void
button_clicked(
    GtkWidget *button,
    gpointer   data)
{
    gtk_button_set_label(GTK_BUTTON(button),
        stop_timeout ? "Stop" : "Start");
    stop_timeout = !stop_timeout;
    stop_timeout2 = !stop_timeout2;
    if (!stop_timeout)
        g_timeout_add(50, (GSourceFunc)on_timeout, GTK_WINDOW(data));
    if (!stop_timeout2)
        g_timeout_add(50, (GSourceFunc)on_timeout2, GTK_WINDOW(data));    
}
  
  
static GtkWidget *window;
  
  
    static void
check_button_toggled(
    GtkWidget *checkbutton,
    gpointer data)
{
    *(gboolean*)data =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkbutton));
    gtk_widget_queue_draw(GTK_WIDGET(window));
}
  
    static void
on_activate(
    GtkApplication *app,
    gpointer        user_data)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Animate");
  
    g_signal_connect(window, "destroy",
        G_CALLBACK(on_destroy), NULL);
  
    GtkWidget *outerbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), outerbox);
  
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(outerbox), box, FALSE, FALSE, 0);
  
    GtkWidget *button = gtk_button_new_with_label("Stop");
    g_signal_connect(GTK_BUTTON(button), "clicked",
        G_CALLBACK(button_clicked), window);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
  
    const gchar *s[3] = {"Red", "Green", "Blue"};
    for (int i = 0; i < 3; i++) {
        button = gtk_check_button_new_with_label(s[i]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
        g_signal_connect(button, "toggled",
            G_CALLBACK(check_button_toggled), &show_line[i]);
        gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(button),
            FALSE, FALSE, 0);
    }
  
    GtkWidget *drawing_area = gtk_drawing_area_new();
    GtkWidget *drawing_area2 = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 300, 300);
    gtk_widget_set_size_request(drawing_area2, 300, 300);
    gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), drawing_area2, TRUE, TRUE, 0);
  
    g_signal_connect( drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect( drawing_area2, "draw", G_CALLBACK(on_draw2), NULL);
    g_signal_connect( drawing_area, "configure-event", G_CALLBACK(on_configure_event), NULL);
    g_signal_connect( drawing_area2, "configure-event", G_CALLBACK(on_configure_event2), NULL);
  
    g_timeout_add(50, (GSourceFunc)on_timeout, drawing_area);
    g_timeout_add(50, (GSourceFunc)on_timeout2, drawing_area2);
  
    gtk_widget_show_all(window);
}
  
    int
main(
    int argc,
    char **argv)
{
    GtkApplication *app = gtk_application_new("org.gtk.animate",
        G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}