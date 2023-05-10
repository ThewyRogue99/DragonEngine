import subprocess

import os
import shutil

__engine_submodules = [
    "Box2D",
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

    result_str = process.stderr.decode('utf-8')
    if result_str != "":
        raise Exception(result_str)
    
    result_str = process.stdout.decode('utf-8') + '\n'

    stdscr.addstr(result_str)

    stdscr.refresh()

    stdscr.addstr("Copying build scripts...\n")

    __SetBuildScripts()

    stdscr.addstr("Finished\n\n")
    stdscr.addstr("Press any key to continue...")

    stdscr.refresh()

    stdscr.getch()

    os.chdir(cwd)


def __SetBuildScripts():
    # Engine Scripts
    for sub in __engine_submodules:
        shutil.copy(f'Engine/vendor/build/scripts/{sub}.lua', f'Engine/vendor/modules/{sub}/premake5.lua')
    
    # Editor Scripts
    for sub in __editor_submodules:
        shutil.copy(f'Editor/vendor/build/scripts/{sub}.lua', f'Editor/vendor/modules/{sub}/premake5.lua')