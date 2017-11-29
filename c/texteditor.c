#include <gtk/gtk.h>

GtkNotebook *notebook;
GtkWidget *window;

static void save_tab(const char *filename){
    gchar *content;
    GtkTextIter end;
    GtkTextIter start;

    int page = gtk_notebook_get_current_page(notebook);
    GtkWidget *scroll_view;
    scroll_view = gtk_notebook_get_nth_page(
      notebook,
      page
    );
    GtkWidget *text_view;
    text_view = gtk_bin_get_child(GTK_BIN(scroll_view));
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

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

    g_file_set_contents(
      filename,
      content,
      -1,
      NULL
    );
    gtk_notebook_set_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      ),
      gtk_label_new(filename)
    );

    g_free(content);
}

static void close_tab(){
    gtk_notebook_remove_page(
      notebook,
      gtk_notebook_get_current_page(notebook)
    );
}

static void new_tab(){
    GtkTextBuffer *buffer;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;

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
      notebook,
      scrolled_window,
      gtk_label_new("UNSAVED")
    );

    gtk_widget_show_all(window);

    gtk_notebook_set_current_page(
      notebook,
      gtk_notebook_get_n_pages(notebook) - 1
    );
    gtk_widget_grab_focus(text_view);
}

static void menu_open(){
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
            if(gtk_notebook_get_n_pages(notebook) <= 0){
                new_tab();
            }

            int page = gtk_notebook_get_current_page(notebook);
            GtkWidget *scroll_view;
            scroll_view = gtk_notebook_get_nth_page(
              notebook,
              page
            );
            GtkWidget *text_view;
            text_view = gtk_bin_get_child(GTK_BIN(scroll_view));
            GtkTextBuffer *buffer;
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

            gtk_text_buffer_set_text(
              buffer,
              content,
              length
            );
            gtk_notebook_set_tab_label(
              notebook,
              gtk_notebook_get_nth_page(
                notebook,
                page
              ),
              gtk_label_new(filename)
            );
        }

        g_free(content);
        g_free(filename);
    }

    gtk_widget_destroy(dialog_open);
}

static void menu_saveas(){
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
        gchar *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog_saveas);
        filename = gtk_file_chooser_get_filename(chooser);

        save_tab(filename);
    }

    gtk_widget_destroy(dialog_saveas);
}

static void menu_save(){
    const gchar *text;
    GtkWidget *label;

    label = gtk_notebook_get_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    );
    text = gtk_label_get_text(GTK_LABEL(label));

    if(g_strcmp0(
      text,
      "UNSAVED"
    ) == 0){
        menu_saveas();

    }else{
        save_tab(text);
    }
}

static void activate(GtkApplication* app, gpointer user_data){
    GtkWidget *box;
    GtkWidget *menu_edit;
    GtkWidget *menu_file;
    GtkWidget *menu_find;
    GtkWidget *menubar;
    GtkWidget *menuitem_edit_copy;
    GtkWidget *menuitem_edit_cut;
    GtkWidget *menuitem_edit_delete;
    GtkWidget *menuitem_edit_paste;
    GtkWidget *menuitem_edit_selectall;
    GtkWidget *menuitem_edit;
    GtkWidget *menuitem_file_closetab;
    GtkWidget *menuitem_file_newtab;
    GtkWidget *menuitem_file_open;
    GtkWidget *menuitem_file_print;
    GtkWidget *menuitem_file_quit;
    GtkWidget *menuitem_file_save;
    GtkWidget *menuitem_file_saveas;
    GtkWidget *menuitem_file;
    GtkWidget *menuitem_find_find;
    GtkWidget *menuitem_find_findnext;
    GtkWidget *menuitem_find_findprevious;
    GtkWidget *menuitem_find_findreplace;
    GtkWidget *menuitem_find_gotoline;
    GtkWidget *menuitem_find;

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
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    new_tab();

    // Setup menu items.
    menubar = gtk_menu_bar_new();
    // File menu.
    menu_file = gtk_menu_new();
    menuitem_file = gtk_menu_item_new_with_mnemonic("_File");
    menuitem_file_closetab = gtk_menu_item_new_with_mnemonic("_Close Tab");
    menuitem_file_newtab = gtk_menu_item_new_with_mnemonic("_New Tab");
    menuitem_file_open = gtk_menu_item_new_with_mnemonic("_Open...");
    menuitem_file_print = gtk_menu_item_new_with_mnemonic("_Print...");
    menuitem_file_quit = gtk_menu_item_new_with_mnemonic("_Quit");
    menuitem_file_save = gtk_menu_item_new_with_mnemonic("_Save");
    menuitem_file_saveas = gtk_menu_item_new_with_mnemonic("Save _As...");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menu_file
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_newtab
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      menuitem_file_closetab
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
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
      menuitem_file_save
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
      menuitem_file_print
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
    menu_edit = gtk_menu_new();
    menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
    menuitem_edit_copy = gtk_menu_item_new_with_mnemonic("_Copy");
    menuitem_edit_cut = gtk_menu_item_new_with_mnemonic("C_ut");
    menuitem_edit_delete = gtk_menu_item_new_with_mnemonic("_Delete");
    menuitem_edit_paste = gtk_menu_item_new_with_mnemonic("_Paste");
    menuitem_edit_selectall = gtk_menu_item_new_with_mnemonic("_Select All");
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
    // Find menu.
    menu_find = gtk_menu_new();
    menuitem_find = gtk_menu_item_new_with_mnemonic("_Find");
    menuitem_find_find = gtk_menu_item_new_with_mnemonic("_Find...");
    menuitem_find_findnext = gtk_menu_item_new_with_mnemonic("Find _Next");
    menuitem_find_findprevious = gtk_menu_item_new_with_mnemonic("Find _Previous");
    menuitem_find_findreplace = gtk_menu_item_new_with_mnemonic("Find and _Replace...");
    menuitem_find_gotoline = gtk_menu_item_new_with_mnemonic("Go to _Line...");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_find),
      menu_find
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      menuitem_find_find
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      menuitem_find_findnext
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      menuitem_find_findprevious
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      menuitem_find_findreplace
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      menuitem_find_gotoline
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_find
    );

    // Setup menu item callbacks.
    g_signal_connect_swapped(
      menuitem_file_newtab,
      "activate",
      G_CALLBACK(new_tab),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_file_closetab,
      "activate",
      G_CALLBACK(close_tab),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_file_open,
      "activate",
      G_CALLBACK(menu_open),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_file_save,
      "activate",
      G_CALLBACK(menu_save),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_file_saveas,
      "activate",
      G_CALLBACK(menu_saveas),
      NULL
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
      GTK_WIDGET(notebook),
      TRUE,
      TRUE,
      0
    );
    gtk_container_add(
      GTK_CONTAINER(window),
      box
    );
    gtk_widget_show_all(window);

    // TEMPORARY: disable nonfunctional menu items.
    gtk_widget_set_sensitive(
      menuitem_file_print,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_copy,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_cut,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_paste,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_delete,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_selectall,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_find_find,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_find_findnext,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_find_findprevious,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_find_findreplace,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_find_gotoline,
      FALSE
    );
}

int main(int argc, char **argv){
    GtkApplication *app;

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
    int status = g_application_run(
      G_APPLICATION(app),
      argc,
      argv
    );
    g_object_unref(app);

    return status;
}
