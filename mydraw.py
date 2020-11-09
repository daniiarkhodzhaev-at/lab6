#!/usr/bin/python3

import ctypes

__mylib = ctypes.cdll.LoadLibrary("main.so")


def setup(name, width, height):
    """
    This function sets up app params
    @param name name of the app
    @param width width of the window
    @param height height of the window
    @return Exit code
    """
    return __mylib.setup(name.encode(), width, height);

def update_leaderboard():
    """
    This function updates leaderboard. Adds name and score to file.
    @return (gint) Exit code
    """
    return __mylib.update_leaderboard()

def init():
    """
    This function initializes lib.
    @return Exit code
    """
    return __mylib.init()

def add_circ(x, y, r, vx, vy, color, score):
    """
    This function adds circle.
    @param x x coord
    @param y y coord
    @param r radius
    @param vx x velocity
    @param vy y velocity
    @param color color
    @param score score increment, used once 
    @return Exit code
    """
    return __mylib.add_circ(x, y, r, vx, vy, color.encode(), score)

def add_rect(x, y, r, vx, vy, color, score):
    """
    This funrcion adds rectangular.
    @param x x coord
    @param y y coord
    @param r radius
    @param vx x velocity
    @param vy y velocity
    @param color color
    @param score score increment, used once
    @return Exit code
    """
    return __mylib.add_rect(x, y, r, vx, vy, color.encode(), score)

def pop_object():
    """
    This funtcion removes the last object from canvas.
    @return Exit code
    """
    return __mylib.pop_object()

def mainloop():
    """
    This is mainloop function. It passes control to GTK+ main event loop.
    @return Exit code
    """
    return __mylib.mainloop()


"""
Demo
"""
if (__name__ == "__main__"):
    setup("main", 640, 600)
    init()
    add_circ(100, 100, 50, 10, 10, "#ff0000", 10)
    add_circ(200, 200, 30, -10, 10, "#00ff00", 10)
    add_circ(300, 300, 40, 10, -10, "#0000ff", 10)
    add_circ(400, 400, 50, 1, 1, "#000000", 10)
    pop_object()
    add_rect(100, 100, 50, 10, 10, "#ff0000", -1)
    add_rect(200, 200, 30, -10, 10, "#00ff00", -1)
    add_rect(300, 300, 40, 10, -10, "#0000ff", -1)
    add_rect(400, 400, 50, 1, 1, "#000000", -1)
    pop_object()
    mainloop()
