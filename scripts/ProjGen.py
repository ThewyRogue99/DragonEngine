import curses
import platform
import subprocess
import shutil

from LibSetup import InstallLibraries
from PremakeSetup import InstallPremake

import os

_engine_submodules = [
    "Box2D",
    "glfw",
    "yaml-cpp"
]

_editor_submodules = [
    "ImGui",
    "nfd"
]

_vs_versions = [
    "vs2017",
    "vs2019",
    "vs2022"
]

def GenerateProjects(stdscr):
    stdscr.clear()

    cwd = os.getcwd()
    os.chdir("..")

    stdscr.refresh()

    InstallPremake(stdscr)

    p = platform.system()

    selected_index = 2
    should_exit = False

    while not should_exit:
        if(p == "Windows"):
            stdscr.addstr("Please select a Visual Studio version:\n\n")

            for idx, version in enumerate(_vs_versions):
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
            elif key == curses.KEY_DOWN and selected_index < len(_vs_versions) - 1:
                selected_index += 1
            elif key == curses.KEY_ENTER or key in [10, 13]:
                should_exit = True
        elif(p == "Darwin"):
            selected_index = -1
            should_exit = True
        else:
            raise Exception("Undefined OS detected!")

    stdscr.clear()

    stdscr.addstr("Copying build scripts...\n")

    SetBuildScripts()

    stdscr.refresh()
    
    command = []

    if selected_index >= 0:
        command = ["vendor\\premake\\bin\\premake5.exe", _vs_versions[selected_index]]
    else:
        command = ["vendor/premake/bin/premake5", "xcode4"]

    process = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    result_str = process.stderr.decode('utf-8') + '\n'
    result_str += process.stdout.decode('utf-8')

    out_str_list = result_str.splitlines()

    parsed_out_str_list = []

    for out_str in out_str_list:
        if out_str != "":
            parsed_out_str_list.append(out_str)

    for i in range(0, len(parsed_out_str_list)):
        stdscr.addstr(i, 0, parsed_out_str_list[i])
    
    stdscr.addstr("\n\n")
    
    InstallLibraries(stdscr)

    stdscr.addstr("Press any key to continue...")

    stdscr.refresh()

    stdscr.getch()

    os.chdir(cwd)

def SetBuildScripts():
    # Engine Scripts
    for sub in _engine_submodules:
        shutil.copy(f'Engine/vendor/build/scripts/{sub}.lua', f'Engine/vendor/modules/{sub}/premake5.lua')
    
    # Editor Scripts
    for sub in _editor_submodules:
        shutil.copy(f'Editor/vendor/build/scripts/{sub}.lua', f'Editor/vendor/modules/{sub}/premake5.lua')