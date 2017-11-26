#include <gtk/gtk.h>

GtkTextBuffer *buffer;
GtkWidget *box;
GtkWidget *file;
GtkWidget *filemenu;
GtkWidget *menubar;
GtkWidget *new;
GtkWidget *open;
GtkWidget *quit;
GtkWidget *save;
GtkWidget *scrolled_window;
GtkWidget *text_view;
GtkWidget *window;

static void menu_new(GtkTextBuffer *buffer){
    gtk_text_buffer_set_text(
      buffer,
      "",
      0
    );
}

static void menu_open(GtkTextBuffer *buffer){
    GtkWidget *dialog_open;
    dialog_open = gtk_file_chooser_dialog_new(
      "Open File",
      NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Open",
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    gint result = gtk_dialog_run(GTK_DIALOG(dialog_open));
    gtk_widget_destroy(dialog_open);
}

static void menu_save(GtkTextBuffer *buffer){
    GtkWidget *dialog_save;
    dialog_save = gtk_file_chooser_dialog_new(
      "Save File",
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Save",
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    gint result = gtk_dialog_run(GTK_DIALOG(dialog_save));
    gtk_widget_destroy(dialog_save);
}

static void activate(GtkApplication* app, gpointer user_data){
    // Setup CSS.
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(
      screen,
      GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    gtk_css_provider_load_from_file(
      provider,
      g_file_new_for_path("../css/style.css"),
      0
    );
    g_object_unref(provider);

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

    // Setup menu items.
    filemenu = gtk_menu_new();
    menubar = gtk_menu_bar_new();
    file = gtk_menu_item_new_with_mnemonic("_File");
    new = gtk_menu_item_new_with_mnemonic("_New");
    open = gtk_menu_item_new_with_mnemonic("_Open");
    quit = gtk_menu_item_new_with_mnemonic("_Quit");
    save = gtk_menu_item_new_with_mnemonic("_Save");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(file),
      filemenu
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      new
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      open
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      save
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(filemenu),
      quit
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      file
    );

    // Setup menu item callbacks.
    g_signal_connect_swapped(
      new,
      "activate",
      G_CALLBACK(menu_new),
      buffer
    );
    g_signal_connect_swapped(
      open,
      "activate",
      G_CALLBACK(menu_open),
      buffer
    );
    g_signal_connect_swapped(
      save,
      "activate",
      G_CALLBACK(menu_save),
      buffer
    );
    g_signal_connect_swapped(
      quit,
      "activate",
      G_CALLBACK(gtk_widget_destroy),
      window
    );

    // Add everything to a box and show.
    gtk_container_add(
      GTK_CONTAINER(scrolled_window),
      text_view
    );
    box = gtk_box_new(
      GTK_ORIENTATION_VERTICAL,
      0
    );
    gtk_box_pack_start(
      GTK_BOX(box),
      menubar,
      FALSE,
      FALSE,
      0
    );
    gtk_box_pack_start(
      GTK_BOX(box),
      scrolled_window,
      TRUE,
      TRUE,
      0
    );
    gtk_container_add(
      GTK_CONTAINER(window),
      box
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
