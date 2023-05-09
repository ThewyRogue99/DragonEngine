import os
import platform
import shutil

from pathlib import Path

import InstallerUtils

class MonoSetup:
    def _GetInstallLink(self):
        p = platform.system()
        if(p == "Windows"):
            return "https://download.mono-project.com/archive/6.12.0/windows-installer/mono-6.12.0.182-x64-0.msi"
        #elif(p == "Darwin"):
            #return "https://download.mono-project.com/archive/6.12.0/macos-10-universal/MonoFramework-MDK-6.12.0.182.macos10.xamarin.universal.pkg"
        else:
            raise Exception("Undefined OS detected!")

    def __init__(self):
        self.installLink = self._GetInstallLink()

    def InstallMono(self):
        fName = "mono-6.12.0.182-x64-0.msi"
        pwd = Path(os.getcwd())

        p = platform.system()
        if p == "Windows":
            installPath = str(pwd.parent.absolute().joinpath("vendor\\lib\\mono"))
            print("Downloading {0:s} to {1:s}".format(self.installLink, installPath))
            InstallerUtils.DownloadFile(self.installLink, f"{installPath}/{fName}")
        
            print("Extracting...")
            os.chdir(installPath)
            os.system(f"start /wait msiexec /a {fName} /qn /norestart TARGETDIR={installPath}\\bin /log install.log")
        else:
            raise Exception("Undefined OS detected!")

        shutil.move(f"./bin/Mono/lib/mono/4.5", "./4.5")
        shutil.move(f"./bin/Mono/lib/mono/4.7.2-api", "./4.7.2-api")
        shutil.move(f"./bin/Mono/bin/mono-2.0-sgen.dll", ".")
        shutil.move(f"./bin/Mono/lib/mono-2.0-sgen.lib", ".")

        shutil.rmtree("bin")
        os.remove(fName)
        os.remove("install.log")

        os.chdir(pwd)
        print("Finished")

        return True