
** "tkportaudio_oss" plugin

This is a very simplistic, API compatible replacement for the 
"tkportaudio" plugin which uses the Linux /dev/dsp OSS device.

It does not require the libportaudio packages to be installed.

The reason why I wrote this is that I had serious problems with
PortAudio on my new notebook (Intel HD Audio) running Debian/Lenny.

Neither the Debian portaudio packages worked, nor did my self-compiled
libportaudio (extrem stuttering, very high latencies. I tried OSS and
Alsa backends).

Well, this plugin gives me a very nice low-latency playback (down to 2 samples!).


