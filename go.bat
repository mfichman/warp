start /Daudio "Warp: ChucK Server" chuck.exe play.ck
FOR /F "tokens=2" %%I in ('TASKLIST /NH /FI "WINDOWTITLE eq Warp: ChucK Server"' ) DO SET PID=%%I 
start /W warp
TASKKILL /PID %PID% 
