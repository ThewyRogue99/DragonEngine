import os
import platform

from pathlib import Path

import InstallerUtils

class PremakeSetup:
    def _GetPremakePlatformName(self):
        p = platform.system()
        if(p == "Windows"):
            return "windows"
        elif(p == "Darwin"):
            return "macosx"
        elif(p == "Linux"):
            return "linux"
        else:
            raise Exception("Undefined OS detected!")

    def _GetPremakeZipUrl(self):
        assets = self.latestRelease["assets"]
        pname = self._GetPremakePlatformName()
        for asset in assets:
            zipUrl = asset["browser_download_url"]
            if(pname in zipUrl):
                return zipUrl

    def __init__(self):
        self.latestRelease = InstallerUtils.GetLatestGitHubRelease("premake", "premake-core")
        self.premakeZipUrl = self._GetPremakeZipUrl()
        self.latestVersion = self.latestRelease["tag_name"]

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
        premakePath = f"{self.premakeDirectory}/premake-{self.latestVersion}-windows.zip"
        print(premakePath)
        print("Downloading {0:s} to {1:s}".format(self.premakeZipUrl, premakePath))
        InstallerUtils.DownloadFile(self.premakeZipUrl, premakePath)
        print("Extracting", premakePath)
        InstallerUtils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"Premake {self.latestVersion} has been downloaded to '{self.premakeDirectory}'")

        premakeLicensePath = f"{self.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(self.premakeLicenseUrl, premakeLicensePath))
        InstallerUtils.DownloadFile(self.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{self.premakeDirectory}'")

        return True

pSetup = PremakeSetup()

pSetup.Validate()