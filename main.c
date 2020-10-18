/* Largerly taken from https://developer.gnome.org/goocanvas/stable/goocanvas-simple-canvas.html */

#include <stdlib.h>
#include <goocanvas.h>

/* global variables */
static gint FPS = 100;
static gint WIDTH = 640;
static gint HEIGHT = 600;

static char *COLORS[] = {"#ff0000", "#0000ff", "#ffff00",
                        "#00ff00", "#ff00ff", "#00ffff"};


struct timeout_args {
    GooCanvasItem   *root;
    GooCanvasItem   *last;
};

struct circle_list {
    struct circle_list *next;
    GooCanvasItem *circ;
    gint x;
    gint y;
    gint vx;
    gint vy;
    gint score;
    gint skip;
};


/* containers */
static GtkWidget *window, *canvas;
static GooCanvasItem *root;
static int score = 0;
static struct circle_list *circles = NULL;


static gboolean on_button_press (GooCanvasItem  *view,
                                      GooCanvasItem  *target,
                                      GdkEventButton *event,
                                      gpointer        data);

static gboolean on_delete_event      (GtkWidget *window,
                                      GdkEvent  *event,
                                      gpointer   unused_data);

static gboolean on_timeout           (gpointer data);

static gboolean on_next_frame        (gpointer unused_data);

static gint     randint              (gint a,
                                      gint b);


int init(char *name) {
    int argc = 1;
    char **argv = &name;

    /* Initialize GTK+. */
    gtk_set_locale ();
    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_widget_show (window);
    g_signal_connect (window, "delete_event", (GtkSignalFunc) on_delete_event,
                      NULL);

    canvas = goo_canvas_new ();
    goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, WIDTH, HEIGHT);
    gtk_widget_show (canvas);
    gtk_container_add (GTK_CONTAINER (window), canvas);

    root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

    return 0;
}

int add_circle(gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
    struct circle_list *new_circ = malloc(sizeof(struct circle_list));
    if (!new_circ) {
        g_print("OOM: dropping\n");
        abort();
    }
    new_circ->next = circles;
    new_circ->score = score;
    new_circ->skip = 0;
    new_circ->x = x;
    new_circ->y = y;
    new_circ->vx = vx;
    new_circ->vy = vy;
    new_circ->circ = goo_canvas_ellipse_new(root, x, y, r, r, "line-width", 0.0, "fill-color", color, NULL);
    g_signal_connect (new_circ->circ, "button_press_event",
                      (GtkSignalFunc) on_button_press, new_circ);
    circles = new_circ;
    return 0;
}

int add_rect(gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
    struct circle_list *new_circ = malloc(sizeof(struct circle_list));
    if (!new_circ) {
        g_print("OOM: dropping\n");
        abort();
    }
    new_circ->next = circles;
    new_circ->score = score;
    new_circ->skip = 0;
    new_circ->x = x;
    new_circ->y = y;
    new_circ->vx = vx;
    new_circ->vy = vy;
    new_circ->circ = goo_canvas_rect_new(root, x, y, r, r, "line-width", 0.0, "fill-color", color, NULL);
    g_signal_connect (new_circ->circ, "button_press_event",
                      (GtkSignalFunc) on_button_press, new_circ);
    circles = new_circ;
    return 0;
}

int pop_circle() {
    goo_canvas_item_remove(circles->circ);
    gpointer tmp = circles;
    circles = circles->next;
    free(tmp);
    return 0;
}

int mainloop() {
    g_timeout_add (FPS, &on_next_frame, NULL);
    gtk_main();
    return 0;
}

int
main (int argc, char *argv[])
{
    GooCanvasItem *circ_item;

    /* Initialize GTK+. */
    gtk_set_locale ();
    gtk_init (&argc, &argv);

    /* Create the window and widgets. */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_widget_show (window);
    g_signal_connect (window, "delete_event", (GtkSignalFunc) on_delete_event,
                      NULL);

    /*scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_win),
                                         GTK_SHADOW_IN);
    gtk_widget_show (scrolled_win);
    gtk_container_add (GTK_CONTAINER (window), scrolled_win);*/

    canvas = goo_canvas_new ();
    /*gtk_widget_set_size_request (canvas, 600, 450);*/
    goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, WIDTH, HEIGHT);
    gtk_widget_show (canvas);
    gtk_container_add (GTK_CONTAINER (window), canvas);

    root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

    /* Add a few simple items. */
    circ_item = goo_canvas_ellipse_new (root, 100, 100, 100, 100,
                                        "line-width", 0.0,
                                        "fill-color", "red",
                                        NULL);

    struct timeout_args *pArgs = malloc(sizeof(struct timeout_args));
    pArgs->root = root;
    pArgs->last = circ_item;

    /* Connect a signal handler for the rectangle item. */
    g_signal_connect (circ_item, "button_press_event",
                      (GtkSignalFunc) on_button_press, NULL);

    add_circle(100, 100, 50, 1, 10, "red", 10);

    /* g_timeout_add (FPS, &on_timeout, pArgs); */
    g_timeout_add (FPS, &on_next_frame, NULL);

    /* Pass control to the GTK+ main event loop. */
    gtk_main ();

    return 0;
}


/* This handles button presses in item views. We simply output a message to
     the console. */
static gboolean
on_button_press (GooCanvasItem    *item,
                      GooCanvasItem    *target,
                      GdkEventButton   *event,
                      gpointer          data)
{
    struct circle_list *cur = (struct circle_list *)data;
    score += cur->score;
    cur->score = 0;
    cur->skip = 1;
    goo_canvas_item_remove(cur->circ);
    return TRUE;
}


/* This is our handler for the "delete-event" signal of the window, which
     is emitted when the 'x' close button is clicked. We just exit here. */
static gboolean
on_delete_event (GtkWidget *window,
                 GdkEvent  *event,
                 gpointer   unused_data)
{
    g_print("Your score is %i.\n", score);
    exit (0);
}

/* This is handler for frames. All magic happens here */
static gboolean
on_timeout (gpointer data) {
    struct timeout_args *pArgs = (struct timeout_args *)data;

    goo_canvas_item_remove (pArgs->last);
    gint x, y, r;
    x = randint(0, WIDTH);
    y = randint(0, HEIGHT);
    r = randint(10, 100);
    pArgs->last = goo_canvas_ellipse_new (pArgs->root, x, y, r, r,
                                          "line-width", 0.0,
                                          "fill-color", COLORS[randint(0, 6)],
                                          NULL);
    g_signal_connect (pArgs->last, "button_press_event",
                      (GtkSignalFunc) on_button_press, NULL);

    return TRUE;
}

/* This is handler for next frame. All magic happens here. It uses global static vars. */
static gboolean
on_next_frame (gpointer unused_data) {
    struct circle_list *cur = circles;
    while (cur) {
        if (cur->skip) {
            cur = cur->next;
            continue;
        }
        cur->x += cur->vx;
        cur->y += cur->vy;
        goo_canvas_item_translate(cur->circ, (gdouble)cur->vx, (gdouble)cur->vy);
        if (cur->x < 0 || cur->x > WIDTH) {
            cur->vx *= -1;
        }
        if (cur->y < 0 || cur->y > HEIGHT) {
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
