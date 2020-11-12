/* Some code taken from https://developer.gnome.org/goocanvas/stable/goocanvas-simple-canvas.html */

#include <stdlib.h>
#include <goocanvas.h>

/* global constants */
char *NAME = "";
static gint FPS = 100;
static gint WIDTH = 640;
static gint HEIGHT = 600;
static const char *LEADERBOARD_FN = "leaderboard";
static const char *TMP_FN = ".tmp.leaderboard";

static char *COLORS[] = {"#ff0000", "#0000ff", "#ffff00",
                        "#00ff00", "#ff00ff", "#00ffff"};


struct object_list {
    struct object_list *next;
    GooCanvasItem *obj;
    gint x;
    gint y;
    gint r;
    gint vx;
    gint vy;
    gint score;
    gint skip;
};

/* containers */
static GtkWidget *window, *canvas, *entry_text;
static GooCanvasItem *root;
static int score = 0;
static char *username;
static struct object_list *objects = NULL;


static gboolean on_button_press      (GooCanvasItem  *view,
                                      GooCanvasItem  *target,
                                      GdkEventButton *event,
                                      gpointer        data);

static gboolean on_delete_event      (GtkWidget *window,
                                      GdkEvent  *event,
                                      gpointer   unused_data);

static gboolean on_configure_event   (GtkWidget *window,
                                      GdkEvent  *event,
                                      gpointer   unused_data);

static gboolean on_next_frame        (gpointer unused_data);

static gboolean on_username_entered  (GtkWidget *confirm_button,
                                      GdkEvent  *event,
                                      gpointer   pop_window);

static gint     randint              (gint a,
                                      gint b);

/**
 * This function sets up params.
 * @param name (char *) name of app
 * @param width (gint) width of the window
 * @param height (gint) height of the window
 *
 * @return (gint) Exit code
 */
int setup (char *name, gint width, gint height) {
    WIDTH = width;
    HEIGHT = height;
    NAME = malloc(strlen(name) + 1);
    memcpy (NAME, name, strlen(name) + 1);
    pid_t pid;
    pid = fork ();
    if (pid == 0) {
        execl ("/usr/bin/touch", "touch", LEADERBOARD_FN, NULL);
    }
    return 0;
}

/**
 * This function updates leaderboard. Adds name and score to file.
 *
 * @return (gint) Exit code
 */
int update_leaderboard () {
    FILE *fd_l = fopen (LEADERBOARD_FN, "r");
    FILE *fd_t = fopen (TMP_FN, "w");
    if (!fd_l) {
        g_print ("Failed to open stream\n");
        return -1;
    }
    if (!fd_t) {
        g_print ("Failed to open stream\n");
        return -1;
    }
    gboolean added = FALSE;
    gulong *pN = malloc (sizeof (gulong));
    gint *pScore = malloc (sizeof (gint));
    *pN = 0;
    ssize_t nread;
    gchar **pLine = malloc (sizeof (char *));
    *pLine = NULL;
    gchar *pName = malloc (256 * sizeof (char *));
    while ((nread = getline (pLine, pN, fd_l)) != -1) {
        sscanf (*pLine, "%s %i", pName, pScore);
        if (*pScore < score && (!added)) {
            fprintf (fd_t, "%s %i\n", username, score);
            added = TRUE;
        }
        fprintf (fd_t, "%s %i\n", pName, *pScore);
        free (*pLine);
        *pLine = NULL;
        *pN = 0;
    }
    if (!added) {
        fprintf (fd_t, "%s %i\n", username, score);
    }
    fclose (fd_l);
    fclose (fd_t);
    
    free (pN);
    free (pScore);

    pid_t pid;
    pid = fork ();
    if (pid == 0) {
        execl ("/usr/bin/mv", "mv", TMP_FN, LEADERBOARD_FN, NULL);
    }
    return 0;
}

/**
 * This is init function. It initialize GTK+. Uses values set by setup function.
 * Also it queries username via pop-up window.
 * @return (gint) Exit code
 */
int init () {
    int argc = 1;
    char **argv = &NAME;

    GtkWidget *hbox, *c_hbox, *vbox, *halign, *stopBtn, *showLeaderbordBtn,
              *pop_window, *entry_text_label, *confirm_button,
              *pop_vbox, *pop_hbox_entry, *pop_hbox_confirm,
              *pop_vbox_spacer_top, *pop_vbox_spacer_mid, *pop_vbox_spacer_bot,
              *pop_hbox_entry_spacer_lft, *pop_hbox_entry_spacer_rt,
              *pop_hbox_confirm_spacer_lft, *pop_hbox_confirm_spacer_rt;
    GtkEntryBuffer *username_buf;

    /* Initialize GTK+. */
    gtk_set_locale ();
    gtk_init (&argc, &argv);

    /* Create GTK+ window and bind closing callback. */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW (window), HEIGHT, WIDTH);
    gtk_window_set_resizable(GTK_WINDOW (window), FALSE);
    
    g_signal_connect (window, "delete_event", (GtkSignalFunc) on_delete_event,
                      NULL);

    vbox = gtk_vbox_new (FALSE, 5);
    hbox = gtk_hbox_new (TRUE, 5);
    c_hbox = gtk_hbox_new(FALSE, 0);
    halign = gtk_alignment_new (1, 0, 0, 0);

    /* Create new canvas, set it size and get root element for future usage.*/
    canvas = goo_canvas_new ();
    goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, WIDTH, HEIGHT);
    gtk_widget_set_size_request (canvas, WIDTH, HEIGHT);
    root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

    /* Add buttons and layout. */
    stopBtn = gtk_button_new_with_label ("Stop");
    /* showLeaderbordBtn = gtk_button_new_with_label ("Show leaderboard"); */
    showLeaderbordBtn = gtk_label_new ("");
    gtk_widget_set_size_request (stopBtn, 200, 40);

    g_signal_connect (stopBtn, "button_press_event",
                      (GtkSignalFunc) on_delete_event, NULL);

    gtk_container_add (GTK_CONTAINER (hbox), stopBtn);
    gtk_container_add (GTK_CONTAINER (hbox), showLeaderbordBtn);
    gtk_container_add (GTK_CONTAINER (halign), hbox);
    gtk_box_pack_start (GTK_BOX(vbox), halign, FALSE, FALSE, 0);
    gtk_container_add (GTK_CONTAINER (c_hbox), canvas);
    gtk_container_add (GTK_CONTAINER (vbox), c_hbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    /* gtk_widget_show_all (window); */


    /* Add dialog for acquiring username */
    pop_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_deletable (GTK_WINDOW (pop_window), FALSE);
    gtk_window_set_resizable (GTK_WINDOW (pop_window), FALSE);

    pop_vbox = gtk_vbox_new (TRUE, 0);
    pop_hbox_entry = gtk_hbox_new (TRUE, 0);
    pop_hbox_confirm = gtk_hbox_new (TRUE, 0);

    pop_vbox_spacer_top = gtk_label_new ("");
    pop_vbox_spacer_mid = gtk_label_new ("");
    pop_vbox_spacer_bot = gtk_label_new ("");

    pop_hbox_entry_spacer_lft = gtk_label_new ("");
    pop_hbox_entry_spacer_rt = gtk_label_new ("");

    pop_hbox_confirm_spacer_lft = gtk_label_new ("");
    pop_hbox_confirm_spacer_rt = gtk_label_new ("");

    entry_text_label = gtk_label_new ("Enter your name:");
    username_buf = gtk_entry_buffer_new (NULL, -1);
    entry_text = gtk_entry_new_with_buffer (username_buf);
    gtk_editable_set_editable (GTK_EDITABLE (entry_text), TRUE);
    
    confirm_button = gtk_button_new_with_label ("Start!");

    gtk_container_add (GTK_CONTAINER (pop_hbox_entry), pop_hbox_entry_spacer_lft);
    gtk_container_add (GTK_CONTAINER (pop_hbox_entry), entry_text);
    gtk_container_add (GTK_CONTAINER (pop_hbox_entry), pop_hbox_entry_spacer_rt);

    gtk_container_add (GTK_CONTAINER (pop_hbox_confirm), pop_hbox_confirm_spacer_lft);
    gtk_container_add (GTK_CONTAINER (pop_hbox_confirm), confirm_button);
    gtk_container_add (GTK_CONTAINER (pop_hbox_confirm), pop_hbox_confirm_spacer_rt);

    gtk_container_add (GTK_CONTAINER (pop_vbox), pop_vbox_spacer_top);
    gtk_container_add (GTK_CONTAINER (pop_vbox), entry_text_label);
    gtk_container_add (GTK_CONTAINER (pop_vbox), pop_hbox_entry);
    gtk_container_add (GTK_CONTAINER (pop_vbox), pop_vbox_spacer_mid);
    gtk_container_add (GTK_CONTAINER (pop_vbox), pop_hbox_confirm);
    gtk_container_add (GTK_CONTAINER (pop_vbox), pop_vbox_spacer_bot);

    gtk_container_add (GTK_CONTAINER (pop_window), pop_vbox);

    g_signal_connect (confirm_button, "button_press_event",
                      (GtkSignalFunc) on_username_entered, pop_window);

    gtk_widget_show_all (pop_window);

    return 0;
}

/**
 * This function adds circle with given params to canvas and sets onclick callback.
 * @param x (gint) x coord
 * @param y (gint) y coord
 * @param r (gint) radius
 * @param vx (gint) x velocity
 * @param vy (gint) y velocity
 * @param color (char *) color in string format
 * @param score (gint) score increment, used once
 *
 * @return (gint) Exit code
 */
int add_circ (gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
    struct object_list *new_obj = malloc(sizeof(struct object_list));
    if (!new_obj) {
        g_print("OOM: dropping\n");
        abort();
    }
    new_obj->next = objects;
    new_obj->score = score;
    new_obj->skip = 0;
    new_obj->x = x;
    new_obj->y = y;
    new_obj->r = r;
    new_obj->vx = vx;
    new_obj->vy = vy;
    new_obj->obj = goo_canvas_ellipse_new(root, x, y, r, r, "line-width", 0.0, "fill-color", color, NULL);
    g_signal_connect (new_obj->obj, "button_press_event",
                      (GtkSignalFunc) on_button_press, new_obj);
    objects = new_obj;
    return 0;
}

/**
 * This function adds rectangle with given params to canvas and sets onclick callback.
 * @param x (gint) x coord
 * @param y (gint) y coord
 * @param r (gint) radius
 * @param vx (gint) x velocity
 * @param vy (gint) y velocity
 * @param color (char *) color in string format
 * @param score (gint) score increment, used once
 *
 * @return (gint) Exit code
 */
int add_rect (gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
    struct object_list *new_obj = malloc(sizeof(struct object_list));
    if (!new_obj) {
        g_print("OOM: dropping\n");
        abort();
    }
    new_obj->next = objects;
    new_obj->score = score;
    new_obj->skip = 0;
    new_obj->x = x;
    new_obj->y = y;
    new_obj->r = r;
    new_obj->vx = vx;
    new_obj->vy = vy;
    new_obj->obj = goo_canvas_rect_new(root, x, y, r, r, "line-width", 0.0, "fill-color", color, NULL);
    g_signal_connect (new_obj->obj, "button_press_event",
                      (GtkSignalFunc) on_button_press, new_obj);
    objects = new_obj;
    return 0;
}

/**
 * This function removes the last added element from canvas.
 * @return (gint) Exit code
 */
int pop_object () {
    goo_canvas_item_remove (objects->obj);
    gpointer tmp = objects;
    objects = objects->next;
    free (tmp);
    return 0;
}

/**
 * This is mainllop function. It sets next frame callback
 * and passes control to GTK+ main event loop.
 * @return (gint) Exit code
 */
int mainloop () {
    /* Set next frame callback */
    g_timeout_add (1000.0 / FPS, &on_next_frame, NULL);

    /* Pass control to the GTK+ main event loop. */
    gtk_main ();
    return 0;
}

/**
 * This is test function to demonstrate functionality.
 * @return (gint) Exit code
 */
int
main (int argc, char *argv[])
{
    setup ("Test", WIDTH, HEIGHT);

    init ();

    for (gint i = 0; i < 10; ++i) {
        gint x, y, r, vx, vy;
        r = randint (10, 100);
        x = randint (r, WIDTH - r);
        y = randint (r, HEIGHT - r);
        vx = randint (-10, 10);
        vy = randint (-10, 10);
        char *color = COLORS [randint (0, 6)];

        add_circ (x, y, r, vx, vy, color, 10);
    }

    for (gint i = 0; i < 10; ++i) {
        gint x, y, r, vx, vy;
        r = randint (10, 100);
        x = randint (r, WIDTH - r);
        y = randint (r, HEIGHT - r);
        vx = randint (-10, 10);
        vy = randint (-10, 10);
        char *color = COLORS [randint (0, 6)];

        add_rect (x, y, r, vx, vy, color, -1);
    }

    mainloop ();

    g_print ("Your score is %i.\n", score);

    update_leaderboard ();

    return 0;
}


/* This handles button presses in item views. */
static gboolean
on_button_press (GooCanvasItem    *item,
                      GooCanvasItem    *target,
                      GdkEventButton   *event,
                      gpointer          data)
{
    struct object_list *cur = (struct object_list *)data;
    score += cur->score;
    cur->score = 0;
    cur->skip = 1;
    goo_canvas_item_remove(cur->obj);
    return TRUE;
}

/* This is our handler for the "delete-event" signal of the window, which
     is emitted when the 'x' close button is clicked. We return controll for
     mainloop initiator here. */
static gboolean
on_delete_event (GtkWidget *window,
                 GdkEvent  *event,
                 gpointer   unused_data)
{
    gtk_main_quit();
    return FALSE;
}

/* This is our handler for the "configure-event" signal of the window, which
     is emitted (not only) when window is resized. We adjust canvas and working
     zone for new dimentions. */
static gboolean
on_configure_event (GtkWidget *window,
                    GdkEvent  *event,
                    gpointer   unused_data)
{
    GtkAllocation *canvas_alloc;

    canvas_alloc = malloc (sizeof (GtkAllocation));
    /* gtk_widget_get_allocation (gtk_widget_get_parent (canvas), canvas_alloc);
    WIDTH = canvas_alloc->width;
    HEIGHT = canvas_alloc->height; */
    gtk_window_get_size (GTK_WINDOW (window), &WIDTH, &HEIGHT);
    WIDTH = 1000;
    HEIGHT = 1000;

    goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, WIDTH, HEIGHT);
    free (canvas_alloc);

    struct object_list *cur = objects;
    while (cur) {
        if (cur->skip) {
            cur = cur->next;
            continue;
        }
        gint old_x, old_y;
        old_x = cur->x;
        old_y = cur->y;
        if (cur->x < cur->r) {
            cur->x = cur->r;
        }
        if (cur->x > WIDTH - cur->r) {
            cur->x = WIDTH - cur->r;
        }
        if (cur->y < cur->r) {
            cur->y = cur->r;
        }
        if (cur->y > HEIGHT - cur->r) {
            cur->y = HEIGHT - cur->r;
        }
        goo_canvas_item_translate(cur->obj, (gdouble)(cur->x - old_x), (gdouble)(cur->y - old_y));
        cur = cur->next;
    }

    return TRUE;
}

static gboolean
on_username_entered (GtkWidget *confirm_button,
                     GdkEvent *event,
                     gpointer  pop_window)
{
    int _len = gtk_entry_get_text_length (GTK_ENTRY (entry_text));
    username = realloc (username, (_len + 1) * sizeof (char));
    memcpy (username, gtk_entry_get_text (GTK_ENTRY (entry_text)), (_len));
    *(username + _len) = 0;
    gtk_widget_destroy (GTK_WIDGET (pop_window));
    gtk_widget_show_all (window);

    return TRUE;
}

/* This is handler for next frame. All magic happens here. It uses global static vars. */
static gboolean
on_next_frame (gpointer unused_data) {
    struct object_list *cur = objects;
    while (cur) {
        if (cur->skip) {
            cur = cur->next;
            continue;
        }
        cur->x += cur->vx;
        cur->y += cur->vy;
        goo_canvas_item_translate(cur->obj, (gdouble)cur->vx, (gdouble)cur->vy);
        if (cur->x < cur->r || cur->x > WIDTH - cur->r) {
            cur->vx *= -1;
        }
        if (cur->y < cur->r || cur->y > HEIGHT - cur->r) {
            cur->vy *= -1;
        }
        cur = cur->next;
    }
    return TRUE;
}

/* This returns random integer from [a, b) */
static gint
randint (gint a, gint b) {
    return a + (gint)(random() * (glong)(b - a) / RAND_MAX);
}
