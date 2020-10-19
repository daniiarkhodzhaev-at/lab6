/* Largerly taken from https://developer.gnome.org/goocanvas/stable/goocanvas-simple-canvas.html */

#include <stdlib.h>
#include <goocanvas.h>

/* global variables */
char *NAME = "";
static gint FPS = 100;
static gint WIDTH = 640;
static gint HEIGHT = 600;

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
static GtkWidget *window, *canvas;
static GooCanvasItem *root;
static int score = 0;
static struct object_list *objects = NULL;


static gboolean on_button_press (GooCanvasItem  *view,
                                      GooCanvasItem  *target,
                                      GdkEventButton *event,
                                      gpointer        data);

static gboolean on_delete_event      (GtkWidget *window,
                                      GdkEvent  *event,
                                      gpointer   unused_data);

static gboolean on_next_frame        (gpointer unused_data);

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
int setup(char *name, gint width, gint height) {
    WIDTH = width;
    HEIGHT = height;
    NAME = malloc(strlen(name) + 1);
    memcpy(NAME, name, strlen(name) + 1);
    return 0;
}

/**
 * This is init function. It initialize GTK+. Uses values set by setup function.
 * @return (gint) Exit code
 */
int init() {
    int argc = 1;
    char **argv = &NAME;

    /* Initialize GTK+. */
    gtk_set_locale ();
    gtk_init (&argc, &argv);

    /* Create GTK+ window and bind closing callback. */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_widget_show (window);
    g_signal_connect (window, "delete_event", (GtkSignalFunc) on_delete_event,
                      NULL);

    /* Create new canvas, set it size and get root element for future usage.*/
    canvas = goo_canvas_new ();
    goo_canvas_set_bounds (GOO_CANVAS (canvas), 0, 0, WIDTH, HEIGHT);
    gtk_widget_show (canvas);
    gtk_container_add (GTK_CONTAINER (window), canvas);
    root = goo_canvas_get_root_item (GOO_CANVAS (canvas));

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
int add_circ(gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
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
int add_rect(gint x, gint y, gint r, gint vx, gint vy, char *color, gint score) {
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
int pop_object() {
    goo_canvas_item_remove(objects->obj);
    gpointer tmp = objects;
    objects = objects->next;
    free(tmp);
    return 0;
}

/**
 * This is mainllop function. It sets next frame callback
 * and passes control to GTK+ main event loop.
 * @return (gint) Exit code
 */
int mainloop() {
    /* Set next frame callback */
    g_timeout_add (1000.0 / FPS, &on_next_frame, NULL);

    /* Pass control to the GTK+ main event loop. */
    gtk_main();
    return 0;
}

/**
 * This is test function to demonstrate functionality.
 * @return (gint) Exit code
 */
int
main (int argc, char *argv[])
{
    init("Test");

    for (gint i = 0; i < 10; ++i) {
        gint x, y, r, vx, vy;
        r = randint(10, 100);
        x = randint(r, WIDTH - r);
        y = randint(r, HEIGHT - r);
        vx = randint(-10, 10);
        vy = randint(-10, 10);
        char *color = COLORS[randint(0, 6)];

        add_circ(x, y, r, vx, vy, color, 10);
    }

    for (gint i = 0; i < 10; ++i) {
        gint x, y, r, vx, vy;
        r = randint(10, 100);
        x = randint(r, WIDTH - r);
        y = randint(r, HEIGHT - r);
        vx = randint(-10, 10);
        vy = randint(-10, 10);
        char *color = COLORS[randint(0, 6)];

        add_rect(x, y, r, vx, vy, color, -1);
    }

    mainloop();

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
     is emitted when the 'x' close button is clicked. We just exit here. */
static gboolean
on_delete_event (GtkWidget *window,
                 GdkEvent  *event,
                 gpointer   unused_data)
{
    g_print("Your score is %i.\n", score);
    exit (0);
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
