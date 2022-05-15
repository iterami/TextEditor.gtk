#pragma once

#define CONFIG_PATH "config/texteditor.cfg"
#define KEY_BOTTOM GDK_KEY_End
#define KEY_CLOSETAB GDK_KEY_w
#define KEY_DELETE GDK_KEY_Delete
#define KEY_DELETELINE GDK_KEY_d
#define KEY_DELETEPREVIOUSWORD GDK_KEY_BackSpace
#define KEY_FIND GDK_KEY_f
#define KEY_FINDMORE GDK_KEY_g
#define KEY_HIDEWINDOWS GDK_KEY_Escape
#define KEY_INSERT GDK_KEY_Insert
#define KEY_LINE GDK_KEY_l
#define KEY_MOVETABLEFT GDK_KEY_underscore
#define KEY_MOVETABRIGHT GDK_KEY_plus
#define KEY_NEWTAB GDK_KEY_t
#define KEY_NEXTTAB GDK_KEY_equal
#define KEY_OPEN GDK_KEY_o
#define KEY_PREVIOUSTAB GDK_KEY_minus
#define KEY_REPLACE GDK_KEY_m
#define KEY_SAVE GDK_KEY_s
#define KEY_SELECTALL GDK_KEY_a
#define KEY_SORT GDK_KEY_r
#define KEY_TOP GDK_KEY_Home
#define KEY_UNDO GDK_KEY_z
#define KEY_UNDOCLEAR GDK_KEY_y
#define LABEL_CANCEL "_Cancel"
#define LABEL_CLEARUNDOREDO "Cl_ear Undo/Redo"
#define LABEL_CLOSETAB "_Close Tab"
#define LABEL_COPY "_Copy"
#define LABEL_CUT "Cu_t"
#define LABEL_DELETE "_Delete"
#define LABEL_DELETELINE "Delete _Line"
#define LABEL_DELETENEXTWORD "Delete Ne_xt Word"
#define LABEL_DELETEPREVIOUSWORD "Delete Pre_vious Word"
#define LABEL_EDIT "_Edit"
#define LABEL_EMOJI "Insert Emo_ji"
#define LABEL_FILE "_File"
#define LABEL_FIND "F_ind"
#define LABEL_FINDNEXT "Find _Next"
#define LABEL_FINDPREVIOUS "Find _Previous"
#define LABEL_FINDREPLACE "_Find/Replace..."
#define LABEL_FINDREPLACEDIALOG "Find/Replace..."
#define LABEL_GOTOBOTTOM "Go to _Bottom"
#define LABEL_GOTOLINE "Go to _Line..."
#define LABEL_GOTOLINEDIALOG "Go to Line..."
#define LABEL_GOTOTOP "Go to _Top"
#define LABEL_HIDEWINDOWS "_Hide Windows"
#define LABEL_MOVETABLEFT "Move Tab _Left"
#define LABEL_MOVETABRIGHT "Move Tab _Right"
#define LABEL_NEWTAB "_New Tab"
#define LABEL_NEXTTAB "Ne_xt Tab"
#define LABEL_OPEN "_Open..."
#define LABEL_OPENBUTTON "_Open"
#define LABEL_OPENDIALOG "Open..."
#define LABEL_PASTE "_Paste"
#define LABEL_PREVIOUSTAB "_Previous Tab"
#define LABEL_QUIT "_Quit"
#define LABEL_REDO "_Redo"
#define LABEL_REPACEALL "_Replace All"
#define LABEL_SAVE "_Save"
#define LABEL_SAVEAS "Save _As..."
#define LABEL_SAVEASBUTTON "_Save As"
#define LABEL_SAVEASDIALOG "Save As..."
#define LABEL_SELECTALL "_Select All"
#define LABEL_SORTASCENDING "Sort _Ascending"
#define LABEL_SORTDESCENDING "Sort Desce_nding"
#define LABEL_TOGGLEOVERWRITE "Toggle _Overwrite"
#define LABEL_UNDO "_Undo"
#define LABEL_UNSAVED "UNSAVED"
#define TAG_FOUND "found"
#define TAG_FOUND_COLOR "#650"
#define TAG_FOUND_PROPERTY "background"
#define TITLE "TextEditor.gtk"

gchar *finding = NULL;
GtkWidget *find_window;
GtkWidget *find_window_find;
GtkWidget *find_window_replace;
GtkWidget *line_window;
GtkWidget *line_window_line;
GtkNotebook *notebook;
gboolean refinding = TRUE;
gint width_sidebar = 100;

void block_insertdelete_signals(GtkTextBuffer *text_buffer);
gboolean check_equals_unsaved(void);
void find_clear_tags(void);
const gchar* get_current_tab_label_text(void);
gchar* get_find_find(void);
gboolean get_notebook_no_pages(void);
GList* get_tabbox_children(GtkNotebook *tabnotebook, const gint page);
void go_to_line(void);
int main(int argc, char **argv);
void menu_clearundoredo(void);
void menu_closetab(void);
void menu_deleteline(void);
void menu_findbottom(void);
void menu_findline(void);
void menu_findnext(void);
void menu_findprevious(void);
void menu_find_recursive(GtkTextBuffer *buffer, GtkTextIter start);
void menu_findreplace(void);
void menu_findreplaceall(void);
void menu_findtop(void);
void menu_hide(void);
void menu_movetab(gint movement);
void menu_newtab(void);
void menu_open(void);
void menu_redo(void);
void menu_refind(void);
void menu_save(void);
void menu_saveas(void);
void menu_sort(gboolean asc);
void menu_undo(void);
GtkWidget* new_scrolled_window(void);
GtkWidget* new_textview(gboolean editable, gchar *name);
void open_file(gchar *filename);
void place_cursor(GtkTextView *text_view, GtkTextBuffer *text_buffer, GtkTextIter *iter);
void save_tab(const gchar *filename);
void scroll_changed_map(void);
void scroll_changed_textview(void);
void startup(GtkApplication* app, gpointer data);
GtkAdjustment* tab_get_mapadjustment(int page);
GtkTextBuffer* tab_get_mapbuffer(int page);
GtkTextBuffer* tab_get_redobuffer(int page);
GtkAdjustment* tab_get_textadjustment(int page);
GtkTextBuffer* tab_get_textbuffer(int page);
GtkTextView* tab_get_textview(int page);
GtkTextBuffer* tab_get_undobuffer(int page);
void text_deleted(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end);
void text_inserted(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *value);
void unblock_insertdelete_signals(GtkTextBuffer *text_buffer);
gchar* undoredo_entry(gchar *value, const gboolean inserted, const int line, const int lineoffset);
void update_opened_files(void);
