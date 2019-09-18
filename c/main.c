#include <gtk/gtk.h>
#include <string.h>
#include "main.h"
#include "../../common/c/core.c"
#include "../../common/c/gtk.c"
#include "../../common/c/sort.c"

void activate(GtkApplication* app, gpointer data){
    gtk_window_present(GTK_WINDOW(window));
}

void block_insertdelete_signals(GtkTextBuffer *text_buffer){
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

gboolean check_equals_unsaved(void){
    return g_strcmp0(
      get_current_tab_label_text(),
      "UNSAVED"
    ) == 0;
}

void find_clear_tags(void){
    GtkTextIter end;
    GtkTextIter start;

    int page = gtk_notebook_get_n_pages(notebook) - 1;
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

const gchar* get_current_tab_label_text(void){
    return gtk_label_get_text(GTK_LABEL(gtk_notebook_get_tab_label(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    )));
}

gchar* get_find_find(void){
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

gboolean get_notebook_no_pages(void){
    return gtk_notebook_get_n_pages(notebook) <= 0;
}

GList* get_tabbox_children(GtkNotebook *tabnotebook, const gint page){
    return gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(
      tabnotebook,
      page
    )));
}

struct tabcontents get_tab_contents(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *tabnotebook;
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
      gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_notebook_get_nth_page(tabnotebook, 0))))),
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(g_list_nth_data(children, 0))),
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(gtk_notebook_get_nth_page(tabnotebook, 0))),
    };
    return result;
}

void go_to_line(void){
    if(get_notebook_no_pages()
      || gtk_entry_get_text_length(GTK_ENTRY(line_window_line)) <= 0){
        return;
    }

    const gchar *entry;
    int linenumber = 0;
    GtkTextIter line;
    tabcontents tab;

    entry = gtk_entry_get_text(GTK_ENTRY(line_window_line));

    int i = 0;
    size_t length_line = strlen(entry);
    while(i < length_line){
        linenumber *= 10;
        linenumber += entry[i] - '0';
        i++;
    }
    linenumber -= 1;

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
    g_signal_connect(
      app,
      "startup",
      G_CALLBACK(startup),
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

void menu_clearundoredo(void){
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

void menu_closetab(void){
    if(get_notebook_no_pages()){
        return;
    }

    gtk_notebook_remove_page(
      notebook,
      gtk_notebook_get_current_page(notebook)
    );

    update_opened_files();
}

void menu_deleteline(void){
    if(get_notebook_no_pages()){
        return;
    }

    int endlinenumber;
    int linenumber;
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
    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &start
    );
}

void menu_findbottom(void){
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

void menu_findline(void){
    if(!gtk_widget_is_visible(GTK_WIDGET(line_window))){
        gtk_widget_show_all(line_window);

        gint height = 0;
        gint width = 0;
        gint x = 0;
        gint y = 0;
        gtk_window_get_position(
          GTK_WINDOW(window),
          &x,
          &y
        );
        gtk_window_get_size(
          GTK_WINDOW(window),
          &width,
          &height
        );

        gtk_window_move(
          GTK_WINDOW(line_window),
          x + width - 320 - width_tabnotebook,
          y + height - 260
        );
    }
    gtk_window_present(GTK_WINDOW(line_window));

    GtkTextIter cursor;
    tabcontents tab;
    tab = get_tab_contents(-1);

    gtk_text_buffer_get_iter_at_mark(
      tab.text_buffer,
      &cursor,
      gtk_text_buffer_get_insert(tab.text_buffer)
    );
    gchar *line = g_strdup_printf(
      "%i",
      gtk_text_iter_get_line(&cursor) + 1
    );
    gtk_entry_set_text(
      GTK_ENTRY(line_window_line),
      line
    );
    g_free(line);

    gtk_widget_grab_focus(line_window_line);
}

void menu_findnext(void){
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

        if(gtk_text_iter_starts_tag(
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

void menu_findprevious(void){
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

        if(gtk_text_iter_starts_tag(
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

void menu_find_recursive(GtkTextBuffer *buffer, GtkTextIter start){
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

void menu_findreplace(void){
    if(!gtk_widget_is_visible(GTK_WIDGET(find_window))){
        gtk_widget_show_all(find_window);

        gint height = 0;
        gint width = 0;
        gint x = 0;
        gint y = 0;
        gtk_window_get_position(
          GTK_WINDOW(window),
          &x,
          &y
        );
        gtk_window_get_size(
          GTK_WINDOW(window),
          &width,
          &height
        );

        gtk_window_move(
          GTK_WINDOW(find_window),
          x + width - 320 - width_tabnotebook,
          y + height - 200
        );
    }
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

void menu_findreplaceall(void){
    if(get_notebook_no_pages()
      || !gtk_widget_is_visible(find_window)){
        return;
    }

    menu_refind();

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

    refinding = FALSE;
    while(!gtk_text_iter_is_end(&end)){
        gtk_text_buffer_get_start_iter(
          tab.text_buffer,
          &end
        );
        gtk_text_iter_forward_to_tag_toggle(
          &end,
          tag_found
        );

        if(gtk_text_iter_starts_tag(
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
    refinding = TRUE;

    menu_refind();
}

void menu_findtop(void){
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

void menu_hide(void){
    gtk_widget_hide(find_window);
    gtk_widget_hide(line_window);
}

void menu_movetab(gint movement){
    int position = gtk_notebook_get_current_page(notebook) + movement;

    if(position <= 0){
        position = 0;

    }else{
        int pages = gtk_notebook_get_n_pages(notebook);

        if(position >= pages){
            position = pages - 1;
        }
    }

    gtk_notebook_reorder_child(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      ),
      position
    );
}

void menu_newtab(void){
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
    text_view = new_textview(
      TRUE,
      NULL
    );
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
    gtk_notebook_popup_enable(tabnotebook);
    gtk_notebook_set_show_border(
      tabnotebook,
      FALSE
    );
    scrolled_window_map = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_map),
      new_textview(
        FALSE,
        "map"
      )
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
      new_textview(
        FALSE,
        NULL
      )
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
      new_textview(
        FALSE,
        NULL
      )
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
    if(width_tabnotebook == 0){
        gtk_widget_get_preferred_width(
          GTK_WIDGET(tabnotebook),
          NULL,
          &width_tabnotebook
        );
    }

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

    g_signal_connect(
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window_map)),
      "value-changed",
      G_CALLBACK(scroll_changed_map),
      NULL
    );
    g_signal_connect(
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window)),
      "value-changed",
      G_CALLBACK(scroll_changed_textview),
      NULL
    );
}

void menu_open(void){
    GtkFileChooser *chooser;
    GtkWidget *dialog_open;

    dialog_open = gtk_file_chooser_dialog_new(
      "Open...",
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
        gchar *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        open_file(filename);

        g_free(filename);
    }

    gtk_widget_destroy(dialog_open);
}

void menu_redo(void){
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
    int i = 2;
    size_t length_line = 0;
    while(entry[i] != ','){
        length_line++;
        i++;
    }
    int line = 0;
    i = 0;
    while(i < length_line){
        line *= 10;
        line += entry[i + 2] - '0';
        i++;
    }

    i = length_line + 3;
    int length_lineoffset = 0;
    while(entry[i] != ','){
        length_lineoffset++;
        i++;
    }
    int lineoffset = 0;
    i = 0;
    while(i < length_lineoffset){
        lineoffset *= 10;
        lineoffset += entry[i + length_line + 3] - '0';
        i++;
    }

    i = length_line + length_lineoffset + 4;
    size_t length_value = 0;
    while(entry[i] != '\n'){
        length_value++;
        i++;
    }
    gchar value[length_value + 1];
    i = 0;
    while(i < length_value){
        value[i] = entry[i + length_line + length_lineoffset + 4];
        if(value[i] == 30){
            value[i] = '\n';
        }
        i++;
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

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &selectstart
    );

    menu_refind();
}

void menu_refind(void){
    if(!refinding){
        return;
    }

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

void menu_save(void){
    if(get_notebook_no_pages()){
        return;
    }

    if(check_equals_unsaved()){
        menu_saveas();

    }else{
        save_tab(NULL);
    }
}

void menu_saveas(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkFileChooser *chooser;
    GtkWidget *dialog_saveas;

    dialog_saveas = gtk_file_chooser_dialog_new(
      "Save As...",
      GTK_WINDOW(window),
      GTK_FILE_CHOOSER_ACTION_SAVE,
      "_Cancel",
      GTK_RESPONSE_CANCEL,
      "_Save As",
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
        gchar *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        save_tab(filename);

        gtk_notebook_set_tab_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            gtk_notebook_get_current_page(notebook)
          ),
          filename
        );
        gtk_notebook_set_menu_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            gtk_notebook_get_current_page(notebook)
          ),
          filename
        );

        g_free(filename);
    }

    gtk_widget_destroy(dialog_saveas);

    update_opened_files();
}

void menu_sort(gboolean asc){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextIter end;
    GtkTextIter start;
    tabcontents tab;

    tab = get_tab_contents(-1);

    if(!gtk_text_buffer_get_selection_bounds(
        tab.text_buffer,
        &start,
        &end
      )){
        return;
    }

    int line_end = gtk_text_iter_get_line(&end);
    int line_start = gtk_text_iter_get_line(&start);

    if(line_start == line_end){
        return;
    }

    gtk_text_iter_set_line_offset(
      &start,
      0
    );
    if(!gtk_text_iter_ends_line(&end)){
        gtk_text_iter_forward_to_line_end(&end);
    }

    gchar *text = gtk_text_buffer_get_text(
      tab.text_buffer,
      &start,
      &end,
      FALSE
    );

    int line_count = line_end - line_start + 1;
    char *token;
    char *line_array[line_count];

    token = strtok(
      text,
      "\n"
    );
    int i;
    for(i = 0; i < line_count; i++){
        GtkTextIter temp_start;

        gtk_text_buffer_get_iter_at_line(
          tab.text_buffer,
          &temp_start,
          line_start + i
        );

        if(gtk_text_iter_ends_line(&temp_start)){
            line_array[i] = "";

        }else{
            line_array[i] = token;
            token = strtok(
              NULL,
              "\n"
            );
        }
    }

    qsort(
      line_array,
      line_count,
      sizeof(char *),
      asc
        ? sort_compare_strings_asc
        : sort_compare_strings_desc
    );

    gtk_text_buffer_delete(
      tab.text_buffer,
      &start,
      &end
    );
    gtk_text_iter_set_line(
      &start,
      line_start
    );

    for(i = 0; i < line_count; i++){
        size_t length = strlen(line_array[i]);

        gtk_text_buffer_insert(
          tab.text_buffer,
          &start,
          line_array[i],
          length
        );

        if(i != line_count - 1){
            gtk_text_buffer_insert(
              tab.text_buffer,
              &start,
              "\n",
              1
            );
        }
    }

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &start
    );
}

void menu_undo(void){
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
    int i = 2;
    size_t length_line = 0;
    while(entry[i] != ','){
        length_line++;
        i++;
    }
    int line = 0;
    i = 0;
    while(i < length_line){
        line *= 10;
        line += entry[i + 2] - '0';
        i++;
    }

    i = length_line + 3;
    int length_lineoffset = 0;
    while(entry[i] != ','){
        length_lineoffset++;
        i++;
    }
    int lineoffset = 0;
    i = 0;
    while(i < length_lineoffset){
        lineoffset *= 10;
        lineoffset += entry[i + length_line + 3] - '0';
        i++;
    }

    i = length_line + length_lineoffset + 4;
    size_t length_value = 0;
    while(entry[i] != '\n'){
        length_value++;
        i++;
    }
    gchar value[length_value + 1];
    i = 0;
    while(i < length_value){
        value[i] = entry[i + length_line + length_lineoffset + 4];
        if(value[i] == 30){
            value[i] = '\n';
        }
        i++;
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

    place_cursor(
      tab.text_view,
      tab.text_buffer,
      &selectstart
    );

    menu_refind();
}

GtkWidget* new_scrolled_window(void){
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

GtkWidget* new_textview(gboolean editable, gchar *name){
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

    gtk_text_view_set_cursor_visible(
      GTK_TEXT_VIEW(text_view),
      editable
    );
    gtk_text_view_set_editable(
      GTK_TEXT_VIEW(text_view),
      editable
    );

    if(name != NULL){
        gtk_widget_set_name(
          text_view,
          name
        );
    }

    return text_view;
}

void open_file(gchar *filename){
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
            menu_newtab();
        }

        tabcontents tab;
        tab = get_tab_contents(-1);

        gtk_notebook_set_tab_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            tab.page
          ),
          filename
        );
        gtk_notebook_set_menu_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            tab.page
          ),
          filename
        );

        block_insertdelete_signals(tab.text_buffer);
        gtk_text_buffer_set_text(
          tab.text_buffer,
          content,
          length
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

void place_cursor(GtkWidget *text_view, GtkTextBuffer *text_buffer, GtkTextIter *iter){
    gtk_text_buffer_place_cursor(
      text_buffer,
      iter
    );
    gtk_text_view_scroll_to_mark(
      GTK_TEXT_VIEW(text_view),
      gtk_text_buffer_get_insert(text_buffer),
      0,
      TRUE,
      0.5,
      0.5
    );
}

void save_tab(const gchar *filename){
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
    int linei = gtk_text_buffer_get_line_count(tab.text_buffer);
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
}

void scroll_changed_map(void){
    tabcontents tab;

    tab = get_tab_contents(-1);

    g_signal_handlers_block_by_func(
      tab.text_adjustment,
      G_CALLBACK(scroll_changed_textview),
      NULL
    );
    gtk_adjustment_set_value(
      tab.text_adjustment,
      (gtk_adjustment_get_upper(tab.text_adjustment) - gtk_adjustment_get_page_size(tab.text_adjustment))
        * (gtk_adjustment_get_value(tab.map_adjustment) / (gtk_adjustment_get_upper(tab.map_adjustment) - gtk_adjustment_get_page_size(tab.map_adjustment)))
    );
    g_signal_handlers_unblock_by_func(
      tab.text_adjustment,
      G_CALLBACK(scroll_changed_textview),
      NULL
    );
}

void scroll_changed_textview(void){
    tabcontents tab;

    tab = get_tab_contents(-1);

    g_signal_handlers_block_by_func(
      tab.map_adjustment,
      G_CALLBACK(scroll_changed_map),
      NULL
    );
    gtk_adjustment_set_value(
      tab.map_adjustment,
      (gtk_adjustment_get_upper(tab.map_adjustment) - gtk_adjustment_get_page_size(tab.map_adjustment))
        * (gtk_adjustment_get_value(tab.text_adjustment) / (gtk_adjustment_get_upper(tab.text_adjustment) - gtk_adjustment_get_page_size(tab.text_adjustment)))
    );
    g_signal_handlers_unblock_by_func(
      tab.map_adjustment,
      G_CALLBACK(scroll_changed_map),
      NULL
    );
}

void startup(GtkApplication* app, gpointer data){
    GtkAccelGroup *accelgroup;
    GtkWidget *box;
    GtkWidget *findreplaceall;
    GtkWidget *findreplacepane;
    GtkWidget *menu_edit;
    GtkWidget *menu_file;
    GtkWidget *menu_find;
    GtkWidget *menubar;
    GtkWidget *menuitem_edit;
    GtkWidget *menuitem_file;
    GtkWidget *menuitem_find;
    GtkWidget *scrolled_window_find;
    GtkWidget *scrolled_window_replace;

    gtk_init_gtk(
      app,
      "TextEditor.gtk"
    );

    // Setup scrollable notebook.
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_popup_enable(notebook);
    gtk_notebook_set_scrollable(
      notebook,
      TRUE
    );
    gtk_notebook_set_show_border(
      notebook,
      FALSE
    );

    // Setup menu items.
    menubar = gtk_menu_bar_new();
    accelgroup = gtk_accel_group_new();
    gtk_window_add_accel_group(
      GTK_WINDOW(window),
      accelgroup
    );
    // File menu.
    menu_file = gtk_menu_new();
    menuitem_file = gtk_menu_item_new_with_mnemonic("_File");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menu_file
    );
    gtk_add_menuitem(
      menu_file,
      "_New Tab",
      accelgroup,
      KEY_NEWTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_newtab),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      "_Open...",
      accelgroup,
      KEY_OPEN,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_open),
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_file,
      "_Save",
      accelgroup,
      KEY_SAVE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_save),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      "Save _As...",
      accelgroup,
      KEY_SAVE,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_saveas),
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_file,
      "_Next Tab",
      accelgroup,
      KEY_NEXTTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(gtk_notebook_next_page),
      GTK_WIDGET(notebook)
    );
    gtk_add_menuitem(
      menu_file,
      "_Previous Tab",
      accelgroup,
      KEY_PREVIOUSTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(gtk_notebook_prev_page),
      GTK_WIDGET(notebook)
    );
    gtk_add_menuitem(
      menu_file,
      "Move Tab _Left",
      accelgroup,
      KEY_MOVETABLEFT,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_movetab),
      (gpointer)-1
    );
    gtk_add_menuitem(
      menu_file,
      "Move Tab _Right",
      accelgroup,
      KEY_MOVETABRIGHT,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_movetab),
      (gpointer)1
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_file),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_file,
      "_Close Tab",
      accelgroup,
      KEY_CLOSETAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_closetab),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      "_Hide Windows",
      accelgroup,
      KEY_HIDEWINDOWS,
      0,
      G_CALLBACK(menu_hide),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      "_Quit",
      accelgroup,
      KEY_QUIT,
      GDK_CONTROL_MASK,
      G_CALLBACK(gtk_widget_destroy),
      window
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_file
    );
    // Edit menu.
    menu_edit = gtk_menu_new();
    menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_edit),
      menu_edit
    );
    gtk_add_menuitem(
      menu_edit,
      "_Undo",
      accelgroup,
      KEY_UNDO,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_undo),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "_Redo",
      accelgroup,
      KEY_UNDO,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_redo),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "Cl_ear Undo/Redo",
      accelgroup,
      KEY_UNDOCLEAR,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_clearundoredo),
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_edit,
      "_Copy",
      accelgroup,
      KEY_COPY,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "Cu_t",
      accelgroup,
      KEY_CUT,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "_Paste",
      accelgroup,
      KEY_PASTE,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_edit,
      "_Delete",
      accelgroup,
      KEY_DELETE,
      0,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "Delete _Line",
      accelgroup,
      KEY_DELETELINE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_deleteline),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "Delete Ne_xt Word",
      accelgroup,
      KEY_DELETE,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      "Delete Pre_vious Word",
      accelgroup,
      KEY_DELETEPREVIOUSWORD,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_edit,
      "Toggle _Overwrite",
      accelgroup,
      KEY_INSERT,
      0,
      NULL,
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_edit,
      "_Select All",
      accelgroup,
      KEY_SELECTALL,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_edit),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_edit,
      "Sort _Ascending",
      accelgroup,
      KEY_SORT,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_sort),
      (gpointer)TRUE
    );
    gtk_add_menuitem(
      menu_edit,
      "Sort Desce_nding",
      accelgroup,
      KEY_SORT,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_sort),
      (gpointer)FALSE
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menubar),
      menuitem_edit
    );
    // Find menu.
    menu_find = gtk_menu_new();
    menuitem_find = gtk_menu_item_new_with_mnemonic("F_ind");
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_find),
      menu_find
    );
    gtk_add_menuitem(
      menu_find,
      "Find _Next",
      accelgroup,
      KEY_FINDMORE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findnext),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      "Find _Previous",
      accelgroup,
      KEY_FINDMORE,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_findprevious),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      "_Find/Replace...",
      accelgroup,
      KEY_FIND,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findreplace),
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_find,
      "_Replace All",
      accelgroup,
      KEY_REPLACE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findreplaceall),
      NULL
    );
    gtk_menu_shell_append(
      GTK_MENU_SHELL(menu_find),
      gtk_separator_menu_item_new()
    );
    gtk_add_menuitem(
      menu_find,
      "Go to _Top",
      accelgroup,
      KEY_TOP,
      0,
      G_CALLBACK(menu_findtop),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      "Go to _Line...",
      accelgroup,
      KEY_LINE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findline),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      "Go to _Bottom",
      accelgroup,
      KEY_BOTTOM,
      0,
      G_CALLBACK(menu_findbottom),
      NULL
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
    findreplacepane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    find_window_find = new_textview(
      TRUE,
      NULL
    );
    scrolled_window_find = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_find),
      find_window_find
    );
    gtk_paned_pack1(
      GTK_PANED(findreplacepane),
      scrolled_window_find,
      TRUE,
      TRUE
    );
    find_window_replace = new_textview(
      TRUE,
      NULL
    );
    scrolled_window_replace = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_replace),
      find_window_replace
    );
    gtk_paned_pack2(
      GTK_PANED(findreplacepane),
      scrolled_window_replace,
      TRUE,
      TRUE
    );
    gtk_container_add(
      GTK_CONTAINER(find_window),
      findreplacepane
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

    // Open previously opened files.
    gchar *temp_path = core_iterami_path(CONFIG_PATH);
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

        int lines = gtk_text_buffer_get_line_count(temp_buffer);
        int line = 0;
        while(line < lines){
            gtk_text_buffer_get_iter_at_line(
              temp_buffer,
              &temp_start,
              line++
            );
            gtk_text_buffer_get_iter_at_line(
              temp_buffer,
              &temp_end,
              line
            );
            if(gtk_text_iter_backward_char(&temp_end)){
                gchar *filename;

                filename = gtk_text_buffer_get_text(
                  temp_buffer,
                  &temp_start,
                  &temp_end,
                  FALSE
                );
                open_file(filename);

                g_free(filename);
            }
        }

        gtk_widget_destroy(temp_text_view);
        g_object_ref_sink(temp_text_view);
    }

    g_free(temp_path);
    g_free(temp_content);
}

void text_deleted(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end){
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

void text_inserted(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *value){
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

void unblock_insertdelete_signals(GtkTextBuffer *text_buffer){
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

gchar* undoredo_entry(gchar *value, const gboolean inserted, const int line, const int lineoffset){
    size_t length_line = core_get_int_length(line);
    size_t length_lineoffset = core_get_int_length(lineoffset);

    char lineoffsetstring[length_lineoffset];
    char linestring[length_line];

    size_t length_value = strlen(value);
    value = g_locale_to_utf8(
      value,
      length_value,
      NULL,
      NULL,
      NULL
    );
    gchar newvalue[length_value];
    int i = 0;
    while(i < length_value){
        newvalue[i] = value[i];
        if(newvalue[i] == '\n'){
            newvalue[i] = 30;
        }
        i++;
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

    entry[0] = inserted ? '1' : '0';
    entry[1] = ',';

    // Line Number.
    i = 0;
    while(i < length_line){
        entry[i + 2] = linestring[i];
        i++;
    }
    entry[length_line + 2] = ',';

    // Position Number.
    i = 0;
    while(i < length_lineoffset){
        entry[length_line + i + 3] = lineoffsetstring[i];
        i++;
    }
    entry[length_line + length_lineoffset + 3] = ',';

    // String.
    i = 0;
    while(i < length_value){
        entry[length_line + length_lineoffset + i + 4] = newvalue[i];
        i++;
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

void update_opened_files(void){
    int page = 0;
    gchar *content;
    GtkTextBuffer *buffer;
    GtkTextIter end;
    GtkTextIter start;
    GtkWidget *temp_text_view;

    buffer = gtk_text_buffer_new(NULL);
    temp_text_view = gtk_text_view_new_with_buffer(buffer);

    int pages = gtk_notebook_get_n_pages(notebook) - 1;
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
              page++
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
    gchar *path = core_iterami_path(CONFIG_PATH);
    g_file_set_contents(
      path,
      content,
      -1,
      NULL
    );
    g_free(content);
    g_free(path);

    gtk_widget_destroy(temp_text_view);
    g_object_ref_sink(temp_text_view);
}
