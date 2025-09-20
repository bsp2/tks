
** "tkportaudio_alsa" plugin

This is an API compatible replacement for the 
"tkportaudio" plugin which uses the Linux ALSA sound system.

It does not require the portaudio packages to be installed.

The reason why I wrote this is that I had serious problems with
PortAudio on my new notebook (Intel HD Audio) running Debian/Lenny.

Neither the Debian portaudio packages worked, nor did my self-compiled
libportaudio (extrem stuttering, very high latencies. I tried OSS and
Alsa backends).

Well, this plugin gives me an OK-latency playback (ALSA reports bufsize=256, periodSize=128, #periods=2 but in reality each time the callback is invoked, it demands 1024 frames to be written).

Maybe/Probably with a better driver/better soundcard the latencies are better.

This version of "tkportaudio_alsa" does NOT use async callbacks.

[26Jul2023]:
Please stop pulse audio / pipewire services before running native ALSA software:
stop_pulseaudio.sh:
#https://linuxmusicians.com/viewtopic.php?t=23206
systemctl --user stop pulseaudio.socket && systemctl --user stop pulseaudio.service
systemctl --user stop pipewire.socket && systemctl --user stop pipewire.service
systemctl --user stop pipewire-pulse.socket && systemctl --user stop pipewire-pulse.service

Tested with
 - "HDA Intel PCH,ALC3271 Analog"
 - Native Instruments Session I/O (2x2)
 - Focusrite Scarlett 18i20 2nd gen
    in /etc/modprobe.d/alsa_scarlett.conf (create new file):
    options snd_usb_audio vid=0x1235 pid=0x8215 device_setup=1
    # see also: https://github.com/geoffreybennett/alsa-scarlett-gui/blob/master/USAGE.md

Example:
see tks-examples/tkportaudio/testsine.tks  (capture+playback)

