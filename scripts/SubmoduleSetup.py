import subprocess

import os

def UpdateSubmodules(stdscr):
    stdscr.clear()

    cwd = os.getcwd()
    os.chdir("..")

    stdscr.addstr(0, 0, "Updating Submodules...")

    stdscr.refresh()

    process = subprocess.run(["git", "submodule", "update", "--remote"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    result_str = process.stderr.decode('utf-8')
    if result_str != "":
        raise Exception(result_str)
    
    result_str = process.stdout.decode('utf-8') + '\n'

    stdscr.addstr(result_str)

    stdscr.refresh()

    stdscr.addstr("Finished\n\n")
    stdscr.addstr("Press any key to continue...")

    stdscr.refresh()

    stdscr.getch()

    os.chdir(cwd)