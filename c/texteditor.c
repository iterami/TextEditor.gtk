#include <gtk/gtk.h>

static void activate(GtkApplication* app, gpointer user_data){
    GtkTextBuffer *buffer;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkWidget *window;

    // Setup window.
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(
      GTK_WINDOW(window),
      800,
      600
    );
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_window_set_title(
      GTK_WINDOW(window),
      "TextEditor.c"
    );

    // Setup text view.
    buffer = gtk_text_buffer_new(NULL);
    text_view = gtk_text_view_new_with_buffer(buffer);
    gtk_text_view_set_wrap_mode(
      GTK_TEXT_VIEW(text_view),
      GTK_WRAP_WORD
    );

    // Setup scrolled window.
    scrolled_window = gtk_scrolled_window_new(
      NULL,
      NULL
    );
    gtk_scrolled_window_set_policy(
      GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC
    );

    // Add everything and show.
    gtk_container_add(
      GTK_CONTAINER(scrolled_window),
      text_view
    );
    gtk_container_add(
      GTK_CONTAINER(window),
      scrolled_window
    );
    gtk_widget_show_all(window);
}

int main(int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new(
      "com.iterami.texteditor",
      G_APPLICATION_FLAGS_NONE
    );
    g_signal_connect(
      app,
      "activate",
      G_CALLBACK(activate),
      NULL
    );
    status = g_application_run(
      G_APPLICATION(app),
      argc,
      argv
    );
    g_object_unref(app);

    return status;
}
