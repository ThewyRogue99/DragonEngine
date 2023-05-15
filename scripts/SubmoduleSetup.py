import subprocess

from ProjGen import SetBuildScripts

import os

def UpdateSubmodules(stdscr):
    stdscr.clear()

    cwd = os.getcwd()
    os.chdir("..")

    stdscr.addstr(0, 0, "Updating Submodules...\n")

    stdscr.refresh()

    process = subprocess.run(["git", "submodule", "update", "--init", "--remote"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    result_str = process.stderr.decode('utf-8')
    if result_str != "":
        raise Exception(result_str)
    
    result_str = process.stdout.decode('utf-8') + '\n'

    stdscr.addstr(result_str)

    stdscr.addstr("Copying build scripts...\n")

    stdscr.refresh()

    SetBuildScripts()

    stdscr.addstr("Finished\n\n")
    stdscr.addstr("Press any key to continue...")

    stdscr.refresh()

    stdscr.getch()

    os.chdir(cwd)