import os
import platform

from pathlib import Path

import InstallerUtils

class PremakeSetup:
    def _GetPremakePlatformName(self):
        p = platform.system()
        if(p == "Windows"):
            return "windows"
        #elif(p == "Darwin"):
            #return "macosx"
        else:
            raise Exception("Undefined OS detected!")

    def _GetPremakeAsset(self):
        assets = self.latestRelease["assets"]
        pname = self._GetPremakePlatformName()
        for asset in assets:
            zipUrl = asset["browser_download_url"]
            if(pname in zipUrl):
                return asset

    def __init__(self):
        self.latestRelease = InstallerUtils.GetLatestGitHubRelease("premake", "premake-core")
        self.premakeAsset = self._GetPremakeAsset()

    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "../vendor/premake/bin"

    def Validate(self):
        if (not self.CheckIfPremakeInstalled()):
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(self.premakeDirectory)}")
        return True

    def CheckIfPremakeInstalled(self):
        premakeExe = Path(f"{self.premakeDirectory}/premake5.exe");
        if (not premakeExe.exists()):
            return self.InstallPremake()

        return True

    def InstallPremake(self):
        fName = self.premakeAsset["name"]
        premakeUrl = self.premakeAsset["browser_download_url"]
        premakePath = f"{self.premakeDirectory}/{fName}"
        print("Downloading {0:s} to {1:s}".format(premakeUrl, premakePath))
        InstallerUtils.DownloadFile(premakeUrl, premakePath)
        print("Extracting", premakePath)
        InstallerUtils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"{fName} has been downloaded to '{self.premakeDirectory}'")

        premakeLicensePath = f"{self.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(self.premakeLicenseUrl, premakeLicensePath))
        InstallerUtils.DownloadFile(self.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{self.premakeDirectory}'")

        return True