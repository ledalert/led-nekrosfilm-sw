# nekros_filmljus
Small project to utilize LED Alert lamps for movie lighting

This project is in early development and poorly documented for the moment.
Hopefully there will be more time for documentation and portability later.

As of now you can make your own hardware profile looking at how it is done for rxlamp and 5strip and point to 
timer capture commpare registers to run on a lot of different MCU's. The uart functionality should be moved to the
hardware profile, end goal is a completely abstracted hardware with low penalty by implementing hardware specific 
functions on as high level as possible.

Severe known bugs:

If multiple lamps are hooked up on a multi drop bus they will get confused by the data packets since they are header less.
This will be fixed rather soon but it involves adding header for the data packages as well.
