import os
import platform

from pathlib import Path

import InstallerUtils

_premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
_premakeDirectory = "vendor/premake/bin"

def _GetPremakePlatformName():
    p = platform.system()

    if(p == "Windows"):
        return "windows"
    elif(p == "Darwin"):
        return "macosx"
    else:
        raise Exception("Undefined OS detected!")

def _GetPremakeAsset():
    latestRelease = InstallerUtils.GetLatestGitHubRelease("premake", "premake-core")
    assets = latestRelease["assets"]
    pname = _GetPremakePlatformName()

    for asset in assets:
        zipUrl = asset["browser_download_url"]
        if(pname in zipUrl):
            return asset

def _CheckIfPremakeInstalled():
    p = platform.system()

    premakePath = Path("")

    if(p == "Windows"):
        premakePath = Path(f"{_premakeDirectory}/premake5.exe")
    elif(p == "Darwin"):
        premakePath = Path(f"{_premakeDirectory}/premake5")
    else:
        raise Exception("Undefined OS detected!")
    
    return premakePath.exists()

def InstallPremake(stdscr):
    if not _CheckIfPremakeInstalled():
        stdscr.addstr("Installing premake...\n")
        stdscr.refresh()

        premakeAsset = _GetPremakeAsset()
        fName = premakeAsset["name"]
        premakeUrl = premakeAsset["browser_download_url"]
        premakePath = f"{_premakeDirectory}/{fName}"

        stdscr.addstr("Downloading {0:s} to {1:s}\n".format(premakeUrl, premakePath))
        InstallerUtils.DownloadFile(stdscr, premakeUrl, premakePath)
        
        stdscr.addstr("Extracting premake...\n")
        InstallerUtils.UnzipFile(stdscr, premakePath, deleteZipFile=True)
        stdscr.addstr(f"{fName} has been downloaded to '{_premakeDirectory}'\n")

        premakeLicensePath = f"{_premakeDirectory}/LICENSE.txt"
        stdscr.addstr("Downloading {0:s} to {1:s}\n".format(_premakeLicenseUrl, premakeLicensePath))
        InstallerUtils.DownloadFile(stdscr, _premakeLicenseUrl, premakeLicensePath)
        stdscr.addstr(f"Premake License file has been downloaded to '{_premakeDirectory}'\n\n")

        stdscr.refresh()

    return True