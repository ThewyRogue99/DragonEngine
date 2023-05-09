from SubmoduleSetup import UpdateSubmodules
from ProjGen import GenerateProjects
import curses

__options = [
    ( "ProjGen", "Generate Projects" ),
    ( "ModUpdate", "Update Modules" ),
    ( "Exit", "Close Manager")
]

def print_menu(stdscr, selected_row_idx):
    stdscr.clear()

    stdscr.addstr(0, 0, "Welcome to Dragon Engine Project Manager!")
    stdscr.addstr(1, 0, "Please select an option:")

    for idx, row in enumerate(__options):
        x = 0
        y = idx + 3

        if idx == selected_row_idx:
            stdscr.attron(curses.color_pair(1))
            stdscr.addstr(y, x, f'{idx + 1}) {row[1]} ')
            stdscr.attroff(curses.color_pair(1))
        else:
            stdscr.addstr(y, x, f'{idx + 1}) {row[1]} ')

    stdscr.refresh()


def print_center(stdscr, text):
    stdscr.clear()
    h, w = stdscr.getmaxyx()
    x = w//2 - len(text)//2
    y = h//2
    stdscr.addstr(y, x, text)
    stdscr.refresh()

def main(stdscr):
    # turn off cursor blinking
    curses.curs_set(0)

    # color scheme for selected row
    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_WHITE)

    stdscr.scrollok(True)

    # specify the current selected row
    current_row = 0

    # print the menu
    print_menu(stdscr, current_row)

    while True:
        key = stdscr.getch()

        if key == curses.KEY_UP and current_row > 0:
            current_row -= 1
        elif key == curses.KEY_DOWN and current_row < len(__options) - 1:
            current_row += 1
        elif key == curses.KEY_ENTER or key in [10, 13]:
            should_exit = HandleOption(stdscr, __options[current_row][0])
            if should_exit:
                break

        print_menu(stdscr, current_row)

def HandleOption(stdscr, option):
    match option:
        case "ProjGen":
            GenerateProjects(stdscr)
            return False
        case "ModUpdate":
            UpdateSubmodules(stdscr)
            return False
        case "Exit":
            return True

curses.wrapper(main)