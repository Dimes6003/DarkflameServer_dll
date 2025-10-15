# DarkflameServer_dll


### Compatible with 1.11 fork only:

[Server Code](https://github.com/Fire-Temple/DarkflameServer)

[Server Releases](https://github.com/Fire-Temple/DarkflameServer/releases)

   
### Install

  **Be sure** you created at least one account on your server before installing
  
1. Download latest release here: 
  [Releases](https://github.com/Dimes6003/DarkflameServer_dll/releases)
  
  
2. Extract the release you downloaded into your LEGO Universe client, You should see these files in your client:
  
	> ```diff
	> d3d9.dll
	> legouniverse.exe
	> server.ini
  
3. Open `server.ini` and add the directory to your MasterServer.exe to `lwoServerPath = ` like so:
     
  	> lwoServerPath = C:/path/to/MasterServer.exe
  
5. Run `legouniverse.exe` enjoy :)
  
  **DO NOT** run multiple instances of `legouniverse.exe` when using the dll,   
  it's the same as running DLU multiple times and can cause issues

   
### Build from source

Built with **VS 2022** 

1. Run `git clone https://github.com/Dimes6003/DarkflameServer_dll` or just download zip

2. In VS, go to `Project -> Properties -> C/C++ -> Additional Include Directories`   
  and add directory to the `minhook/include` folder provided

**ONLY** build as Release and x86   
in order to work with LEGO Universe

   
### Credits

  **d3d9-blank-proxy**
  
  Source: https://github.com/ME3Tweaks/d3d9-blank-proxy
  
  **minhook**   
  
  Source: https://github.com/TsudaKageyu/minhook   
  License: https://github.com/Dimes6003/DarkflameServer_dll/blob/main/minhook/LICENSE.txt
