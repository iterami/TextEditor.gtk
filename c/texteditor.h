#define KEY_BOTTOM GDK_KEY_End
#define KEY_CLOSETAB GDK_KEY_w
#define KEY_COPY GDK_KEY_c
#define KEY_CUT GDK_KEY_x
#define KEY_DELETE GDK_KEY_Delete
#define KEY_DELETELINE GDK_KEY_d
#define KEY_DELETEPREVIOUSWORD GDK_KEY_BackSpace
#define KEY_FIND GDK_KEY_f
#define KEY_FINDMORE GDK_KEY_g
#define KEY_HIDEWINDOWS GDK_KEY_Escape
#define KEY_LINE GDK_KEY_l
#define KEY_NEWTAB GDK_KEY_t
#define KEY_OPEN GDK_KEY_o
#define KEY_PASTE GDK_KEY_v
#define KEY_REPLACE GDK_KEY_h
#define KEY_SAVE GDK_KEY_s
#define KEY_SELECTALL GDK_KEY_a
#define KEY_SORT GDK_KEY_r
#define KEY_TOP GDK_KEY_Home
#define KEY_UNDO GDK_KEY_z
#define KEY_UNDOCLEAR GDK_KEY_y

static gchar *finding = NULL;
static GtkNotebook *notebook;
static GtkWidget *find_window;
static GtkWidget *find_window_find;
static GtkWidget *find_window_replace;
static GtkWidget *line_window;
static GtkWidget *line_window_line;
static GtkWidget *window;

typedef struct tabcontents{
  int page;
  GtkWidget *text_view;
  GtkTextBuffer *text_buffer;
  GtkTextBuffer *undo_buffer;
  GtkTextBuffer *redo_buffer;
  GtkTextBuffer *map_buffer;
} tabcontents;

static void activate(GtkApplication* app, gpointer data);
static void block_insertdelete_signals(GtkTextBuffer *text_buffer);
static gboolean check_equals_unsaved(void);
static void close_tab(void);
static void find_clear_tags(void);
static void find_close(void);
static const gchar* get_current_tab_label_text(void);
static gchar* get_find_find(void);
static gboolean get_notebook_no_pages(void);
static GList* get_tabbox_children(GtkNotebook *tabnotebook, gint page);
static struct tabcontents get_tab_contents(gint page);
static void go_to_line(void);
int main(int argc, char **argv);
static void menu_clearundoredo(void);
static void menu_deleteline(void);
static void menu_find(void);
static void menu_findbottom(void);
static void menu_findline(void);
static void menu_findnext(void);
static void menu_findprevious(void);
static void menu_find_recursive(GtkTextBuffer *buffer, GtkTextIter start);
static void menu_findreplaceall(void);
static void menu_findtop(void);
static void menu_hide(void);
static void menu_open(void);
static void menu_redo(void);
static void menu_refind(void);
static void menu_save(void);
static void menu_saveas(void);
static void menu_undo(void);
static GtkWidget* new_scrolled_window(void);
static void new_tab(void);
static GtkWidget* new_textview(gboolean map);
static void open_file(char *filename);
static void place_cursor(GtkWidget *text_view, GtkTextBuffer *text_buffer, GtkTextIter *iter);
static void save_tab(const char *filename);
static void text_deleted(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end);
static void text_inserted(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *value);
static void unblock_insertdelete_signals(GtkTextBuffer *text_buffer);
static gchar* undoredo_entry(gchar *value, gboolean inserted, gint line, gint lineoffset);
static void update_map(void);
static void update_opened_files(void);
