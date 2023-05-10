import platform
import os

from pathlib import Path
import shutil

import InstallerUtils

_depReleaseName = "Dependencies-V1"
_libDirectory = "vendor/lib"

_monoDirectory = f"{_libDirectory}/mono"

def _CheckIfMonoExists():
    p = platform.system()

    monoPath = Path("")

    if(p == "Windows"):
        monoPath = Path(f"{_monoDirectory}/libmono-static-sgen.lib")
    else:
        raise Exception("Undefined OS detected!")
    
    return monoPath.exists()

def _GetDepReleaseUrl():
    latestRelease = InstallerUtils.GetLatestGitHubReleaseByName("ThewyRogue99", "DragonEngine", _depReleaseName)
    assets = latestRelease["assets"]

    for asset in assets:
        name = asset["name"]
        if(name == f'{_depReleaseName}.zip'):
            return asset["browser_download_url"]
    
def InstallLibraries(stdscr):
    uninstalledLibraries = []

    stdscr.addstr("Checking Libraries...\n")
    stdscr.refresh()

    if not _CheckIfMonoExists():
        uninstalledLibraries.append("mono")
    
    if len(uninstalledLibraries) > 0:
        stdscr.addstr("Installing Libraries...\n")
        stdscr.refresh()

        download_url = _GetDepReleaseUrl()
        fName = f"{_depReleaseName}.zip"
        download_path = f"{_libDirectory}/{fName}"

        stdscr.addstr("Downloading {0:s} to {1:s}\n".format(download_url, download_path))
        InstallerUtils.DownloadFile(stdscr, download_url, download_path)
        
        stdscr.addstr("Extracting Libraries...\n")
        InstallerUtils.UnzipFile(stdscr, download_path, deleteZipFile=True)

        for lib in uninstalledLibraries:
            stdscr.addstr(f"Extracting {lib}...\n")
            stdscr.refresh()

            shutil.copytree(f"{_libDirectory}/{_depReleaseName}/{lib}", f"{_libDirectory}/{lib}", dirs_exist_ok=True)
        
        shutil.rmtree(f"{_libDirectory}/{_depReleaseName}")
    else:
        stdscr.addstr("All libraries are up-to-date.\n")

    stdscr.addstr(f"Finished.\n")
