import PremakeSetup
import MonoSetup

pSetup = PremakeSetup.PremakeSetup()
pSetup.InstallPremake()

mSetup = MonoSetup.MonoSetup()
mSetup.InstallMono()

fString = """pushd ..\\
call vendor\\premake\\bin\\premake5.exe vs2022
popd

PAUSE"""

f = open("Win-GenProjects.bat", "w")
f.write(fString)
f.close()