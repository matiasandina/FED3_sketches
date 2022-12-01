This is a FreeFeeding program that gives a pellet with an inter-trial-interval.
It follows an experimental paradigm shown in the diagram below.

![](exp_timeline.png)

## Library Breaks

This code has the addition of a function to externalize `rtc.now()` into `fed3.now()` to provide `DateTime` access in the `.ino`


This code sends pulses on every event using [FEDWatcher](https://github.com/matiasandina/FEDWatcher).
Because of that, this folder contains associated files to bypass the standard library and run from local files.

Check `version.txt` for a list of FED3 libraries that were tested with this program.
