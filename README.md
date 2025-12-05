THIS IS AN ARCHIVE FOR BOTH SOURCES e.g. BEservice.exe dummy project & Battleyent source project.
This is currently being replaced by another repo that does not require a dummy service.

INSERT - Toggle Menu
END - Safe Exit
(unload dll and terminate process)

♠ Player Features ♠
-God Mode
-Infinite Stamina
-No Fall Damage
-No Energy Drain
-No Hydration Drain
-No Fatigue

♠ Combat Features ♠
-No Recoil
-No Weapon Durability Loss
-No Weapon Malfunctions
-No Weapon Overheating

♠ AI Features ♠
-Disable AI Sight
-Disable AI Attacks

♠ Environment Features ♠
-No Barbed Wire Damage
-No Minefield Damage
-No Sniper Zone Damage

♠ Loot Features ♠
-Instant Search

♠ Utility Features ♠
-Breach Any Door

♠ Automatic Features ♠
-Forces offline PvE
(Select Streets or whatever, wait a few seconds, select factory or whatever, then switch back. The BSG Servers checkbox top right should be unticked before proceeding.)

_________________________________________________________________
Setup:
_________________________________________________________________

♠ Battleyen't ♠
BattlEyen't - play offline raids without BattlEye on live version
♠ TegridyMenu.zip ♠
TegridyMenu.zip Download

Build instructions below if you don't want to wait for download.

1. Extract contents of TegridyMenu.zip to Battleyen't folder.

2. Run Launch.bat as Admin
wait for it to finish it will start battleyent and close when it is done.

3. Launch game after battleyen't says waiting.



_________________________________________________________________
Build Instructions:
_________________________________________________________________

for modifying battleyent source & creating dummy beservice.exe

1st create the dummy service project and compile

In Visual Studio
Project type: C++
Template: Windows Desktop Application or a blank project.
Configuration type: Ensure that in the project properties -> Configuration Properties -> General -> Configuration Type, it is set to Application (.exe).

- 2nd download imgui-1.92.5 find this on google, you will need dependencies from this version.

- 3rd download battleyent source code
BattlEyen't - play offline raids without BattlEye on live version
open the folder and double click on battleyent_helper.sln
once loaded in Visual Studio, double click on dllmain.cpp and replace the code with the one below.


- you will need to include required Linker & C/C++ dependency stuff in the solution properties we will get to that last (if you know how to do this you can go ahead and add required) until this is done unresolved definition conflicts will occur we can skip for now.

- You can use this screenshot of my project to recreate main files
(resource files are extracted from imgui library we will do this at the end)
Headers & Source files need to be created ->



It's time to add dependencies to clear up unresolved errors and compile.
In the battleyent-master>battleyent_helper
Create a folder named ImGui and extract the imgui files needed use screenshot for reference.



Now include ImGui Right click battleyent_helper in solutionexplorer>properties>C/C++>



And the last dependency go to Linker>Input>add d3d11.lib



After everything is done we finally
set build mode to Release x64, then build and grab the newly build battleyent_helper.dll and replace the old one from the battleyent folder
Drop your bedummy.exe into the same folder and rename it BEservice.exe
That's it.. That's a lot and this took me a long time to write, if it's any use to you thanks for reading.

Use this Launch.bat to start must run as Admin (it will kill/remove BE and setup dummy, then open battleyent read Setup for info on using.)



Also here's a bat for restoring battleye (may not always work) if it doesn't, manually run sc delete BEservice in admin powershell.
Verify game files and repair.


If I missed something in this massive post lemme know I'll fix. Laters

Update: After testing today the steps I follow to go back to online play is the following and it works for me what works for you may be different.
-run cmd as admin and type: sc delete BEservice then hit enter
-open bsg launcher, clear cache, browse local files and close the launcher.
-in the local files, open logs folder and delete all logs, then go back into the bsg launcher directory, and run the uninstall.
-now reinstall the launcher. sign in. click play. It should run as normal with battleye.
