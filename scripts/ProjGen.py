import curses
import platform
import subprocess

import os

__vs_versions = [
    "vs2017",
    "vs2019",
    "vs2022"
]

def GenerateProjects(stdscr):
    stdscr.clear()

    p = platform.system()

    selected_index = 2
    should_exit = False

    while not should_exit:
        if(p == "Windows"):
            stdscr.clear()

            stdscr.addstr("Please select a Visual Studio version:\n\n")

            for idx, version in enumerate(__vs_versions):
                if idx == selected_index:
                    stdscr.attron(curses.color_pair(1))
                    stdscr.addstr(f'{idx + 1}) {version} \n')
                    stdscr.attroff(curses.color_pair(1))
                else:
                    stdscr.addstr(f'{idx + 1}) {version} \n')

            stdscr.refresh()

            key = stdscr.getch()

            if key == curses.KEY_UP and selected_index > 0:
                selected_index -= 1
            elif key == curses.KEY_DOWN and selected_index < len(__vs_versions) - 1:
                selected_index += 1
            elif key == curses.KEY_ENTER or key in [10, 13]:
                should_exit = True
        elif(p == "Darwin"):
            selected_index = -1
            should_exit = True
        else:
            raise Exception("Undefined OS detected!")

    stdscr.clear()
    stdscr.refresh()
    
    command = []

    if selected_index >= 0:
        command = ["vendor\\premake\\bin\\premake5.exe", __vs_versions[selected_index]]
    else:
        command = ["vendor/premake/bin/premake5", "xcode4"]

    cwd = os.getcwd()
    os.chdir("..")

    process = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    os.chdir(cwd)

    result_str = process.stderr.decode('utf-8') + '\n'
    result_str += process.stdout.decode('utf-8')

    out_str_list = result_str.splitlines()

    parsed_out_str_list = []

    for out_str in out_str_list:
        if out_str != "":
            parsed_out_str_list.append(out_str)

    for i in range(0, len(parsed_out_str_list)):
        stdscr.addstr(i, 0, parsed_out_str_list[i])

    stdscr.addstr("\n\nPress any key to continue...")

    stdscr.refresh()

    stdscr.getch()