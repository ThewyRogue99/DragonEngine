import subprocess

import os
import shutil

__engine_submodules = [
    "Box2D",
    "glad",
    "glfw",
    "OpenAL-Soft",
    "yaml-cpp"
]

__editor_submodules = [
    "ImGui",
    "nfd"
]

def UpdateSubmodules(stdscr):
    stdscr.clear()

    cwd = os.getcwd()
    os.chdir("..")

    stdscr.addstr(0, 0, "Updating Submodules...")

    stdscr.refresh()

    process = subprocess.run(["git", "submodule", "update"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    result_str = process.stderr.decode('utf-8') + '\n'
    result_str += process.stdout.decode('utf-8')

    out_str_list = result_str.splitlines()

    parsed_out_str_list = []

    for out_str in out_str_list:
        if out_str != "":
            parsed_out_str_list.append(out_str)

    for i in range(0, len(parsed_out_str_list)):
        stdscr.addstr(i + 1, 0, parsed_out_str_list[i])

    __SetBuildScripts()

    stdscr.addstr(len(parsed_out_str_list) + 1, 0, "Finished")
    stdscr.addstr(len(parsed_out_str_list) + 3, 0, "Press any key to continue...")

    stdscr.refresh()

    stdscr.getch()

    os.chdir(cwd)


def __SetBuildScripts():
    # Engine Scripts
    for sub in __engine_submodules:
        shutil.copy(f'Engine/build/{sub}/premake5.lua', f'Engine/vendor/{sub}/premake5.lua')
    
    # Editor Scripts
    for sub in __editor_submodules:
        shutil.copy(f'Editor/build/{sub}/premake5.lua', f'Editor/vendor/{sub}/premake5.lua')