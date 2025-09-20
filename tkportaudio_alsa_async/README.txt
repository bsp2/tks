
** "tkportaudio_alsa" plugin

This is a very simplistic, API compatible replacement for the 
"tkportaudio" plugin which uses the Linux ALSA sound system.

It does not require the libportaudio packages to be installed.

The reason why I wrote this is that I had serious problems with
PortAudio on my new notebook (Intel HD Audio) running Debian/Lenny.

Neither the Debian portaudio packages worked, nor did my self-compiled
libportaudio (extrem stuttering, very high latencies. I tried OSS and
Alsa backends).

Well, this plugin gives me an OK-latency playback (ALSA reports bufsize=256, periodSize=128, #periods=2 but in reality each time the callback is invoked, it demands 1024 frames to be written).

Maybe/Probably with a better driver/better soundcard the latencies are better.

This version of "tkportaudio_alsa" uses async callbacks.

