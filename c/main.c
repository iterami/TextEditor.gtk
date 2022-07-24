#include <gtk/gtk.h>
#include <string.h>
#include "main.h"
#include "../../common/c/core.c"
#include "../../common/c/gtk.c"
#include "../../common/c/sort.c"

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
      LABEL_UNSAVED
    ) == 0;
}

void find_clear_tags(void){
    GtkTextIter end;
    GtkTextIter start;

    int page = gtk_notebook_get_n_pages(notebook) - 1;
    while(page >= 0){
        GtkTextBuffer *textbuffer;

        textbuffer = tab_get_textbuffer(page);

        gtk_text_buffer_get_bounds(
          textbuffer,
          &start,
          &end
        );

        gtk_text_buffer_remove_all_tags(
          textbuffer,
          &start,
          &end
        );
        page--;
    }
}

const gchar* get_current_tab_label_text(void){
    return gtk_notebook_get_tab_label_text(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    );
}

const gchar* get_current_tab_menu_text(void){
    return gtk_notebook_get_menu_label_text(
      notebook,
      gtk_notebook_get_nth_page(
        notebook,
        gtk_notebook_get_current_page(notebook)
      )
    );
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

void go_to_line(void){
    if(get_notebook_no_pages()
      || gtk_entry_get_text_length(GTK_ENTRY(line_window_line)) <= 0){
        return;
    }

    const gchar *entry;
    int linenumber = 0;

    entry = gtk_entry_get_text(GTK_ENTRY(line_window_line));

    int i = 0;
    size_t length_line = strlen(entry);
    while(i < length_line){
        linenumber *= 10;
        linenumber += entry[i] - '0';
        i++;
    }
    linenumber -= 1;

    GtkTextBuffer *textbuffer;
    GtkTextIter line;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_iter_at_line(
      textbuffer,
      &line,
      linenumber
    );

    place_cursor(
      textview,
      textbuffer,
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
      G_CALLBACK(gtk_activate),
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
    GtkTextBuffer *redobuffer;
    GtkTextBuffer *undobuffer;
    GtkTextIter end;
    GtkTextIter start;

    redobuffer = tab_get_redobuffer(-1);
    undobuffer = tab_get_undobuffer(-1);

    gtk_text_buffer_set_text(
      undobuffer,
      "",
      0
    );
    gtk_text_buffer_set_text(
      redobuffer,
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

    if(get_notebook_no_pages()){
        gtk_window_set_title(
          GTK_WINDOW(window),
          TITLE
        );
    }
}

void menu_deleteline(void){
    if(get_notebook_no_pages()){
        return;
    }

    int endlinenumber;
    int linenumber;
    GtkTextBuffer *textbuffer;
    GtkTextIter end;
    GtkTextIter line;
    GtkTextIter start;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_iter_at_mark(
      textbuffer,
      &line,
      gtk_text_buffer_get_insert(textbuffer)
    );
    linenumber = gtk_text_iter_get_line(&line);
    gtk_text_buffer_get_iter_at_line(
      textbuffer,
      &end,
      linenumber + 1
    );
    endlinenumber = gtk_text_iter_get_line(&end);

    // Deleting first line.
    if(linenumber == 0){
        gtk_text_buffer_get_start_iter(
          textbuffer,
          &start
        );
        if(endlinenumber == 0){
            gtk_text_buffer_get_end_iter(
              textbuffer,
              &end
            );

        }else{
            gtk_text_buffer_get_iter_at_line(
              textbuffer,
              &end,
              1
            );
        }

    // Deleting last line.
    }else if(linenumber == endlinenumber){
        gtk_text_buffer_get_iter_at_line(
          textbuffer,
          &start,
          linenumber
        );
        gtk_text_iter_backward_char(&start);
        gtk_text_buffer_get_end_iter(
          textbuffer,
          &end
        );

    // Deleting any other line.
    }else{
        gtk_text_buffer_get_iter_at_line(
          textbuffer,
          &start,
          linenumber
        );
        gtk_text_buffer_get_iter_at_line(
          textbuffer,
          &end,
          linenumber + 1
        );
    }

    gtk_text_buffer_delete(
      textbuffer,
      &start,
      &end
    );
    place_cursor(
      textview,
      textbuffer,
      &start
    );
}

void menu_findbottom(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextBuffer *textbuffer;
    GtkTextIter end;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_end_iter(
      textbuffer,
      &end
    );

    place_cursor(
      textview,
      textbuffer,
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
          x + width - 325 - width_sidebar,
          y + height - 275
        );
    }
    gtk_window_present(GTK_WINDOW(line_window));

    GtkTextBuffer *textbuffer;
    GtkTextIter cursor;

    textbuffer = tab_get_textbuffer(-1);

    gtk_text_buffer_get_iter_at_mark(
      textbuffer,
      &cursor,
      gtk_text_buffer_get_insert(textbuffer)
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

    GtkTextBuffer *textbuffer;
    GtkTextIter cursor;
    GtkTextTag *tag_found;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_iter_at_mark(
      textbuffer,
      &cursor,
      gtk_text_buffer_get_insert(textbuffer)
    );
    tag_found = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(textbuffer),
      TAG_FOUND
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
          textview,
          textbuffer,
          &cursor
        );
        gtk_text_buffer_select_range(
          textbuffer,
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

    GtkTextBuffer *textbuffer;
    GtkTextIter cursor;
    GtkTextTag *tag_found;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_iter_at_mark(
      textbuffer,
      &cursor,
      gtk_text_buffer_get_insert(textbuffer)
    );
    tag_found = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(textbuffer),
      TAG_FOUND
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
          textview,
          textbuffer,
          &cursor
        );
        gtk_text_buffer_select_range(
          textbuffer,
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
          TAG_FOUND,
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
          x + width - 325 - width_sidebar,
          y + height - 200
        );
    }
    gtk_window_present(GTK_WINDOW(find_window));

    GtkTextBuffer *findbuffer;
    GtkTextBuffer *textbuffer;

    findbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(find_window_find));
    textbuffer = tab_get_textbuffer(-1);

    if(gtk_text_buffer_get_has_selection(textbuffer)){
        GtkTextIter end;
        GtkTextIter start;
        gtk_text_buffer_get_selection_bounds(
          textbuffer,
          &start,
          &end
        );

        finding = gtk_text_buffer_get_text(
          textbuffer,
          &start,
          &end,
          FALSE
        );
        gtk_text_buffer_set_text(
          findbuffer,
          finding,
          -1
        );
    }

    gtk_widget_grab_focus(find_window_find);

    GtkTextIter findend;
    GtkTextIter findstart;

    gtk_text_buffer_get_bounds(
      findbuffer,
      &findstart,
      &findend
    );

    gtk_text_buffer_select_range(
      findbuffer,
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
    GtkTextBuffer *textbuffer;
    GtkTextIter end;
    GtkTextIter replace_end;
    GtkTextIter replace_start;
    GtkTextIter start;
    GtkTextTag *tag_found;

    textbuffer = tab_get_textbuffer(-1);

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
      gtk_text_buffer_get_tag_table(textbuffer),
      TAG_FOUND
    );

    refinding = FALSE;
    while(!gtk_text_iter_is_end(&end)){
        gtk_text_buffer_get_start_iter(
          textbuffer,
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
              textbuffer,
              &start,
              &end
            );

            gtk_text_buffer_get_iter_at_offset(
              textbuffer,
              &start,
              offset
            );
            gtk_text_buffer_insert(
              textbuffer,
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

    GtkTextBuffer *textbuffer;
    GtkTextIter start;
    GtkTextView *textview;

    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_start_iter(
      textbuffer,
      &start
    );

    place_cursor(
      textview,
      textbuffer,
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

    update_opened_files();
}

void menu_newtab(void){
    GtkWidget *scrolled_window_map;
    GtkWidget *scrolled_window_redo;
    GtkWidget *scrolled_window_undo;
    GtkWidget *scrolled_window;
    GtkWidget *sidebar;
    GtkWidget *tabbox;
    GtkWidget *text_view;
    GtkWidget *undoredo;

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
    sidebar = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_size_request(
      sidebar,
      width_sidebar,
      0
    );
    scrolled_window_map = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_map),
      new_textview(
        FALSE,
        "map"
      )
    );
    gtk_paned_pack1(
      GTK_PANED(sidebar),
      scrolled_window_map,
      TRUE,
      TRUE
    );
    undoredo = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    scrolled_window_undo = new_scrolled_window();
    gtk_container_add(
      GTK_CONTAINER(scrolled_window_undo),
      new_textview(
        FALSE,
        NULL
      )
    );
    gtk_paned_pack1(
      GTK_PANED(undoredo),
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
      GTK_PANED(undoredo),
      scrolled_window_redo,
      TRUE,
      TRUE
    );
    gtk_paned_pack2(
      GTK_PANED(sidebar),
      undoredo,
      TRUE,
      TRUE
    );
    gtk_box_pack_start(
      GTK_BOX(tabbox),
      GTK_WIDGET(sidebar),
      FALSE,
      FALSE,
      0
    );
    gtk_notebook_append_page_menu(
      notebook,
      tabbox,
      gtk_label_new(LABEL_UNSAVED),
      gtk_label_new(LABEL_UNSAVED)
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

    gtk_window_present(GTK_WINDOW(window));
}

void menu_open(void){
    GtkFileChooser *chooser;
    GtkWidget *dialog_open;

    dialog_open = gtk_file_chooser_dialog_new(
      LABEL_OPENDIALOG,
      GTK_WINDOW(window),
      GTK_FILE_CHOOSER_ACTION_OPEN,
      LABEL_CANCEL,
      GTK_RESPONSE_CANCEL,
      LABEL_OPENBUTTON,
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    chooser = GTK_FILE_CHOOSER(dialog_open);
    if(!check_equals_unsaved()){
        gtk_file_chooser_set_file(
          chooser,
          g_file_new_for_path(get_current_tab_menu_text()),
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
        update_opened_files();
    }

    gtk_widget_destroy(dialog_open);
}

void menu_redo(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextBuffer *redobuffer;
    GtkTextIter redoend;
    GtkTextIter redostart;

    redobuffer = tab_get_redobuffer(-1);

    gtk_text_buffer_get_start_iter(
      redobuffer,
      &redostart
    );
    gtk_text_buffer_get_iter_at_line(
      redobuffer,
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
    GtkTextBuffer *mapbuffer;
    GtkTextBuffer *textbuffer;
    GtkTextBuffer *undobuffer;
    GtkTextIter mapstart;
    GtkTextIter selectstart;
    GtkTextIter undostart;
    GtkTextView *textview;

    mapbuffer = tab_get_mapbuffer(-1);
    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);
    undobuffer = tab_get_undobuffer(-1);

    gtk_text_buffer_get_start_iter(
      undobuffer,
      &undostart
    );

    entry = gtk_text_buffer_get_text(
      redobuffer,
      &redostart,
      &redoend,
      FALSE
    );

    gtk_text_buffer_insert(
      undobuffer,
      &undostart,
      entry,
      -1
    );
    gtk_text_buffer_delete(
      redobuffer,
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
    block_insertdelete_signals(textbuffer);
    gtk_text_buffer_get_iter_at_line_offset(
      textbuffer,
      &selectstart,
      line,
      lineoffset
    );

    gtk_text_buffer_get_iter_at_offset(
      mapbuffer,
      &mapstart,
      gtk_text_iter_get_offset(&selectstart)
    );

    if(inserted){
        gtk_text_buffer_insert(
          textbuffer,
          &selectstart,
          value,
          -1
        );
        gtk_text_buffer_insert(
          mapbuffer,
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
          mapbuffer,
          &mapend,
          gtk_text_iter_get_offset(&selectend)
        );

        gtk_text_buffer_delete(
          textbuffer,
          &selectstart,
          &selectend
        );
        gtk_text_buffer_delete(
          mapbuffer,
          &mapstart,
          &mapend
        );
    }
    unblock_insertdelete_signals(textbuffer);

    place_cursor(
      textview,
      textbuffer,
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

    GtkTextBuffer *textbuffer;
    GtkTextIter tabstart;

    find_clear_tags();

    textbuffer = tab_get_textbuffer(-1);

    gtk_text_buffer_get_start_iter(
      textbuffer,
      &tabstart
    );

    menu_find_recursive(
      textbuffer,
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
        save_tab(get_current_tab_menu_text());
    }
}

void menu_saveas(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkFileChooser *chooser;
    GtkWidget *dialog_saveas;

    dialog_saveas = gtk_file_chooser_dialog_new(
      LABEL_SAVEASDIALOG,
      GTK_WINDOW(window),
      GTK_FILE_CHOOSER_ACTION_SAVE,
      LABEL_CANCEL,
      GTK_RESPONSE_CANCEL,
      LABEL_SAVEASBUTTON,
      GTK_RESPONSE_ACCEPT,
      NULL
    );
    chooser = GTK_FILE_CHOOSER(dialog_saveas);
    if(!check_equals_unsaved()){
        gtk_file_chooser_set_file(
          chooser,
          g_file_new_for_path(get_current_tab_menu_text()),
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
          g_path_get_basename(filename)
        );
        gtk_notebook_set_menu_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            gtk_notebook_get_current_page(notebook)
          ),
          filename
        );
        gtk_window_set_title(
          GTK_WINDOW(window),
          g_path_get_basename(filename)
        );

        g_free(filename);
        update_opened_files();
    }

    gtk_widget_destroy(dialog_saveas);
}

void menu_sort(gboolean asc){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextBuffer *textbuffer;
    GtkTextIter end;
    GtkTextIter start;

    textbuffer = tab_get_textbuffer(-1);

    if(!gtk_text_buffer_get_selection_bounds(
        textbuffer,
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
      textbuffer,
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
          textbuffer,
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
      textbuffer,
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
          textbuffer,
          &start,
          line_array[i],
          length
        );

        if(i != line_count - 1){
            gtk_text_buffer_insert(
              textbuffer,
              &start,
              "\n",
              1
            );
        }
    }

    GtkTextView *textview;

    textview = tab_get_textview(-1);

    place_cursor(
      textview,
      textbuffer,
      &start
    );
}

void menu_undo(void){
    if(get_notebook_no_pages()){
        return;
    }

    GtkTextBuffer *undobuffer;
    GtkTextIter undoend;
    GtkTextIter undostart;

    undobuffer = tab_get_undobuffer(-1);

    gtk_text_buffer_get_start_iter(
      undobuffer,
      &undostart
    );
    gtk_text_buffer_get_iter_at_line(
      undobuffer,
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
    GtkTextBuffer *mapbuffer;
    GtkTextBuffer *redobuffer;
    GtkTextBuffer *textbuffer;
    GtkTextIter mapstart;
    GtkTextIter redostart;
    GtkTextIter selectstart;
    GtkTextView *textview;

    mapbuffer = tab_get_mapbuffer(-1);
    redobuffer = tab_get_redobuffer(-1);
    textbuffer = tab_get_textbuffer(-1);
    textview = tab_get_textview(-1);

    gtk_text_buffer_get_start_iter(
      redobuffer,
      &redostart
    );

    entry = gtk_text_buffer_get_text(
      undobuffer,
      &undostart,
      &undoend,
      FALSE
    );

    gtk_text_buffer_insert(
      redobuffer,
      &redostart,
      entry,
      -1
    );
    gtk_text_buffer_delete(
      undobuffer,
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
    block_insertdelete_signals(textbuffer);
    gtk_text_buffer_get_iter_at_line_offset(
      textbuffer,
      &selectstart,
      line,
      lineoffset
    );

    gtk_text_buffer_get_iter_at_offset(
      mapbuffer,
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
          mapbuffer,
          &mapend,
          gtk_text_iter_get_offset(&selectend)
        );

        gtk_text_buffer_delete(
          textbuffer,
          &selectstart,
          &selectend
        );
        gtk_text_buffer_delete(
          mapbuffer,
          &mapstart,
          &mapend
        );

    }else{
        gtk_text_buffer_insert(
          textbuffer,
          &selectstart,
          value,
          -1
        );
        gtk_text_buffer_insert(
          mapbuffer,
          &mapstart,
          value,
          -1
        );
    }
    unblock_insertdelete_signals(textbuffer);

    place_cursor(
      textview,
      textbuffer,
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
    gtk_text_view_set_monospace(
      GTK_TEXT_VIEW(text_view),
      TRUE
    );
    gtk_text_view_set_wrap_mode(
      GTK_TEXT_VIEW(text_view),
      GTK_WRAP_WORD
    );
    gtk_text_buffer_create_tag(
      buffer,
      TAG_FOUND,
      TAG_FOUND_PROPERTY,
      TAG_FOUND_COLOR,
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

        int page = gtk_notebook_get_current_page(notebook);
        GtkTextBuffer *mapbuffer;
        GtkTextBuffer *textbuffer;
        GtkTextView *textview;

        mapbuffer = tab_get_mapbuffer(page);
        textbuffer = tab_get_textbuffer(page);
        textview = tab_get_textview(page);

        gtk_notebook_set_tab_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            page
          ),
          g_path_get_basename(filename)
        );
        gtk_notebook_set_menu_label_text(
          notebook,
          gtk_notebook_get_nth_page(
            notebook,
            page
          ),
          filename
        );
        gtk_window_set_title(
          GTK_WINDOW(window),
          g_path_get_basename(filename)
        );

        block_insertdelete_signals(textbuffer);
        gtk_text_buffer_set_text(
          textbuffer,
          content,
          length
        );
        unblock_insertdelete_signals(textbuffer);

        GtkTextIter start;
        gtk_text_buffer_get_start_iter(
          textbuffer,
          &start
        );
        place_cursor(
          textview,
          textbuffer,
          &start
        );

        gtk_text_buffer_set_text(
          mapbuffer,
          content,
          -1
        );

        gtk_window_present(GTK_WINDOW(window));
    }

    g_free(content);
}

void place_cursor(GtkTextView *text_view, GtkTextBuffer *text_buffer, GtkTextIter *iter){
    gtk_text_buffer_place_cursor(
      text_buffer,
      iter
    );
    gtk_text_view_scroll_to_mark(
      text_view,
      gtk_text_buffer_get_insert(text_buffer),
      0,
      TRUE,
      0.5,
      0.5
    );
}

void save_tab(const gchar *filename){
    gchar *content;
    GtkTextBuffer *textbuffer;
    GtkTextIter checkend;
    GtkTextIter checkstart;
    GtkTextIter end;
    GtkTextIter start;

    textbuffer = tab_get_textbuffer(-1);

    // Trim line endings.
    int linei = gtk_text_buffer_get_line_count(textbuffer);
    while(linei >= 0){
        linei--;
        gtk_text_buffer_get_iter_at_line(
          textbuffer,
          &checkend,
          linei
        );
        gtk_text_iter_forward_to_line_end(&checkend);
        checkstart = checkend;
        if(gtk_text_iter_backward_char(&checkstart)){
            gchar *checked;
            checked = gtk_text_buffer_get_text(
              textbuffer,
              &checkstart,
              &checkend,
              FALSE
            );
            if(checked[0] == ' '){
                gboolean forward = TRUE;
                while(checked[0] == ' '){
                    if(gtk_text_iter_backward_char(&checkstart)){
                        checked = gtk_text_buffer_get_text(
                          textbuffer,
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
                  textbuffer,
                  &checkstart,
                  &checkend
                );
            }
            g_free(checked);
        }
    }

    gtk_text_buffer_get_end_iter(
      textbuffer,
      &end
    );
    // Make sure file ends with newline.
    checkstart = end;
    if(gtk_text_iter_backward_char(&checkstart)){
        gchar *lastchar;
        lastchar = gtk_text_buffer_get_text(
          textbuffer,
          &checkstart,
          &end,
          FALSE
        );
        if(lastchar[0] != '\n'){
            gtk_text_buffer_insert(
              textbuffer,
              &end,
              "\n",
              1
            );

            gtk_text_buffer_get_end_iter(
              textbuffer,
              &end
            );
        }
        g_free(lastchar);
    }
    gtk_text_buffer_get_start_iter(
      textbuffer,
      &start
    );

    content = gtk_text_buffer_get_text(
      textbuffer,
      &start,
      &end,
      FALSE
    );
    FILE *file;
    file = fopen(
      filename,
      "w"
    );
    fprintf(
      file,
      "%s",
      content
    );
    fclose(file);
    g_free(content);
}

void scroll_changed_map(void){
    GtkAdjustment *mapadjustment;
    GtkAdjustment *textadjustment;

    mapadjustment = tab_get_mapadjustment(-1);
    textadjustment = tab_get_textadjustment(-1);

    g_signal_handlers_block_by_func(
      textadjustment,
      G_CALLBACK(scroll_changed_textview),
      NULL
    );
    gtk_adjustment_set_value(
      textadjustment,
      (gtk_adjustment_get_upper(textadjustment) - gtk_adjustment_get_page_size(textadjustment))
        * (gtk_adjustment_get_value(mapadjustment) / (gtk_adjustment_get_upper(mapadjustment) - gtk_adjustment_get_page_size(mapadjustment)))
    );
    g_signal_handlers_unblock_by_func(
      textadjustment,
      G_CALLBACK(scroll_changed_textview),
      NULL
    );
}

void scroll_changed_textview(void){
    GtkAdjustment *mapadjustment;
    GtkAdjustment *textadjustment;

    mapadjustment = tab_get_mapadjustment(-1);
    textadjustment = tab_get_textadjustment(-1);

    g_signal_handlers_block_by_func(
      mapadjustment,
      G_CALLBACK(scroll_changed_map),
      NULL
    );
    gtk_adjustment_set_value(
      mapadjustment,
      (gtk_adjustment_get_upper(mapadjustment) - gtk_adjustment_get_page_size(mapadjustment))
        * (gtk_adjustment_get_value(textadjustment) / (gtk_adjustment_get_upper(textadjustment) - gtk_adjustment_get_page_size(textadjustment)))
    );
    g_signal_handlers_unblock_by_func(
      mapadjustment,
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
      TITLE
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
    g_signal_connect_after(
      notebook,
      "switch-page",
      G_CALLBACK(switch_page),
      NULL
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
    menuitem_file = gtk_menu_item_new_with_mnemonic(LABEL_FILE);
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_file),
      menu_file
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_NEWTAB,
      accelgroup,
      KEY_NEWTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_newtab),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_OPEN,
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
      LABEL_SAVE,
      accelgroup,
      KEY_SAVE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_save),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_SAVEAS,
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
      LABEL_NEXTTAB,
      accelgroup,
      KEY_NEXTTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(gtk_notebook_next_page),
      GTK_WIDGET(notebook)
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_PREVIOUSTAB,
      accelgroup,
      KEY_PREVIOUSTAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(gtk_notebook_prev_page),
      GTK_WIDGET(notebook)
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_MOVETABLEFT,
      accelgroup,
      KEY_MOVETABLEFT,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_movetab),
      (gpointer)-1
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_MOVETABRIGHT,
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
      LABEL_CLOSETAB,
      accelgroup,
      KEY_CLOSETAB,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_closetab),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_HIDEWINDOWS,
      accelgroup,
      KEY_HIDEWINDOWS,
      0,
      G_CALLBACK(menu_hide),
      NULL
    );
    gtk_add_menuitem(
      menu_file,
      LABEL_QUIT,
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
    menuitem_edit = gtk_menu_item_new_with_mnemonic(LABEL_EDIT);
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_edit),
      menu_edit
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_UNDO,
      accelgroup,
      KEY_UNDO,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_undo),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_REDO,
      accelgroup,
      KEY_UNDO,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_redo),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_CLEARUNDOREDO,
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
      LABEL_COPY,
      accelgroup,
      KEY_COPY,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_CUT,
      accelgroup,
      KEY_CUT,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_PASTE,
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
      LABEL_DELETE,
      accelgroup,
      KEY_DELETE,
      0,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_DELETELINE,
      accelgroup,
      KEY_DELETELINE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_deleteline),
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_DELETENEXTWORD,
      accelgroup,
      KEY_DELETE,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_DELETEPREVIOUSWORD,
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
      LABEL_EMOJI,
      accelgroup,
      KEY_INSERT_EMOJI,
      GDK_CONTROL_MASK,
      NULL,
      NULL
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_TOGGLEOVERWRITE,
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
      LABEL_SELECTALL,
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
      LABEL_SORTASCENDING,
      accelgroup,
      KEY_SORT,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_sort),
      (gpointer)TRUE
    );
    gtk_add_menuitem(
      menu_edit,
      LABEL_SORTDESCENDING,
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
    menuitem_find = gtk_menu_item_new_with_mnemonic(LABEL_FIND);
    gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(menuitem_find),
      menu_find
    );
    gtk_add_menuitem(
      menu_find,
      LABEL_FINDNEXT,
      accelgroup,
      KEY_FINDMORE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findnext),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      LABEL_FINDPREVIOUS,
      accelgroup,
      KEY_FINDMORE,
      GDK_CONTROL_MASK | GDK_SHIFT_MASK,
      G_CALLBACK(menu_findprevious),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      LABEL_FINDREPLACE,
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
      LABEL_REPACEALL,
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
      LABEL_GOTOTOP,
      accelgroup,
      KEY_TOP,
      0,
      G_CALLBACK(menu_findtop),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      LABEL_GOTOLINE,
      accelgroup,
      KEY_LINE,
      GDK_CONTROL_MASK,
      G_CALLBACK(menu_findline),
      NULL
    );
    gtk_add_menuitem(
      menu_find,
      LABEL_GOTOBOTTOM,
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
      LABEL_FINDREPLACEDIALOG
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
      LABEL_GOTOLINEDIALOG
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

void switch_page(void){
    gtk_window_set_title(
      GTK_WINDOW(window),
      get_current_tab_label_text()
    );
}

GtkAdjustment* tab_get_mapadjustment(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *sidebar;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    sidebar = g_list_nth_data(
      children,
      1
    );

    return gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(GTK_BIN(gtk_paned_get_child1(GTK_PANED(sidebar)))));
}

GtkTextBuffer* tab_get_mapbuffer(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *sidebar;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    sidebar = g_list_nth_data(
      children,
      1
    );

    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_paned_get_child1(GTK_PANED(sidebar))))));
}

GtkTextBuffer* tab_get_redobuffer(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *sidebar;
    GtkWidget *undoredo;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    sidebar = g_list_nth_data(
      children,
      1
    );
    undoredo = gtk_paned_get_child2(GTK_PANED(sidebar));

    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_paned_get_child2(GTK_PANED(undoredo))))));
}

GtkAdjustment* tab_get_textadjustment(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GList *children = get_tabbox_children(
      notebook,
      page
    );

    return gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(g_list_nth_data(children, 0)));
}

GtkTextBuffer* tab_get_textbuffer(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkWidget *text_view;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    text_view = gtk_bin_get_child(GTK_BIN(g_list_nth_data(
      children,
      0
    )));

    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
}

GtkTextView* tab_get_textview(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GList *children = get_tabbox_children(
      notebook,
      page
    );

    return GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(g_list_nth_data(children, 0))));
}

GtkTextBuffer* tab_get_undobuffer(int page){
    if(page < 0){
        page = gtk_notebook_get_current_page(notebook);
    }

    GtkNotebook *sidebar;
    GtkWidget *undoredo;

    GList *children = get_tabbox_children(
      notebook,
      page
    );
    sidebar = g_list_nth_data(
      children,
      1
    );
    undoredo = gtk_paned_get_child2(GTK_PANED(sidebar));

    return gtk_text_view_get_buffer(GTK_TEXT_VIEW(gtk_bin_get_child(GTK_BIN(gtk_paned_get_child1(GTK_PANED(undoredo))))));
}

void text_deleted(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end){
    GtkTextBuffer *mapbuffer;
    GtkTextBuffer *redobuffer;
    GtkTextBuffer *textbuffer;
    GtkTextBuffer *undobuffer;
    GtkTextIter first;
    GtkTextIter mapend;
    GtkTextIter mapstart;

    mapbuffer = tab_get_mapbuffer(-1);
    redobuffer = tab_get_redobuffer(-1);
    textbuffer = tab_get_textbuffer(-1);
    undobuffer = tab_get_undobuffer(-1);

    gtk_text_buffer_get_start_iter(
      undobuffer,
      &first
    );

    char *entry = undoredo_entry(
      gtk_text_buffer_get_text(
        textbuffer,
        start,
        end,
        FALSE
      ),
      FALSE,
      gtk_text_iter_get_line(start),
      gtk_text_iter_get_line_offset(start)
    );
    gtk_text_buffer_insert(
      undobuffer,
      &first,
      entry,
      -1
    );
    g_free(entry);

    gtk_text_buffer_get_iter_at_offset(
      mapbuffer,
      &mapstart,
      gtk_text_iter_get_offset(start)
    );
    gtk_text_buffer_get_iter_at_offset(
      mapbuffer,
      &mapend,
      gtk_text_iter_get_offset(end)
    );
    gtk_text_buffer_delete(
      mapbuffer,
      &mapstart,
      &mapend
    );

    menu_refind();

    gtk_text_buffer_set_text(
      redobuffer,
      "",
      0
    );
}

void text_inserted(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *value){
    GtkTextBuffer *mapbuffer;
    GtkTextBuffer *redobuffer;
    GtkTextBuffer *textbuffer;
    GtkTextBuffer *undobuffer;
    GtkTextIter first;
    GtkTextIter mapiter;

    mapbuffer = tab_get_mapbuffer(-1);
    redobuffer = tab_get_redobuffer(-1);
    textbuffer = tab_get_textbuffer(-1);
    undobuffer = tab_get_undobuffer(-1);

    gtk_text_buffer_get_start_iter(
      undobuffer,
      &first
    );

    char *entry = undoredo_entry(
      value,
      TRUE,
      gtk_text_iter_get_line(iter),
      gtk_text_iter_get_line_offset(iter)
    );
    gtk_text_buffer_insert(
      undobuffer,
      &first,
      entry,
      -1
    );
    g_free(entry);

    gtk_text_buffer_get_iter_at_offset(
      mapbuffer,
      &mapiter,
      gtk_text_iter_get_offset(iter)
    );
    gtk_text_buffer_insert(
      mapbuffer,
      &mapiter,
      value,
      -1
    );

    menu_refind();

    gtk_text_buffer_set_text(
      redobuffer,
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
          gtk_notebook_get_menu_label_text(
            notebook,
            gtk_notebook_get_nth_page(
              notebook,
              page++
            )
          ),
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
    FILE *file;
    file = fopen(
      path,
      "w"
    );
    fprintf(
      file,
      "%s",
      content
    );
    fclose(file);
    g_free(content);
    g_free(path);

    gtk_widget_destroy(temp_text_view);
    g_object_ref_sink(temp_text_view);
}
