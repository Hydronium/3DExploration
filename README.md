Plaintext because I can.
This README will serve as a log as of Nov 23, 2012.

[Nov 23, 2012]
Original project had errors. Suspect them to be due to error swallowing by Windows 7 x64 OS.
Restarted project. So far it is clean. Shinier, even.

-Redid window creation and message handling.
-Implemented Timer-queue with base timer at 8ms period.
-Program now blocks while waiting for timer to elapse, allowing the CPU to handle other programs and preventing excessive CPU usage. (Worked first try, hell yea. (And so far without any errors I'm able to detect, though the resolution may suck. We'll see.))
-OpenGL functions are loaded, no errors.
-Created an application manifest resource through Pelles C that should eliminate Win7 x64 compatibility issues.
