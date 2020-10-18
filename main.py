#!/usr/bin/python3

from mydraw import *
from random import randint

COLORS = ["#ff0000", "#0000ff", "#ffff00",
          "#00ff00", "#ff00ff", "#00ffff"];

def main() -> int:
    init("main")
    for _ in range(randint(0, 10)):
        add_circle(randint(0, 640), randint(0, 600), randint(10, 100), randint(-10, 10), randint(-10, 10), COLORS[randint(0, len(COLORS) - 1)], 10)
    for _ in range(randint(0, 10)):
        add_rect(randint(0, 640), randint(0, 600), randint(10, 100), randint(-10, 10), randint(-10, 10), COLORS[randint(0, len(COLORS) - 1)], -1)
    mainloop()
    return 0

if (__name__ == "__main__"):
    main()
