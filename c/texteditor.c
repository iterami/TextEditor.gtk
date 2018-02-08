#include <gtk/gtk.h>
#include "texteditor.h"
#include "../../common/gtk/gtk.c"

static void activate(GtkApplication* app, gpointer data){
    common_init_gtk();

    GtkAccelGroup *accelgroup;
    GtkWidget *box;
    GtkWidget *findreplaceall;
    GtkWidget *menubar;
    GtkWidget *menuitem_edit;
    GtkWidget *menuitem_edit_clearundoredo;
    GtkWidget *menuitem_edit_copy;
    GtkWidget *menuitem_edit_cut;
    GtkWidget *menuitem_edit_delete;
    GtkWidget *menuitem_edit_deleteline;
    GtkWidget *menuitem_edit_deletenextword;
    GtkWidget *menuitem_edit_deletepreviousword;
    GtkWidget *menuitem_edit_paste;
    GtkWidget *menuitem_edit_redo;
    GtkWidget *menuitem_edit_selectall;
    GtkWidget *menuitem_edit_sort;
    GtkWidget *menuitem_edit_undo;
    GtkWidget *menuitem_file;
    GtkWidget *menuitem_file_closetab;
    GtkWidget *menuitem_file_hide;
    GtkWidget *menuitem_file_newtab;
    GtkWidget *menuitem_file_open;
    GtkWidget *menuitem_file_quit;
    GtkWidget *menuitem_file_save;
    GtkWidget *menuitem_file_saveas;
    GtkWidget *menuitem_find;
    GtkWidget *menuitem_find_find;
    GtkWidget *menuitem_find_findnext;
    GtkWidget *menuitem_find_findprevious;
    GtkWidget *menuitem_find_gotobottom;
    GtkWidget *menuitem_find_gotoline;
    GtkWidget *menuitem_find_gototop;
    GtkWidget *menuitem_find_replace;
    GtkWidget *menumenu_edit;
    GtkWidget *menumenu_file;
    GtkWidget *menumenu_find;
    GtkWidget *outerbox;

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
      "TextEditor.gtk"
    );

    // Setup notebook and tab with scrollable text view.
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_scrollable(
      notebook,
      TRUE
    );
    new_tab();

    // Setup menu items.
    menubar = gtk_menu_bar_new();
    accelgroup = gtk_accel_group_new();
    gtk_window_add_accel_group(
      GTK_WINDOW(window),
      accelgroup
    );
    // File menu.
    menumenu_file = gtk_menu_new();
    menuitem_file = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menumenu_file
    );
    menuitem_file_newtab = common_add_menuitem(
      menumenu_file,
      "_New Tab",
      accelgroup,
      GDK_KEY_t,
      GDK_CONTROL_MASK
    );
    menuitem_file_closetab = common_add_menuitem(
      menumenu_file,
      "_Close Tab",
      accelgroup,
      GDK_KEY_w,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    menuitem_file_open = common_add_menuitem(
      menumenu_file,
      "_Open...",
      accelgroup,
      GDK_KEY_O,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    menuitem_file_save = common_add_menuitem(
      menumenu_file,
      "_Save",
      accelgroup,
      GDK_KEY_s,
      GDK_CONTROL_MASK
    );
    menuitem_file_saveas = common_add_menuitem(
      menumenu_file,
      "Save _As...",
      accelgroup,
      GDK_KEY_s,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    menuitem_file_hide = common_add_menuitem(
      menumenu_file,
      "_Hide Windows",
      accelgroup,
      GDK_KEY_Escape,
      0
    );
    menuitem_file_quit = common_add_menuitem(
      menumenu_file,
      "_Quit",
      accelgroup,
      KEY_QUIT,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_file
    );
    // Edit menu.
    menumenu_edit = gtk_menu_new();
    menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_edit),
      menumenu_edit
    );
    menuitem_edit_undo = common_add_menuitem(
      menumenu_edit,
      "_Undo",
      accelgroup,
      GDK_KEY_z,
      GDK_CONTROL_MASK
    );
    menuitem_edit_redo = common_add_menuitem(
      menumenu_edit,
      "_Redo",
      accelgroup,
      GDK_KEY_z,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK
    );
    menuitem_edit_clearundoredo = common_add_menuitem(
      menumenu_edit,
      "Cl_ear Undo/Redo",
      accelgroup,
      GDK_KEY_y,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    menuitem_edit_copy = common_add_menuitem(
      menumenu_edit,
      "_Copy",
      accelgroup,
      GDK_KEY_c,
      GDK_CONTROL_MASK
    );
    menuitem_edit_cut = common_add_menuitem(
      menumenu_edit,
      "Cu_t",
      accelgroup,
      GDK_KEY_x,
      GDK_CONTROL_MASK
    );
    menuitem_edit_paste = common_add_menuitem(
      menumenu_edit,
      "_Paste",
      accelgroup,
      GDK_KEY_v,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    menuitem_edit_delete = common_add_menuitem(
      menumenu_edit,
      "_Delete",
      accelgroup,
      GDK_KEY_Delete,
      0
    );
    menuitem_edit_deleteline = common_add_menuitem(
      menumenu_edit,
      "Delete _Line",
      accelgroup,
      GDK_KEY_d,
      GDK_CONTROL_MASK
    );
    menuitem_edit_deletenextword = common_add_menuitem(
      menumenu_edit,
      "Delete Ne_xt Word",
      accelgroup,
      GDK_KEY_Delete,
      GDK_CONTROL_MASK
    );
    menuitem_edit_deletepreviousword = common_add_menuitem(
      menumenu_edit,
      "Delete Pre_vious Word",
      accelgroup,
      GDK_KEY_BackSpace,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    menuitem_edit_selectall = common_add_menuitem(
      menumenu_edit,
      "Select _All",
      accelgroup,
      GDK_KEY_a,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    menuitem_edit_sort = common_add_menuitem(
      menumenu_edit,
      "_Sort Lines",
      accelgroup,
      GDK_KEY_r,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_edit
    );
    // Find menu.
    menumenu_find = gtk_menu_new();
    menuitem_find = gtk_menu_item_new_with_mnemonic("F_ind");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_find),
      menumenu_find
    );
    menuitem_find_findnext = common_add_menuitem(
      menumenu_find,
      "Find _Next",
      accelgroup,
      GDK_KEY_g,
      GDK_CONTROL_MASK
    );
    menuitem_find_findprevious = common_add_menuitem(
      menumenu_find,
      "Find _Previous",
      accelgroup,
      GDK_KEY_g,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK
    );
    menuitem_find_find = common_add_menuitem(
      menumenu_find,
      "_Find/Replace...",
      accelgroup,
      GDK_KEY_f,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      gtk_separator_menu_item_new()
    );
    menuitem_find_replace = common_add_menuitem(
      menumenu_find,
      "_Replace All",
      accelgroup,
      GDK_KEY_h,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      gtk_separator_menu_item_new()
    );
    menuitem_find_gototop = common_add_menuitem(
      menumenu_find,
      "Go to _Top",
      accelgroup,
      GDK_KEY_Home,
      0
    );
    menuitem_find_gotobottom = common_add_menuitem(
      menumenu_find,
      "Go to _Bottom",
      accelgroup,
      GDK_KEY_End,
      0
    );
    menuitem_find_gotoline = common_add_menuitem(
      menumenu_find,
      "Go to _Line",
      accelgroup,
      GDK_KEY_l,
      GDK_CONTROL_MASK
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_find
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

    // Setup find window.
    find_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_add_accel_group(
      GTK_WINDOW(find_window),
      accelgroup
    );
    gtk_window_set_default_size(
      GTK_WINDOW(find_window),
      300,
      200
    );
    gtk_window_set_title(
      GTK_WINDOW(find_window),
      "Find/Replace..."
    );
    gtk_window_set_attached_to(
      GTK_WINDOW(find_window),
      window
    );
    gtk_window_set_transient_for(
      GTK_WINDOW(find_window),
      GTK_WINDOW(window)
    );
    gtk_window_set_type_hint(
      GTK_WINDOW(find_window),
      GDK_WINDOW_TYPE_HINT_DIALOG
    );
    outerbox = gtk_box_new(
      GTK_ORIENTATION_VERTICAL,
      0
    );
    find_window_find = new_textview(FALSE);
    gtk_box_pack_start(
      GTK_BOX(outerbox),
      find_window_find,
      TRUE,
      TRUE,
      1
    );
    find_window_replace = new_textview(FALSE);
    gtk_box_pack_start(
      GTK_BOX(outerbox),
      find_window_replace,
      TRUE,
      TRUE,
      0
    );
    gtk_container_add(
      GTK_CONTAINER(find_window),
      outerbox
    );
    g_signal_connect_swapped(
      find_window,
      "delete-event",
      G_CALLBACK(gtk_widget_hide_on_delete),
      find_window
    );
    g_signal_connect_swapped(
      find_window,
      "hide",
      G_CALLBACK(find_clear_tags),
      NULL
    );

    // Setup line window.
    line_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_add_accel_group(
      GTK_WINDOW(line_window),
      accelgroup
    );
    gtk_window_set_default_size(
      GTK_WINDOW(line_window),
      300,
      0
    );
    gtk_window_set_title(
      GTK_WINDOW(line_window),
      "Go to Line..."
    );
    gtk_window_set_attached_to(
      GTK_WINDOW(line_window),
      window
    );
    gtk_window_set_transient_for(
      GTK_WINDOW(line_window),
      GTK_WINDOW(window)
    );
    gtk_window_set_type_hint(
      GTK_WINDOW(line_window),
      GDK_WINDOW_TYPE_HINT_DIALOG
    );
    outerbox = gtk_box_new(
      GTK_ORIENTATION_VERTICAL,
      0
    );
    line_window_line = gtk_entry_new();
    gtk_container_add(
      GTK_CONTAINER(line_window),
      line_window_line
    );
    g_signal_connect_swapped(
      line_window_line,
      "activate",
      G_CALLBACK(go_to_line),
      NULL
    );
    g_signal_connect_swapped(
      line_window,
      "delete-event",
      G_CALLBACK(gtk_widget_hide_on_delete),
      line_window
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
      menuitem_file_hide,
      "activate",
      G_CALLBACK(menu_hide),
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
    g_signal_connect_swapped(
      menuitem_edit_redo,
      "activate",
      G_CALLBACK(menu_redo),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_edit_undo,
      "activate",
      G_CALLBACK(menu_undo),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_edit_clearundoredo,
      "activate",
      G_CALLBACK(menu_clearundoredo),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_edit_deleteline,
      "activate",
      G_CALLBACK(menu_deleteline),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_find,
      "activate",
      G_CALLBACK(menu_find),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_findnext,
      "activate",
      G_CALLBACK(menu_findnext),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_findprevious,
      "activate",
      G_CALLBACK(menu_findprevious),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_replace,
      "activate",
      G_CALLBACK(menu_findreplaceall),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_gotobottom,
      "activate",
      G_CALLBACK(menu_findbottom),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_gototop,
      "activate",
      G_CALLBACK(menu_findtop),
      NULL
    );
    g_signal_connect_swapped(
      menuitem_find_gotoline,
      "activate",
      G_CALLBACK(menu_findline),
      NULL
    );

    // Disable nonfunctional menu items.
    gtk_widget_set_sensitive(
      menuitem_edit_cut,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_copy,
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
      menuitem_edit_deletenextword,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_deletepreviousword,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_selectall,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_sort,
      FALSE
    );

    // Open previously opened files.
    gchar *temp_path = common_iterami_path("cfg/texteditor.cfg");
    gchar *temp_content;
    gssize temp_length;

    if(g_file_get_contents(
      temp_path,
      &temp_content,
      &temp_length,
      NULL
    )){
        GtkTextIter temp_end;
        GtkTextIter temp_start;
        GtkTextBuffer *temp_buffer;
        GtkWidget *temp_text_view;

        temp_buffer = gtk_text_buffer_new(NULL);
        temp_text_view = gtk_text_view_new_with_buffer(temp_buffer);
        gtk_text_buffer_set_text(
          temp_buffer,
          temp_content,
          temp_length
        );

        gint lines = gtk_text_buffer_get_line_count(temp_buffer);
        gint line = 0;
        while(line < lines){
            char *filename;

            gtk_text_buffer_get_iter_at_line(
              temp_buffer,
              &temp_start,
              line
            );
            gtk_text_buffer_get_iter_at_line(
              temp_buffer,
              &temp_end,
              line + 1
            );
            if(gtk_text_iter_backward_char(&temp_end)){
                filename = gtk_text_buffer_get_text(
                  temp_buffer,
                  &temp_start,
                  &temp_end,
                  FALSE
                );

                open_file(filename);
            }

            g_free(filename);
            line++;
        }

        gtk_widget_destroy(temp_text_view);
    }

    g_free(temp_path);
    g_free(temp_content);
}

static void block_insertdelete_signals(GtkTextBuffer *text_buffer){
    g_signal_handlers_block_by_func(
      text_buffer,
      G_CALLBACK(text_deleted),
      NULL
    );
    g_signal_handlers_block_by_func(
      text_buffer,
      G_CALLBACK(text_inserted),
      NULL
    );
}

static gboolean check_equals_unsaved(void){
    return g_strcmp0(
      get_current_tab_label_text(),
      "UNSAVED"
    ) == 0;
}

static void close_tab(void){
    if(get_notebook_no_pages()){
        return;
    }

    gtk_notebook_remove_page(
      notebook,
      gtk_notebook_get_current_page(notebook)
    );

    update_opened_files();
}

static void find_clear_tags(void){
    gint page;
    GtkTextIter end;
    GtkTextIter start;

    page = gtk_notebook_get_n_pages(notebook) - 1;
    while(page >= 0){
        tabcontents tab;
        tab = get_tab_contents(page);
        gtk_text_buffer_get_bounds(
          tab.text_buffer,
          &start,
          &end
        );

        gtk_text_buffer_remove_all_tags(
          tab.text_buffer,
          &start,
          &end
        );
        page--;
    }
}

static const gchar* get_current_tab_label_text(void){
    return gtk_label_get_text(GTK_LABEL(gtk_notebook_get_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    )));
}

static gchar* get_find_find(void){
    GtkTextBuffer *buffer;
    GtkTextIter findend;
    GtkTextIter findstart;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_window_find));
    gtk_text_buffer_get_bounds(
      buffer,
      &findstart,
      &findend
    );

    return gtk_text_buffer_get_text(
      buffer,
      &findstart,
      &findend,
      FALSE
    );
}

static gboolean get_notebook_no_pages(void){
    return gtk_notebook_get_n_pages(notebook) <= 0;
}

static GList* get_tabbox_children(GtkNotebook *tabnotebook, gint page){
    return gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(
      tabnotebook,
      page
    )));
}

static struct tabcontents get_tab_contents(gint page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *tabnotebook;
    GtkWidget *tabmappane;
    GtkWidget *tabundopane;
    GtkWidget *text_view;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    text_view = gtk_bin_get_child(GTK_BIN(g_list_nth_data(
      children,
      0
    )));
    tabnotebook = g_list_nth_data(
      children,
      1
    );
    tabmappane = gtk_notebook_get_nth_page(
      tabnotebook,
      0
    );
    tabundopane = gtk_notebook_get_nth_page(
      tabnotebook,
      1
    );

    tabcontents result = {
      page,
      text_view,
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)),
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_paned_get_child1(GTK_PANED(tabundopane)))))),
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_paned_get_child2(GTK_PANED(tabundopane)))))),
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(tabmappane))))
    };
    return result;
}

static void go_to_line(void){
    if(get_notebook_no_pages()
      || gtk_entry_get_text_length(GTK_ENTRY(line_window_line)) <= 0){
        return;
    }

    const gchar *entry;
    gint linenumber = 0;
    GtkTextIter line;
    tabcontents tab;

    entry = gtk_entry_get_text(GTK_ENTRY(line_window_line));

    int loopi = 0;
    int length_line = 0;
    while(entry[loopi] != '\0'){
        length_line++;
        loopi++;
    }
    loopi = 0;
    while(loopi < length_line){
        linenumber *= 10;
        linenumber += entry[loopi] - '0';
        loopi++;
    }

    tab = get_tab_contents(-1);

    gtk_text_buffer_get_iter_at_line(
      tab.text_buffer,
      &line,
      linenumber
    );

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &line
    );
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv){
    GtkApplication *app;

    app = gtk_application_new(
      "com.iterami.texteditorgtk",
      0
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

static void menu_clearundoredo(void){
    GtkTextIter end;
    GtkTextIter start;
    tabcontents tab;

    tab = get_tab_contents(-1);

    gtk_text_buffer_set_text(
      tab.undo_buffer,
      "",
      0
    );
    gtk_text_buffer_set_text(
      tab.redo_buffer,
      "",
      0
    );
}

static void menu_deleteline(void){
    if(get_notebook_no_pages()){
        return;
    }

    gint endlinenumber;
    gint linenumber;
    GtkTextIter end;
    GtkTextIter line;
    GtkTextIter start;
    tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_iter_at_mark(
      tab.text_buffer,
      &line,
      gtk_text_buffer_get_insert(tab.text_buffer)
    );
    linenumber = gtk_text_iter_get_line(&line);
    gtk_text_buffer_get_iter_at_line(
      tab.text_buffer,
      &end,
      linenumber + 1
    );
    endlinenumber = gtk_text_iter_get_line(&end);

    // Deleting first line.
    if(linenumber == 0){
        gtk_text_buffer_get_start_iter(
          tab.text_buffer,
          &start
        );
        if(endlinenumber == 0){
            gtk_text_buffer_get_end_iter(
              tab.text_buffer,
              &end
            );

        }else{
            gtk_text_buffer_get_iter_at_line(
              tab.text_buffer,
              &end,
              1
            );
        }

    // Deleting last line.
    }else if(linenumber == endlinenumber){
        gtk_text_buffer_get_iter_at_line(
          tab.text_buffer,
          &start,
          linenumber
        );
        gtk_text_iter_backward_char(&start);
        gtk_text_buffer_get_end_iter(
          tab.text_buffer,
          &end
        );

    // Deleting any other line.
    }else{
        gtk_text_buffer_get_iter_at_line(
          tab.text_buffer,
          &start,
          linenumber
        );
        gtk_text_buffer_get_iter_at_line(
          tab.text_buffer,
          &end,
          linenumber + 1
        );
    }

    gtk_text_buffer_delete(
      tab.text_buffer,
      &start,
      &end
    );
}

static void menu_find(void){
    gtk_widget_show_all(find_window);
    gtk_window_present(GTK_WINDOW(find_window));

    GtkTextBuffer *buffer;
    tabcontents tab;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_window_find));
    tab = get_tab_contents(-1);

    if(gtk_text_buffer_get_has_selection(tab.text_buffer)){
        GtkTextIter end;
        GtkTextIter start;
        gtk_text_buffer_get_selection_bounds(
          tab.text_buffer,
          &start,
          &end
        );

        finding = gtk_text_buffer_get_text(
          tab.text_buffer,
          &start,
          &end,
          FALSE
        );
        gtk_text_buffer_set_text(
          buffer,
          finding,
          -1
        );
    }

    gtk_widget_grab_focus(find_window_find);

    GtkTextIter findend;
    GtkTextIter findstart;

    gtk_text_buffer_get_bounds(
      buffer,
      &findstart,
      &findend
    );

    gtk_text_buffer_select_range(
      buffer,
      &findstart,
      &findend
    );

    menu_refind();
}

static void menu_findbottom(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextIter end;
    tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_end_iter(
      tab.text_buffer,
      &end
    );

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &end
    );
}

static void menu_findline(void){
    gtk_widget_show_all(line_window);
    gtk_window_present(GTK_WINDOW(line_window));

    gtk_widget_grab_focus(line_window_line);
}

static void menu_findnext(void){
    if(get_notebook_no_pages()){
        return;
    }

    if(!finding
      || finding != get_find_find()){
        menu_refind();
    }

    GtkTextIter cursor;
    GtkTextTag *tag_found;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_iter_at_mark(
      tab.text_buffer,
      &cursor,
      gtk_text_buffer_get_insert(tab.text_buffer)
    );
    tag_found = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(tab.text_buffer),
      "found"
    );
    if(gtk_text_iter_forward_to_tag_toggle(
      &cursor,
      tag_found
    )){
        GtkTextIter end;
        GtkTextIter start;

        if(gtk_text_iter_begins_tag(
          &cursor,
          tag_found
        )){
            start = cursor;
            gtk_text_iter_forward_to_tag_toggle(
              &cursor,
              tag_found
            );
            end = cursor;

        }else{
            end = cursor;
            gtk_text_iter_backward_to_tag_toggle(
              &cursor,
              tag_found
            );
            start = cursor;
        }

        place_cursor(
          tab.text_view,
          tab.text_buffer,
          &cursor
        );
        gtk_text_buffer_select_range(
          tab.text_buffer,
          &end,
          &start
        );
    }
}

static void menu_findprevious(void){
    if(get_notebook_no_pages()){
        return;
    }

    if(!finding
      || finding != get_find_find()){
        menu_refind();
    }

    GtkTextIter cursor;
    GtkTextTag *tag_found;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_iter_at_mark(
      tab.text_buffer,
      &cursor,
      gtk_text_buffer_get_insert(tab.text_buffer)
    );
    tag_found = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(tab.text_buffer),
      "found"
    );
    if(gtk_text_iter_backward_to_tag_toggle(
      &cursor,
      tag_found
    )){
        GtkTextIter end;
        GtkTextIter start;

        if(gtk_text_iter_begins_tag(
          &cursor,
          tag_found
        )){
            start = cursor;
            gtk_text_iter_forward_to_tag_toggle(
              &cursor,
              tag_found
            );
            end = cursor;

        }else{
            end = cursor;
            gtk_text_iter_backward_to_tag_toggle(
              &cursor,
              tag_found
            );
            start = cursor;
        }

        place_cursor(
          tab.text_view,
          tab.text_buffer,
          &cursor
        );
        gtk_text_buffer_select_range(
          tab.text_buffer,
          &start,
          &end
        );
    }
}

static void menu_find_recursive(GtkTextBuffer *buffer, GtkTextIter start){
    GtkTextIter match_end;
    GtkTextIter match_start;
    if(gtk_text_iter_forward_search(
      &start,
      finding,
      GTK_TEXT_SEARCH_CASE_INSENSITIVE,
      &match_start,
      &match_end,
      NULL
    )){
        gtk_text_buffer_apply_tag_by_name(
          buffer,
          "found",
          &match_start,
          &match_end
        );

        menu_find_recursive(
          buffer,
          match_end
        );
    }
}

static void menu_findreplaceall(void){
    if(get_notebook_no_pages()
      || !gtk_widget_is_visible(find_window)){
        return;
    }

    gchar *replace_content;
    GtkTextBuffer *replace_buffer;
    GtkTextIter end;
    GtkTextIter replace_end;
    GtkTextIter replace_start;
    GtkTextIter start;
    GtkTextTag *tag_found;
    tabcontents tab;

    tab = get_tab_contents(-1);

    replace_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_window_replace));
    gtk_text_buffer_get_bounds(
      replace_buffer,
      &replace_start,
      &replace_end
    );
    replace_content = gtk_text_buffer_get_text(
      replace_buffer,
      &replace_start,
      &replace_end,
      FALSE
    );

    tag_found = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(tab.text_buffer),
      "found"
    );
    while(!gtk_text_iter_is_end(&end)){
        menu_refind();

        gtk_text_buffer_get_start_iter(
          tab.text_buffer,
          &end
        );
        gtk_text_iter_forward_to_tag_toggle(
          &end,
          tag_found
        );

        if(gtk_text_iter_begins_tag(
          &end,
          tag_found
        )){
            start = end;
            int offset = gtk_text_iter_get_offset(&start);

            gtk_text_iter_forward_to_tag_toggle(
              &end,
              tag_found
            );

            gtk_text_buffer_delete(
              tab.text_buffer,
              &start,
              &end
            );

            gtk_text_buffer_get_iter_at_offset(
              tab.text_buffer,
              &start,
              offset
            );
            gtk_text_buffer_insert(
              tab.text_buffer,
              &start,
              replace_content,
              -1
            );
        }
    }
}

static void menu_findtop(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextIter start;
    tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_start_iter(
      tab.text_buffer,
      &start
    );

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &start
    );
}

static void menu_hide(void){
    gtk_widget_hide(find_window);
    gtk_widget_hide(line_window);
}

static void menu_open(void){
    GtkFileChooser *chooser;
    GtkWidget *dialog_open;

    dialog_open = gtk_file_chooser_dialog_new(
      "Open File",
      GTK_WINDOW(window),
      GTK_FILE_CHOOSER_ACTION_OPEN,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Open",
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    chooser = GTK_FILE_CHOOSER(dialog_open);
    if(!check_equals_unsaved()){
        gtk_file_chooser_set_file(
          chooser,
          g_file_new_for_path(get_current_tab_label_text()),
          NULL
        );
    }
    gtk_file_chooser_set_show_hidden(
      chooser,
      TRUE
    );

    if(gtk_dialog_run(GTK_DIALOG(dialog_open)) == GTK_RESPONSE_ACCEPT){
        char *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        open_file(filename);

        g_free(filename);
    }

    gtk_widget_destroy(dialog_open);
}

static void menu_redo(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextIter mapstart;
    GtkTextIter redoend;
    GtkTextIter redostart;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_start_iter(
      tab.redo_buffer,
      &redostart
    );
    gtk_text_buffer_get_iter_at_line(
      tab.redo_buffer,
      &redoend,
      1
    );
    if(gtk_text_iter_compare(
      &redostart,
      &redoend
    ) == 0){
        return;
    }

    gchar *entry;
    GtkTextIter selectstart;
    GtkTextIter undostart;

    gtk_text_buffer_get_start_iter(
      tab.undo_buffer,
      &undostart
    );

    entry = gtk_text_buffer_get_text(
      tab.redo_buffer,
      &redostart,
      &redoend,
      FALSE
    );

    gtk_text_buffer_insert(
      tab.undo_buffer,
      &undostart,
      entry,
      -1
    );
    gtk_text_buffer_delete(
      tab.redo_buffer,
      &redostart,
      &redoend
    );

    // Parse entry.
    gboolean inserted = TRUE;
    if(entry[0] == '0'){
        inserted = FALSE;
    }
    int loopi = 2;
    int length_line = 0;
    while(entry[loopi] != ','){
        length_line++;
        loopi++;
    }
    gint line = 0;
    loopi = 0;
    while(loopi < length_line){
        line *= 10;
        line += entry[loopi + 2] - '0';
        loopi++;
    }

    loopi = length_line + 3;
    int length_lineoffset = 0;
    while(entry[loopi] != ','){
        length_lineoffset++;
        loopi++;
    }
    gint lineoffset = 0;
    loopi = 0;
    while(loopi < length_lineoffset){
        lineoffset *= 10;
        lineoffset += entry[loopi + length_line + 3] - '0';
        loopi++;
    }

    loopi = length_line + length_lineoffset + 4;
    int length_value = 0;
    while(entry[loopi] != '\n'){
        length_value++;
        loopi++;
    }
    gchar value[length_value + 1];
    loopi = 0;
    while(loopi < length_value){
        value[loopi] = entry[loopi + length_line + length_lineoffset + 4];
        if(value[loopi] == 30){
            value[loopi] = '\n';
        }
        loopi++;
    }
    value[length_value] = '\0';

    // Repeat entry.
    block_insertdelete_signals(tab.text_buffer);
    gtk_text_buffer_get_iter_at_line_offset(
      tab.text_buffer,
      &selectstart,
      line,
      lineoffset
    );
    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &selectstart
    );

    gtk_text_buffer_get_iter_at_offset(
      tab.map_buffer,
      &mapstart,
      gtk_text_iter_get_offset(&selectstart)
    );

    if(inserted){
        gtk_text_buffer_insert(
          tab.text_buffer,
          &selectstart,
          value,
          -1
        );
        gtk_text_buffer_insert(
          tab.map_buffer,
          &mapstart,
          value,
          -1
        );

    }else{
        GtkTextIter mapend;
        GtkTextIter selectend;

        selectend = selectstart;
        gtk_text_iter_forward_chars(
           &selectend,
           length_value
        );
        gtk_text_buffer_get_iter_at_offset(
          tab.map_buffer,
          &mapend,
          gtk_text_iter_get_offset(&selectend)
        );

        gtk_text_buffer_delete(
          tab.text_buffer,
          &selectstart,
          &selectend
        );
        gtk_text_buffer_delete(
          tab.map_buffer,
          &mapstart,
          &mapend
        );
    }
    unblock_insertdelete_signals(tab.text_buffer);

    menu_refind();
}

static void menu_refind(void){
    finding = get_find_find();
    if(get_notebook_no_pages()
      || !gtk_widget_is_visible(find_window)){
        return;
    }
    if(finding
      && finding[0] == '\0'){
        return;
    }

    GtkTextIter tabstart;
    tabcontents tab;

    find_clear_tags();
    tab = get_tab_contents(-1);
    gtk_text_buffer_get_start_iter(
      tab.text_buffer,
      &tabstart
    );

    menu_find_recursive(
      tab.text_buffer,
      tabstart
    );
}

static void menu_save(void){
    if(get_notebook_no_pages()){
        return;
    }

    if(check_equals_unsaved()){
        menu_saveas();

    }else{
        save_tab(NULL);
    }
}

static void menu_saveas(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkFileChooser *chooser;
    GtkWidget *dialog_saveas;

    dialog_saveas = gtk_file_chooser_dialog_new(
      "Save File",
      GTK_WINDOW(window),
      GTK_FILE_CHOOSER_ACTION_SAVE,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Save",
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    chooser = GTK_FILE_CHOOSER(dialog_saveas);
    if(!check_equals_unsaved()){
        gtk_file_chooser_set_file(
          chooser,
          g_file_new_for_path(get_current_tab_label_text()),
          NULL
        );
    }
    gtk_file_chooser_set_do_overwrite_confirmation(
      chooser,
      TRUE
    );
    gtk_file_chooser_set_show_hidden(
      chooser,
      TRUE
    );

    if(gtk_dialog_run(GTK_DIALOG(dialog_saveas)) == GTK_RESPONSE_ACCEPT){
        save_tab(gtk_file_chooser_get_filename(chooser));
    }

    gtk_widget_destroy(dialog_saveas);

    update_opened_files();
}

static void menu_undo(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextIter mapstart;
    GtkTextIter undoend;
    GtkTextIter undostart;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_start_iter(
      tab.undo_buffer,
      &undostart
    );
    gtk_text_buffer_get_iter_at_line(
      tab.undo_buffer,
      &undoend,
      1
    );
    if(gtk_text_iter_compare(
      &undostart,
      &undoend
    ) == 0){
        return;
    }

    gchar *entry;
    GtkTextIter redostart;
    GtkTextIter selectstart;

    gtk_text_buffer_get_start_iter(
      tab.redo_buffer,
      &redostart
    );

    entry = gtk_text_buffer_get_text(
      tab.undo_buffer,
      &undostart,
      &undoend,
      FALSE
    );

    gtk_text_buffer_insert(
      tab.redo_buffer,
      &redostart,
      entry,
      -1
    );
    gtk_text_buffer_delete(
      tab.undo_buffer,
      &undostart,
      &undoend
    );

    // Parse entry.
    gboolean inserted = TRUE;
    if(entry[0] == '0'){
        inserted = FALSE;
    }
    int loopi = 2;
    int length_line = 0;
    while(entry[loopi] != ','){
        length_line++;
        loopi++;
    }
    gint line = 0;
    loopi = 0;
    while(loopi < length_line){
        line *= 10;
        line += entry[loopi + 2] - '0';
        loopi++;
    }

    loopi = length_line + 3;
    int length_lineoffset = 0;
    while(entry[loopi] != ','){
        length_lineoffset++;
        loopi++;
    }
    gint lineoffset = 0;
    loopi = 0;
    while(loopi < length_lineoffset){
        lineoffset *= 10;
        lineoffset += entry[loopi + length_line + 3] - '0';
        loopi++;
    }

    loopi = length_line + length_lineoffset + 4;
    int length_value = 0;
    while(entry[loopi] != '\n'){
        length_value++;
        loopi++;
    }
    gchar value[length_value + 1];
    loopi = 0;
    while(loopi < length_value){
        value[loopi] = entry[loopi + length_line + length_lineoffset + 4];
        if(value[loopi] == 30){
            value[loopi] = '\n';
        }
        loopi++;
    }
    value[length_value] = '\0';

    // Repeat entry.
    block_insertdelete_signals(tab.text_buffer);
    gtk_text_buffer_get_iter_at_line_offset(
      tab.text_buffer,
      &selectstart,
      line,
      lineoffset
    );
    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &selectstart
    );

    gtk_text_buffer_get_iter_at_offset(
      tab.map_buffer,
      &mapstart,
      gtk_text_iter_get_offset(&selectstart)
    );

    if(inserted){
        GtkTextIter mapend;
        GtkTextIter selectend;

        selectend = selectstart;
        gtk_text_iter_forward_chars(
           &selectend,
           length_value
        );
        gtk_text_buffer_get_iter_at_offset(
          tab.map_buffer,
          &mapend,
          gtk_text_iter_get_offset(&selectend)
        );

        gtk_text_buffer_delete(
          tab.text_buffer,
          &selectstart,
          &selectend
        );
        gtk_text_buffer_delete(
          tab.map_buffer,
          &mapstart,
          &mapend
        );

    }else{
        gtk_text_buffer_insert(
          tab.text_buffer,
          &selectstart,
          value,
          -1
        );
        gtk_text_buffer_insert(
          tab.map_buffer,
          &mapstart,
          value,
          -1
        );
    }
    unblock_insertdelete_signals(tab.text_buffer);

    menu_refind();
}

static GtkWidget* new_scrolled_window(void){
    GtkWidget *scrolled_window;

    scrolled_window = gtk_scrolled_window_new(
      NULL,
      NULL
    );
    gtk_scrolled_window_set_policy(
      GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC
    );

    return scrolled_window;
}

static void new_tab(void){
    GtkNotebook *tabnotebook;
    GtkWidget *scrolled_window_map;
    GtkWidget *scrolled_window_redo;
    GtkWidget *scrolled_window_undo;
    GtkWidget *scrolled_window;
    GtkWidget *tabbox;
    GtkWidget *text_view;
    GtkWidget *undopaned;

    tabbox = gtk_box_new(
      GTK_ORIENTATION_HORIZONTAL,
      1
    );
    text_view = new_textview(FALSE);
    scrolled_window = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window),
      text_view
    );
    gtk_box_pack_start(
      GTK_BOX(tabbox),
      scrolled_window,
      TRUE,
      TRUE,
      0
    );
    tabnotebook = GTK_NOTEBOOK(gtk_notebook_new());
    scrolled_window_map = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_map),
      new_textview(TRUE)
    );
    gtk_notebook_append_page(
      tabnotebook,
      scrolled_window_map,
      gtk_label_new("Map")
    );
    undopaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    scrolled_window_undo = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_undo),
      new_textview(FALSE)
    );
    gtk_paned_pack1(
      GTK_PANED(undopaned),
      scrolled_window_undo,
      TRUE,
      TRUE
    );
    scrolled_window_redo = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_redo),
      new_textview(FALSE)
    );
    gtk_paned_pack2(
      GTK_PANED(undopaned),
      scrolled_window_redo,
      TRUE,
      TRUE
    );
    gtk_notebook_append_page(
      tabnotebook,
      undopaned,
      gtk_label_new("Undo/Redo")
    );
    gtk_box_pack_start(
      GTK_BOX(tabbox),
      GTK_WIDGET(tabnotebook),
      FALSE,
      FALSE,
      0
    );
    gtk_notebook_append_page(
      notebook,
      tabbox,
      gtk_label_new("UNSAVED")
    );
    gtk_widget_show_all(window);

    gtk_notebook_set_current_page(
      notebook,
      gtk_notebook_get_n_pages(notebook) - 1
    );
    gtk_widget_grab_focus(text_view);

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect_swapped(
      buffer,
      "insert-text",
      G_CALLBACK(text_inserted),
      NULL
    );
    g_signal_connect_swapped(
      buffer,
      "delete-range",
      G_CALLBACK(text_deleted),
      NULL
    );
}

static GtkWidget* new_textview(gboolean map){
    GtkTextBuffer *buffer;
    GtkWidget *text_view;

    buffer = gtk_text_buffer_new(NULL);
    text_view = gtk_text_view_new_with_buffer(buffer);
    gtk_text_view_set_wrap_mode(
      GTK_TEXT_VIEW(text_view),
      GTK_WRAP_WORD
    );
    gtk_text_buffer_create_tag(
      buffer,
      "found",
      "background",
      "#650",
      NULL
    );

    if(map){
        gtk_text_view_set_editable(
          GTK_TEXT_VIEW(text_view),
          FALSE
        );
        gtk_widget_set_name(
          text_view,
          "map"
        );
    }

    return text_view;
}

static void open_file(char *filename){
    gchar *content;
    gssize length;

    if(g_file_get_contents(
      filename,
      &content,
      &length,
      NULL
    ) && g_utf8_validate(
      content,
      length,
      NULL
    )){
        if(get_notebook_no_pages()
          || !check_equals_unsaved()){
            new_tab();
        }

        tabcontents tab;
        tab = get_tab_contents(-1);

        block_insertdelete_signals(tab.text_buffer);
        gtk_text_buffer_set_text(
          tab.text_buffer,
          content,
          length
        );
        gtk_text_buffer_set_text(
          tab.undo_buffer,
          "",
          0
        );
        gtk_text_buffer_set_text(
          tab.redo_buffer,
          "",
          0
        );
        gtk_notebook_set_tab_label(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            tab.page
          ),
          gtk_label_new(filename)
        );
        unblock_insertdelete_signals(tab.text_buffer);

        GtkTextIter start;
        gtk_text_buffer_get_start_iter(
          tab.text_buffer,
          &start
        );
        place_cursor(
          tab.text_view,
          tab.text_buffer,
          &start
        );

        gtk_text_buffer_set_text(
          tab.map_buffer,
          content,
          -1
        );
    }

    g_free(content);

    update_opened_files();
}

static void place_cursor(GtkWidget *text_view, GtkTextBuffer *text_buffer, GtkTextIter *iter){
    gtk_text_buffer_place_cursor(
      text_buffer,
      iter
    );
    gtk_text_view_scroll_to_iter(
      GTK_TEXT_VIEW(text_view),
      iter,
      0,
      TRUE,
      0,
      0.5
    );
}

static void save_tab(const char *filename){
    if(!filename){
        filename = get_current_tab_label_text();
    }

    gchar *content;
    GtkTextIter checkend;
    GtkTextIter checkstart;
    GtkTextIter end;
    GtkTextIter start;
    tabcontents tab;
    tab = get_tab_contents(-1);

    // Trim line endings.
    gint linei = gtk_text_buffer_get_line_count(tab.text_buffer);
    while(linei >= 0){
        linei--;
        gtk_text_buffer_get_iter_at_line(
          tab.text_buffer,
          &checkend,
          linei
        );
        gtk_text_iter_forward_to_line_end(&checkend);
        checkstart = checkend;
        if(gtk_text_iter_backward_char(&checkstart)){
            gchar *checked;
            checked = gtk_text_buffer_get_text(
              tab.text_buffer,
              &checkstart,
              &checkend,
              FALSE
            );
            if(checked[0] == ' '){
                gboolean forward = TRUE;
                while(checked[0] == ' '){
                    if(gtk_text_iter_backward_char(&checkstart)){
                        checked = gtk_text_buffer_get_text(
                          tab.text_buffer,
                          &checkstart,
                          &checkend,
                          FALSE
                        );

                    }else{
                        forward = FALSE;
                        break;
                    }
                }
                if(forward){
                    gtk_text_iter_forward_char(&checkstart);
                }
                gtk_text_buffer_delete(
                  tab.text_buffer,
                  &checkstart,
                  &checkend
                );
            }
            g_free(checked);
        }
    }

    block_insertdelete_signals(tab.text_buffer);

    gtk_text_buffer_get_end_iter(
      tab.text_buffer,
      &end
    );
    // Make sure file ends with newline.
    checkstart = end;
    if(gtk_text_iter_backward_char(&checkstart)){
        gchar *lastchar;
        lastchar = gtk_text_buffer_get_text(
          tab.text_buffer,
          &checkstart,
          &end,
          FALSE
        );
        if(lastchar[0] != '\n'){
            gtk_text_buffer_insert(
              tab.text_buffer,
              &end,
              "\n",
              1
            );

            gtk_text_buffer_get_end_iter(
              tab.text_buffer,
              &end
            );
        }
        g_free(lastchar);
    }
    gtk_text_buffer_get_start_iter(
      tab.text_buffer,
      &start
    );

    content = gtk_text_buffer_get_text(
      tab.text_buffer,
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
    g_free(content);

    gtk_notebook_set_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      ),
      gtk_label_new(filename)
    );

    unblock_insertdelete_signals(tab.text_buffer);
}

static void text_deleted(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end){
    GtkTextIter first;
    GtkTextIter mapend;
    GtkTextIter mapstart;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_start_iter(
      tab.undo_buffer,
      &first
    );

    char *entry = undoredo_entry(
      gtk_text_buffer_get_text(
        tab.text_buffer,
        start,
        end,
        FALSE
      ),
      FALSE,
      gtk_text_iter_get_line(start),
      gtk_text_iter_get_line_offset(start)
    );
    gtk_text_buffer_insert(
      tab.undo_buffer,
      &first,
      entry,
      -1
    );
    g_free(entry);

    gtk_text_buffer_get_iter_at_offset(
      tab.map_buffer,
      &mapstart,
      gtk_text_iter_get_offset(start)
    );
    gtk_text_buffer_get_iter_at_offset(
      tab.map_buffer,
      &mapend,
      gtk_text_iter_get_offset(end)
    );
    gtk_text_buffer_delete(
      tab.map_buffer,
      &mapstart,
      &mapend
    );

    menu_refind();

    gtk_text_buffer_set_text(
      tab.redo_buffer,
      "",
      0
    );
}

static void text_inserted(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *value){
    GtkTextIter first;
    GtkTextIter mapiter;
    tabcontents tab;

    tab = get_tab_contents(-1);
    gtk_text_buffer_get_start_iter(
      tab.undo_buffer,
      &first
    );

    char *entry = undoredo_entry(
      value,
      TRUE,
      gtk_text_iter_get_line(iter),
      gtk_text_iter_get_line_offset(iter)
    );
    gtk_text_buffer_insert(
      tab.undo_buffer,
      &first,
      entry,
      -1
    );
    g_free(entry);

    gtk_text_buffer_get_iter_at_offset(
      tab.map_buffer,
      &mapiter,
      gtk_text_iter_get_offset(iter)
    );
    gtk_text_buffer_insert(
      tab.map_buffer,
      &mapiter,
      value,
      -1
    );

    menu_refind();

    gtk_text_buffer_set_text(
      tab.redo_buffer,
      "",
      0
    );
}

static void unblock_insertdelete_signals(GtkTextBuffer *text_buffer){
    g_signal_handlers_unblock_by_func(
      text_buffer,
      G_CALLBACK(text_deleted),
      NULL
    );
    g_signal_handlers_unblock_by_func(
      text_buffer,
      G_CALLBACK(text_inserted),
      NULL
    );
}

static gchar* undoredo_entry(gchar *value, gboolean inserted, gint line, gint lineoffset){
    gint length_line = common_get_int_length(line);
    gint length_lineoffset = common_get_int_length(lineoffset);
    gint length_value = 0;

    char lineoffsetstring[length_lineoffset];
    char linestring[length_line];

    while(value[length_value] != '\0'){
        length_value++;
    }

    value = g_locale_to_utf8(
      value,
      length_value,
      NULL,
      NULL,
      NULL
    );
    gchar newvalue[length_value];
    gint loopi = 0;
    while(loopi < length_value){
        newvalue[loopi] = value[loopi];
        if(newvalue[loopi] == '\n'){
            newvalue[loopi] = 30;
        }
        loopi++;
    }

    sprintf(
      linestring,
      "%i",
      line
    );
    sprintf(
      lineoffsetstring,
      "%i",
      lineoffset
    );

    gchar *entry = g_malloc(length_value + length_line + length_lineoffset + 6);

    // 1 == Inserted, 0 == Deleted.
    if(inserted){
        entry[0] = '1';
    }else{
        entry[0] = '0';
    }
    entry[1] = ',';

    // Line Number.
    loopi = 0;
    while(loopi < length_line){
        entry[loopi + 2] = linestring[loopi];
        loopi++;
    }
    entry[length_line + 2] = ',';

    // Position Number.
    loopi = 0;
    while(loopi < length_lineoffset){
        entry[length_line + loopi + 3] = lineoffsetstring[loopi];
        loopi++;
    }
    entry[length_line + length_lineoffset + 3] = ',';

    // String.
    loopi = 0;
    while(loopi < length_value){
        entry[length_line + length_lineoffset + loopi + 4] = newvalue[loopi];
        loopi++;
    }
    entry[length_value + length_line + length_lineoffset + 4] = '\n';
    entry[length_value + length_line + length_lineoffset + 5] = '\0';

    entry = g_locale_to_utf8(
      entry,
      length_value + length_line + length_lineoffset + 5,
      NULL,
      NULL,
      NULL
    );

    return entry;
}

static void update_opened_files(void){
    gint page = 0;
    gint pages;
    gchar *content;
    GtkTextBuffer *buffer;
    GtkTextIter end;
    GtkTextIter start;
    GtkWidget *temp_text_view;

    buffer = gtk_text_buffer_new(NULL);
    temp_text_view = gtk_text_view_new_with_buffer(buffer);

    pages = gtk_notebook_get_n_pages(notebook) - 1;
    while(page <= pages){
        gtk_text_buffer_get_end_iter(
          buffer,
          &end
        );

        gtk_text_buffer_insert(
          buffer,
          &end,
          gtk_label_get_text(GTK_LABEL(gtk_notebook_get_tab_label(
            notebook,
            gtk_notebook_get_nth_page(
              notebook,
              page
            )
          ))),
          -1
        );
        gtk_text_buffer_insert(
          buffer,
          &end,
          "\n",
          1
        );

        page++;
    }

    gtk_text_buffer_get_bounds(
      buffer,
      &start,
      &end
    );

    content = gtk_text_buffer_get_text(
      buffer,
      &start,
      &end,
      FALSE
    );
    gchar *path = common_iterami_path("cfg/texteditor.cfg");
    g_file_set_contents(
      path,
      content,
      -1,
      NULL
    );
    g_free(content);
    g_free(path);

    gtk_widget_destroy(temp_text_view);
}
