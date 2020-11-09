#!/usr/bin/python3

from mydraw import *
from random import randint

WIDTH = 1200
HEIGHT = 800
COLORS = ["#ff0000", "#0000ff", "#ffff00",
          "#00ff00", "#ff00ff", "#00ffff"];

def add_rand_circ() -> int:
    """
    This function creates circle in radom point with random params
    @return Exit code
    """
    r = randint(10, 100)
    x = randint(r, WIDTH - r)
    y = randint(r, HEIGHT - r)
    vx = randint(-10, 10)
    vy = randint(-10, 10)
    color = COLORS[randint(0, len(COLORS) - 1)]
    add_circ(x, y, r, vx, vy, color, int(100 / r))
    return 0

def add_rand_rect() -> int:
    """
    This function creates rectangular in radom point with random params
    @return Exit code
    """
    r = randint(10, 100)
    x = randint(r, WIDTH - r)
    y = randint(r, HEIGHT - r)
    vx = randint(-10, 10)
    vy = randint(-10, 10)
    color = COLORS[randint(0, len(COLORS) - 1)]
    add_rect(x, y, r, vx, vy, color, int(-100 / r))
    return 0

def main() -> int:
    """
    This is main function
    """
    setup("main", WIDTH, HEIGHT)
    init()
    for _ in range(randint(10, 10)):
        add_rand_circ()
    for _ in range(randint(10, 10)):
        add_rand_rect()
    mainloop()
    update_leaderboard()
    return 0

if (__name__ == "__main__"):
    main()
