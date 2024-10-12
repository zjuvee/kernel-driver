sc create kerneldriver type= kernel binPath= "D:\Repos\gamehacking\kernel-game\x64\Release\kernel-game.sys"
sc start kerneldriver
timeout /t 2
sc stop kerneldriver
sc delete kerneldriver