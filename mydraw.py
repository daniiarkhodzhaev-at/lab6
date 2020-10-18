#!/usr/bin/python3

import ctypes

__mylib = ctypes.cdll.LoadLibrary("main.so")

def init(name):
    return __mylib.init(name.encode())

def add_circle(x, y, r, vx, vy, color, score):
    return __mylib.add_circle(x, y, r, vx, vy, color.encode(), score)

def add_rect(x, y, r, vx, vy, color, score):
    return __mylib.add_rect(x, y, r, vx, vy, color.encode(), score)

def pop_fig():
    return __mylib.pop_circle()

def mainloop():
    return __mylib.mainloop()

if (__name__ == "__main__"):
    init("main")
    add_circle(100, 100, 50, 10, 10, "#ff0000", 10)
    add_circle(200, 200, 30, -10, 10, "#00ff00", 10)
    add_circle(300, 300, 40, 10, -10, "#0000ff", 10)
    add_circle(400, 400, 50, 1, 1, "#000000", 10)
    pop_fig()
    add_rect(100, 100, 50, 10, 10, "#ff0000", -1)
    add_rect(200, 200, 30, -10, 10, "#00ff00", -1)
    add_rect(300, 300, 40, 10, -10, "#0000ff", -1)
    add_rect(400, 400, 50, 1, 1, "#000000", -1)
    pop_fig()
    mainloop()
