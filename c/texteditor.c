#include <gtk/gtk.h>

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

    if(result == GTK_RESPONSE_ACCEPT){
        char *filename;
        gssize length;
        gchar *content;

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog_open);
        filename = gtk_file_chooser_get_filename(chooser);

        if(g_file_get_contents(filename, &content, &length, NULL)){
            gtk_text_buffer_set_text(
              buffer,
              content,
              length
            );
        }

        g_free(content);
        g_free(filename);
    }

    gtk_widget_destroy(dialog_open);
}

static void menu_saveas(GtkTextBuffer *buffer){
    GtkWidget *dialog_saveas;
    dialog_saveas = gtk_file_chooser_dialog_new(
      "Save File",
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Save",
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    gint result = gtk_dialog_run(GTK_DIALOG(dialog_saveas));

    if(result == GTK_RESPONSE_ACCEPT){
        char *filename;
        gchar *content;
        GtkTextIter end;
        GtkTextIter start;

        gtk_text_buffer_get_start_iter(
          buffer,
          &start
        );
        gtk_text_buffer_get_end_iter(
          buffer,
          &end
        );

        content = gtk_text_buffer_get_text(
          buffer,
          &start,
          &end,
          FALSE
        );

        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog_saveas);
        filename = gtk_file_chooser_get_filename(chooser);

        g_file_set_contents(
          filename,
          content,
          -1,
          NULL
        );

        g_free(content);
        g_free(filename);
    }

    gtk_widget_destroy(dialog_saveas);
}

static void activate(GtkApplication* app, gpointer user_data){
    GtkTextBuffer *buffer;
    GtkWidget *box;
    GtkWidget *menu_edit;
    GtkWidget *menu_file;
    GtkWidget *menubar;
    GtkWidget *menuitem_edit_copy;
    GtkWidget *menuitem_edit_cut;
    GtkWidget *menuitem_edit_delete;
    GtkWidget *menuitem_edit_paste;
    GtkWidget *menuitem_edit_selectall;
    GtkWidget *menuitem_edit;
    GtkWidget *menuitem_file_new;
    GtkWidget *menuitem_file_open;
    GtkWidget *menuitem_file_quit;
    GtkWidget *menuitem_file_saveas;
    GtkWidget *menuitem_file;
    GtkWidget *notebook;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkWidget *window;

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

    // Setup notebook and tab with scrollable text view.
    notebook = gtk_notebook_new();
    buffer = gtk_text_buffer_new(NULL);
    text_view = gtk_text_view_new_with_buffer(buffer);
    gtk_text_view_set_wrap_mode(
      GTK_TEXT_VIEW(text_view),
      GTK_WRAP_WORD
    );
    scrolled_window = gtk_scrolled_window_new(
      NULL,
      NULL
    );
    gtk_scrolled_window_set_policy(
      GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC
    );
    gtk_container_add(
      GTK_CONTAINER(scrolled_window),
      text_view
    );
    gtk_notebook_append_page(
      GTK_NOTEBOOK(notebook),
      scrolled_window,
      NULL
    );

    // Setup menu items.
    menubar = gtk_menu_bar_new();
    menu_file = gtk_menu_new();
    menuitem_file = gtk_menu_item_new_with_mnemonic("_File");
    menuitem_file_new = gtk_menu_item_new_with_mnemonic("_New");
    menuitem_file_open = gtk_menu_item_new_with_mnemonic("_Open");
    menuitem_file_quit = gtk_menu_item_new_with_mnemonic("_Quit");
    menuitem_file_saveas = gtk_menu_item_new_with_mnemonic("Save _As");
    menu_edit = gtk_menu_new();
    menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
    menuitem_edit_copy = gtk_menu_item_new_with_mnemonic("_Copy");
    menuitem_edit_cut = gtk_menu_item_new_with_mnemonic("C_ut");
    menuitem_edit_paste = gtk_menu_item_new_with_mnemonic("_Paste");
    menuitem_edit_delete = gtk_menu_item_new_with_mnemonic("_Delete");
    menuitem_edit_selectall = gtk_menu_item_new_with_mnemonic("_Select All");
    // File menu.
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menu_file
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_new
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_open
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_saveas
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_quit
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_file
    );
    // Edit menu.
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_edit),
      menu_edit
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      menuitem_edit_copy
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      menuitem_edit_cut
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      menuitem_edit_paste
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      menuitem_edit_delete
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      menuitem_edit_selectall
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_edit
    );

    // Setup menu item callbacks.
    g_signal_connect_swapped(
      menuitem_file_new,
      "activate",
      G_CALLBACK(menu_new),
      buffer
    );
    g_signal_connect_swapped(
      menuitem_file_open,
      "activate",
      G_CALLBACK(menu_open),
      buffer
    );
    g_signal_connect_swapped(
      menuitem_file_saveas,
      "activate",
      G_CALLBACK(menu_saveas),
      buffer
    );
    g_signal_connect_swapped(
      menuitem_file_quit,
      "activate",
      G_CALLBACK(gtk_widget_destroy),
      window
    );

    // Add everything to a box and show.
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
      notebook,
      TRUE,
      TRUE,
      0
    );
    gtk_container_add(
      GTK_CONTAINER(window),
      box
    );
    gtk_widget_show_all(window);
    gtk_widget_grab_focus(text_view);
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
