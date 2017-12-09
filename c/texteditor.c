#include <gtk/gtk.h>

static gchar *finding = NULL;
static GtkNotebook *notebook;
static GtkWidget *find_window_find;
static GtkWidget *find_window_replace;
static GtkWidget *find_window;
static GtkWidget *menuitem_edit_redo;
static GtkWidget *menuitem_edit_undo;
static GtkWidget *window;

typedef struct tabcontents{
  int page;
  GtkWidget *text_view;
  GtkTextBuffer *text_buffer;
  GtkTextBuffer *undo_buffer;
  GtkTextBuffer *redo_buffer;
} tabcontents;

static struct tabcontents get_tab_contents(gint page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    static GtkNotebook *tabnotebook;
    static GtkWidget *box;
    static GtkWidget *redo_text_view;
    static GtkWidget *text_view;
    static GtkWidget *undo_text_view;

    box = gtk_bin_get_child(GTK_BIN(gtk_notebook_get_nth_page(
      notebook,
      page
    )));
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
    text_view = g_list_nth_data(
      children,
      0
    );
    tabnotebook = g_list_nth_data(
      children,
      1
    );

    tabcontents result = {
      page,
      text_view,
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view))
    };
    return result;
}

static gboolean get_notebook_has_pages(){
    return gtk_notebook_get_n_pages(notebook) <= 0;
}

static void menu_undo(){
    if(get_notebook_has_pages()){
        return;
    }

    static tabcontents tab;

    tab = get_tab_contents(-1);
}

static void menu_redo(){
    if(get_notebook_has_pages()){
        return;
    }

    static tabcontents tab;

    tab = get_tab_contents(-1);
}

static const gchar* get_current_tab_label_text(){
    return gtk_label_get_text(GTK_LABEL(gtk_notebook_get_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    )));
}

static gboolean check_equals_unsaved(){
    return g_strcmp0(
      get_current_tab_label_text(),
      "UNSAVED"
    ) == 0;
}

static GtkWidget* new_textview(){
    static GtkTextBuffer *buffer;
    static GtkWidget *text_view;

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

    return text_view;
}

static void new_tab(){
    static GtkNotebook *tabnotebook;
    static GtkWidget *scrolled_window;
    static GtkWidget *tabbox;
    static GtkWidget *text_view;
    static GtkWidget *undobox;

    tabbox = gtk_box_new(
      GTK_ORIENTATION_HORIZONTAL,
      1
    );
    text_view = new_textview();
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
    gtk_box_pack_start(
      GTK_BOX(tabbox),
      scrolled_window,
      TRUE,
      TRUE,
      0
    );
    tabnotebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_append_page(
      tabnotebook,
      new_textview(),
      gtk_label_new("Map")
    );
    undobox = gtk_box_new(
      GTK_ORIENTATION_VERTICAL,
      1
    );
    gtk_box_pack_start(
      GTK_BOX(undobox),
      new_textview(),
      TRUE,
      TRUE,
      0
    );
    gtk_box_pack_start(
      GTK_BOX(undobox),
      new_textview(),
      TRUE,
      TRUE,
      0
    );
    gtk_notebook_append_page(
      tabnotebook,
      undobox,
      gtk_label_new("Undo")
    );
    gtk_notebook_append_page(
      tabnotebook,
      new_textview(),
      gtk_label_new("Info")
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
}

static void save_tab(const char *filename){
    if(!filename){
        filename = get_current_tab_label_text();
    }

    static gchar *content;
    static GtkTextIter end;
    static GtkTextIter start;
    static tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_start_iter(
      tab.text_buffer,
      &start
    );
    gtk_text_buffer_get_end_iter(
      tab.text_buffer,
      &end
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
    if(get_notebook_has_pages()){
        return;
    }

    gtk_notebook_remove_page(
      notebook,
      gtk_notebook_get_current_page(notebook)
    );
}

static void menu_open(){
    static GtkWidget *dialog_open;

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
    if(gtk_dialog_run(GTK_DIALOG(dialog_open)) == GTK_RESPONSE_ACCEPT){
        static char *filename;
        static gchar *content;
        static gssize length;
        static GtkFileChooser *chooser;

        chooser = GTK_FILE_CHOOSER(dialog_open);
        filename = gtk_file_chooser_get_filename(chooser);

        if(g_file_get_contents(filename, &content, &length, NULL)){
            if(g_utf8_validate(
              content,
              length,
              NULL
            )){
                if(get_notebook_has_pages()
                  || !check_equals_unsaved()){
                    new_tab();
                }

                static tabcontents tab;
                tab =  get_tab_contents(-1);

                gtk_text_buffer_set_text(
                  tab.text_buffer,
                  content,
                  length
                );
                gtk_notebook_set_tab_label(
                  notebook,
                  gtk_notebook_get_nth_page(
                    notebook,
                    tab.page
                  ),
                  gtk_label_new(filename)
                );
            }
        }

        g_free(content);
        g_free(filename);
    }

    gtk_widget_destroy(dialog_open);
}

static void menu_saveas(){
    if(get_notebook_has_pages()){
        return;
    }

    static GtkWidget *dialog_saveas;
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
    if(gtk_dialog_run(GTK_DIALOG(dialog_saveas)) == GTK_RESPONSE_ACCEPT){
        save_tab(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog_saveas)));
    }

    gtk_widget_destroy(dialog_saveas);
}

static void menu_save(){
    if(get_notebook_has_pages()){
        return;
    }

    if(check_equals_unsaved()){
        menu_saveas();

    }else{
        save_tab(NULL);
    }
}

static void find_clear_tags(){
    static gint page;
    static GtkTextIter end;
    static GtkTextIter start;

    page = gtk_notebook_get_n_pages(notebook) - 1;
    while(page >= 0){
        static tabcontents tab;
        tab = get_tab_contents(page);
        gtk_text_buffer_get_start_iter(
          tab.text_buffer,
          &start
        );
        gtk_text_buffer_get_end_iter(
          tab.text_buffer,
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

static void find_close(){
    finding = NULL;
    find_clear_tags();
}

static gchar* get_find_find(){
    static GtkTextBuffer *buffer;
    static GtkTextIter findend;
    static GtkTextIter findstart;

    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_window_find));
    gtk_text_buffer_get_start_iter(
      buffer,
      &findstart
    );
    gtk_text_buffer_get_end_iter(
      buffer,
      &findend
    );

    return gtk_text_buffer_get_text(
      buffer,
      &findstart,
      &findend,
      FALSE
    );
}

static void menu_find_recursive(GtkTextBuffer *buffer, GtkTextIter start){
    static GtkTextIter match_end;
    static GtkTextIter match_start;
    if(gtk_text_iter_forward_search(
      &start,
      finding,
      0,
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

static void menu_find(){
    gtk_widget_show_all(find_window);

    finding = get_find_find();
    if(finding
      && finding[0] == '\0'){
        return;
    }

    static GtkTextIter tabstart;
    static tabcontents tab;

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

static void menu_findnext(){
    if(get_notebook_has_pages()){
        return;
    }

    if(!finding
      || finding != get_find_find()){
        menu_find();
    }

    static GtkTextIter cursor;
    static GtkTextTag *tag_found;
    static tabcontents tab;

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
        static GtkTextIter end;
        static GtkTextIter start;

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

        gtk_text_buffer_select_range(
          tab.text_buffer,
          &end,
          &start
        );
    }
    gtk_text_view_scroll_to_iter(
      GTK_TEXT_VIEW(tab.text_view),
      &cursor,
      0,
      TRUE,
      0,
      0
    );
}

static void menu_findprevious(){
    if(get_notebook_has_pages()){
        return;
    }

    if(!finding
      || finding != get_find_find()){
        menu_find();
    }

    static GtkTextIter cursor;
    static GtkTextTag *tag_found;
    static tabcontents tab;

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
        static GtkTextIter end;
        static GtkTextIter start;

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

        gtk_text_buffer_select_range(
          tab.text_buffer,
          &start,
          &end
        );
    }
    gtk_text_view_scroll_to_iter(
      GTK_TEXT_VIEW(tab.text_view),
      &cursor,
      0,
      TRUE,
      0,
      0
    );
}

static void menu_findreplaceall(){
}

static void menu_findbottom(){
    if(get_notebook_has_pages()){
        return;
    }

    static GtkTextIter end;
    static tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_end_iter(
      tab.text_buffer,
      &end
    );

    gtk_text_buffer_place_cursor(
      tab.text_buffer,
      &end
    );
    gtk_text_view_scroll_to_iter(
      GTK_TEXT_VIEW(tab.text_view),
      &end,
      0,
      TRUE,
      0,
      0
    );
}

static void menu_findtop(){
    if(get_notebook_has_pages()){
        return;
    }

    static GtkTextIter start;
    static tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_start_iter(
      tab.text_buffer,
      &start
    );

    gtk_text_buffer_place_cursor(
      tab.text_buffer,
      &start
    );
    gtk_text_view_scroll_to_iter(
      GTK_TEXT_VIEW(tab.text_view),
      &start,
      0,
      TRUE,
      0,
      0
    );
}

static void menu_deleteline(){
    if(get_notebook_has_pages()){
        return;
    }

    static gint endlinenumber;
    static gint linenumber;
    static GtkTextIter end;
    static GtkTextIter endall;
    static GtkTextIter line;
    static GtkTextIter start;
    static tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_iter_at_mark(
      tab.text_buffer,
      &line,
      gtk_text_buffer_get_insert(tab.text_buffer)
    );
    linenumber = gtk_text_iter_get_line(&line);
    gtk_text_buffer_get_iter_at_line(
      tab.text_buffer,
      &endall,
      linenumber + 1
    );
    endlinenumber = gtk_text_iter_get_line(&endall);

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
          linenumber - 1
        );
        gtk_text_iter_forward_to_line_end(&start);
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

static void activate(GtkApplication* app, gpointer user_data){
    static GtkAccelGroup *accelgroup;
    static GtkCssProvider *provider;
    static GtkWidget *box;
    static GtkWidget *findnext;
    static GtkWidget *findprevious;
    static GtkWidget *findreplaceall;
    static GtkWidget *innerbox;
    static GtkWidget *menubar;
    static GtkWidget *menuitem_edit_copy;
    static GtkWidget *menuitem_edit_cut;
    static GtkWidget *menuitem_edit_delete;
    static GtkWidget *menuitem_edit_deleteline;
    static GtkWidget *menuitem_edit_deletenextword;
    static GtkWidget *menuitem_edit_deletepreviousword;
    static GtkWidget *menuitem_edit_paste;
    static GtkWidget *menuitem_edit_selectall;
    static GtkWidget *menuitem_edit_sort;
    static GtkWidget *menuitem_edit;
    static GtkWidget *menuitem_file_closetab;
    static GtkWidget *menuitem_file_newtab;
    static GtkWidget *menuitem_file_open;
    static GtkWidget *menuitem_file_print;
    static GtkWidget *menuitem_file_quit;
    static GtkWidget *menuitem_file_save;
    static GtkWidget *menuitem_file_saveas;
    static GtkWidget *menuitem_file;
    static GtkWidget *menuitem_find_find;
    static GtkWidget *menuitem_find_findnext;
    static GtkWidget *menuitem_find_findprevious;
    static GtkWidget *menuitem_find_gotobottom;
    static GtkWidget *menuitem_find_gotoline;
    static GtkWidget *menuitem_find_gototop;
    static GtkWidget *menuitem_find;
    static GtkWidget *menumenu_edit;
    static GtkWidget *menumenu_file;
    static GtkWidget *menumenu_find;
    static GtkWidget *outerbox;

    // Setup CSS.
    provider = gtk_css_provider_new();
    gtk_style_context_add_provider_for_screen(
      gdk_display_get_default_screen(gdk_display_get_default()),
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
    menuitem_file_closetab = gtk_menu_item_new_with_mnemonic("_Close Tab");
    gtk_widget_add_accelerator(
      menuitem_file_closetab,
      "activate",
      accelgroup,
      GDK_KEY_w,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_newtab = gtk_menu_item_new_with_mnemonic("_New Tab");
    gtk_widget_add_accelerator(
      menuitem_file_newtab,
      "activate",
      accelgroup,
      GDK_KEY_t,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_open = gtk_menu_item_new_with_mnemonic("_Open...");
    gtk_widget_add_accelerator(
      menuitem_file_open,
      "activate",
      accelgroup,
      GDK_KEY_o,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_print = gtk_menu_item_new_with_mnemonic("_Print...");
    gtk_widget_add_accelerator(
      menuitem_file_print,
      "activate",
      accelgroup,
      GDK_KEY_p,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_quit = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_widget_add_accelerator(
      menuitem_file_quit,
      "activate",
      accelgroup,
      GDK_KEY_q,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_save = gtk_menu_item_new_with_mnemonic("_Save");
    gtk_widget_add_accelerator(
      menuitem_file_save,
      "activate",
      accelgroup,
      GDK_KEY_s,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_file_saveas = gtk_menu_item_new_with_mnemonic("Save _As...");
    gtk_widget_add_accelerator(
      menuitem_file_saveas,
      "activate",
      accelgroup,
      GDK_KEY_s,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      GTK_ACCEL_VISIBLE
    );
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menumenu_file
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_newtab
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_closetab
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_open
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_save
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_saveas
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_print
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_file),
      menuitem_file_quit
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_file
    );
    // Edit menu.
    menumenu_edit = gtk_menu_new();
    menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
    menuitem_edit_copy = gtk_menu_item_new_with_mnemonic("_Copy");
    gtk_widget_add_accelerator(
      menuitem_edit_copy,
      "activate",
      accelgroup,
      GDK_KEY_c,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_cut = gtk_menu_item_new_with_mnemonic("Cu_t");
    gtk_widget_add_accelerator(
      menuitem_edit_cut,
      "activate",
      accelgroup,
      GDK_KEY_x,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_delete = gtk_menu_item_new_with_mnemonic("_Delete");
    gtk_widget_add_accelerator(
      menuitem_edit_delete,
      "activate",
      accelgroup,
      GDK_KEY_Delete,
      0,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_deleteline = gtk_menu_item_new_with_mnemonic("Delete _Line");
    gtk_widget_add_accelerator(
      menuitem_edit_deleteline,
      "activate",
      accelgroup,
      GDK_KEY_d,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_deletenextword = gtk_menu_item_new_with_mnemonic("Delete Ne_xt Word");
    gtk_widget_add_accelerator(
      menuitem_edit_deletenextword,
      "activate",
      accelgroup,
      GDK_KEY_Delete,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_deletepreviousword = gtk_menu_item_new_with_mnemonic("Delete Pre_vious Word");
    gtk_widget_add_accelerator(
      menuitem_edit_deletepreviousword,
      "activate",
      accelgroup,
      GDK_KEY_BackSpace,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_paste = gtk_menu_item_new_with_mnemonic("_Paste");
    gtk_widget_add_accelerator(
      menuitem_edit_paste,
      "activate",
      accelgroup,
      GDK_KEY_v,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_redo = gtk_menu_item_new_with_mnemonic("_Redo");
    gtk_widget_add_accelerator(
      menuitem_edit_redo,
      "activate",
      accelgroup,
      GDK_KEY_z,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_selectall = gtk_menu_item_new_with_mnemonic("Select _All");
    gtk_widget_add_accelerator(
      menuitem_edit_selectall,
      "activate",
      accelgroup,
      GDK_KEY_a,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_sort = gtk_menu_item_new_with_mnemonic("_Sort Lines");
    gtk_widget_add_accelerator(
      menuitem_edit_sort,
      "activate",
      accelgroup,
      GDK_KEY_r,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_edit_undo = gtk_menu_item_new_with_mnemonic("_Undo");
    gtk_widget_add_accelerator(
      menuitem_edit_undo,
      "activate",
      accelgroup,
      GDK_KEY_z,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_edit),
      menumenu_edit
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_undo
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_redo
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_copy
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_cut
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_paste
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_delete
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_deleteline
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_deletenextword
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_deletepreviousword
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_selectall
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_edit),
      menuitem_edit_sort
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_edit
    );
    // Find menu.
    menumenu_find = gtk_menu_new();
    menuitem_find = gtk_menu_item_new_with_mnemonic("F_ind");
    menuitem_find_find = gtk_menu_item_new_with_mnemonic("_Find and Replace...");
    gtk_widget_add_accelerator(
      menuitem_find_find,
      "activate",
      accelgroup,
      GDK_KEY_f,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_find_findnext = gtk_menu_item_new_with_mnemonic("Find _Next");
    gtk_widget_add_accelerator(
      menuitem_find_findnext,
      "activate",
      accelgroup,
      GDK_KEY_g,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_find_findprevious = gtk_menu_item_new_with_mnemonic("Find _Previous");
    gtk_widget_add_accelerator(
      menuitem_find_findprevious,
      "activate",
      accelgroup,
      GDK_KEY_g,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_find_gotobottom = gtk_menu_item_new_with_mnemonic("Go to _Bottom");
    gtk_widget_add_accelerator(
      menuitem_find_gotobottom,
      "activate",
      accelgroup,
      GDK_KEY_End,
      0,
      GTK_ACCEL_VISIBLE
    );
    menuitem_find_gotoline = gtk_menu_item_new_with_mnemonic("Go to _Line...");
    gtk_widget_add_accelerator(
      menuitem_find_gotoline,
      "activate",
      accelgroup,
      GDK_KEY_l,
      GDK_CONTROL_MASK,
      GTK_ACCEL_VISIBLE
    );
    menuitem_find_gototop = gtk_menu_item_new_with_mnemonic("Go to _Top");
    gtk_widget_add_accelerator(
      menuitem_find_gototop,
      "activate",
      accelgroup,
      GDK_KEY_Home,
      0,
      GTK_ACCEL_VISIBLE
    );
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_find),
      menumenu_find
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_find
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_findnext
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_findprevious
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      gtk_separator_menu_item_new()
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_gototop
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_gotobottom
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menumenu_find),
      menuitem_find_gotoline
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
      "Find and Replace..."
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
    find_window_find = new_textview();
    gtk_box_pack_start(
      GTK_BOX(outerbox),
      find_window_find,
      TRUE,
      TRUE,
      0
    );
    innerbox = gtk_box_new(
      GTK_ORIENTATION_HORIZONTAL,
      0
    );
    findprevious = gtk_button_new_with_label("←");
    gtk_box_pack_start(
      GTK_BOX(innerbox),
      findprevious,
      TRUE,
      TRUE,
      0
    );
    findnext = gtk_button_new_with_label("→");
    gtk_box_pack_start(
      GTK_BOX(innerbox),
      findnext,
      TRUE,
      TRUE,
      0
    );
    findreplaceall = gtk_button_new_with_mnemonic("_Replace All");
    gtk_box_pack_start(
      GTK_BOX(innerbox),
      findreplaceall,
      TRUE,
      TRUE,
      0
    );
    gtk_box_pack_start(
      GTK_BOX(outerbox),
      innerbox,
      FALSE,
      FALSE,
      0
    );
    find_window_replace = new_textview();
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
    g_signal_connect(
      findnext,
      "clicked",
      G_CALLBACK(menu_findnext),
      NULL
    );
    g_signal_connect(
      findprevious,
      "clicked",
      G_CALLBACK(menu_findprevious),
      NULL
    );
    g_signal_connect(
      findreplaceall,
      "clicked",
      G_CALLBACK(menu_findreplaceall),
      NULL
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
      G_CALLBACK(find_close),
      NULL
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

    // Disable nonfunctional menu items.
    gtk_widget_set_sensitive(
      menuitem_file_print,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_undo,
      FALSE
    );
    gtk_widget_set_sensitive(
      menuitem_edit_redo,
      FALSE
    );
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
    gtk_widget_set_sensitive(
      menuitem_find_gotoline,
      FALSE
    );
}

int main(int argc, char **argv){
    static GtkApplication *app;

    app = gtk_application_new(
      "com.iterami.texteditor",
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
