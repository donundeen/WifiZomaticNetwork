Daily Record of all the edits and commands run on the Pi to get it running.

Code things are in backticks `like this` , so don't type the backticks into your commands when you see them.


## Nov 8 2021
- new image installed using raspberry Pi Imager. Settings:
    - Rasbian Lite OS
    - command-shift-X for advanced options
        - auto-fill wifi from keychain
        - set hostname rasberrypi.local
        - enable SSH
        - password for pi user: my usual makerspace on
        - Wifi Country: CA
        - Locale Settings
            - Timezone: Canada/Eastern
        - Skip first-run wizard: yes
    - Write to Disk (takes some time)
- Remove SD card, re-insert
- Edits on SD card to support USB connect
    - in boot folder, type `touch ssh` to create empty file called ssh
        - select 'overlays' folder, then finger->services->open iterm 2 window here
        - in term: `cd .. ; touch ssh`
    - edit config.txt, adding     dtoverlay=dwc2 at the end, under [all]
    - Now, edit the file called `cmdline.txt`. Look for `rootwait`, and add `modules-load=dwc2,g_ether` immediately after. (with a space after rootwait and after g_ether)
- Eject SD card

Note: Use __ usb port for connected via Ethernet-over-usb

## Nov 14, 2021

### connecting for the first time
- inserted SD card
- inserted icro usb from latop to Pi, using Outer usb port on pi
    - Longer wait the first time, to connect
- open terminal in laptop, type:
    - `ssh pi@raspberrypi.local`
- Got error:
    - WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!
    - this happens whenever I set up a new SD card image for a pi I've already used. 
    - Need to go into my laptop's file: /Users/donaldundeen/.ssh/known_hosts
        - And delete the line that references the Pi:
        - `sudo nano /Users/donaldundeen/.ssh/known_hosts`
        - find a line that starts with something like `raspberrypi.local,2607:fa49:6141:6b00::32e1` and delete it.
- Now connect to pi from laptop with `ssh pi@raspberrypi.local`
    - agree to accept ECDSA fingerprint
    - enter password to log in to pi


### Initial upgrade and installs

Testing wifi access
`ping google.com`
- works!

update and upgrade:
`
sudo apt-get update
sudo apt-get upgrade
`
- this part always takes a while…


###  Trying this, to get pi working in AP and Client mode:
https://github.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP

```shell
curl https://raw.githubusercontent.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP/master/ap_sta_config.sh | sudo bash -s -- --ap thelog loglogloglog --client JJandJsKewlPad MyWIFIPW --country CA --ip 10.0.0.202

```

Response:
`Populate /etc/udev/rules.d/70-persistent-net.rules
Populate /etc/dnsmasq.conf
Populate /etc/hostapd/hostapd.conf
bash: line 1: /etc/hostapd/hostapd.conf: No such file or directory`

Create hostapd.conf file:
`sudo mkdir /etc/hostapd ; sudo touch /etc/hostapd/hostapd.conf`

Ran curl command again, got result:
Unit dnsmasq.service does not exist, proceeding anyway.
Unit hostapd.service does not exist, proceeding anyway.
Failed to enable unit: Unit file dnsmasq.service does not exist.

need to install hostapd and dnsmasq:
`
sudo apt-get install hostapd
sudo apt-get install dnsmasq
`

Then turn them off:
`    
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq
`

Then run the curl command again

Command completes, looks like success?

`sudo reboot`

then reconnect to pi
`ping google.com` - success

But, could not find AP "thelog"
- suspect a problem with hostapd
- 
ran:
`sudo hostapd /etc/hostapd/hostapd.conf`
got results (amongst other things)
`Could not read interface ap0 flags: No such device
nl80211: Driver does not support authentication/association or connect commands
`
Other egs of hostapd.conf don't include the driver line, so goign to edit hostapd.conf to comment that out
`#driver=nl80211`
- no change, same results
- changed the file back to uncomment the driver line

ran this command:
`sudo /sbin/iw dev wlan0 interface add ap0 type __ap`
and the usb connection to the laptop was lost and I'm unable to reconnect…

    unplug and replug pi - able to log back in

Ran this command (from part of /etc/udev/rules.d/70-persistent-net.rules):
`/sbin/iw phy phy0 interface add ap0 type __ap`
and I get kicked off the pi again, but now I see thelog as an available SSID
- trying to connect with my phone, but it just keeps spinning…


Ran
`journalctl -b` to see boot debugging messages

In the output, see:
`systemd-udevd[120]: /etc/udev/rules.d/70-persistent-net.rules:1 Invalid key/value pair, ignoring.`
- something to look into...

edited 
/etc/udev/rules.d/70-persistent-net.rules

looks like there was a missing quote at the end?
`
SUBSYSTEM=="ieee80211" \
, ACTION=="add|change" \
, ATTR{macaddress}=="b8:27:eb:5c:1f:0e", \
KERNEL=="phy0", \
RUN+="/sbin/iw phy phy0 interface add ap0 type __ap", \
RUN+="/bin/ip link set ap0 address b8:27:eb:5c:1f:0e"
`

rebooting…
now I see thelog SSID!
and I can ping google
- I can't seem to get to the internet when I'm connected to thelog network from my laptop, but I don't want to anyways.


### (skipping for now) Setting up capture page:
https://github.com/openNDS/openNDS 

Digging into the capture page issue, it seems like setting up a capture page when the rpi isn't ALSO connecting to the internet, is pretty complicated:
https://github.com/openNDS/openNDS/issues/170

it might get easier, but for now maybe I'll focus on setting up the webserver

### Set up Webserver

Installing Apache2:
`sudo apt-get install apache2` 
- now I can go to: http://raspberrypi.local/ and see the default apache webpage
- this works when I'm connected via USB, or when I connect to the log SSID

### removing pw for the AP
- edited /etc/hostapd/hostapd.conf
- commented out all lines that start with wpa_ or rsn_

### Changed SSID name to 'log'
- edited /etc/hostapd/hostapd.conf
- change line to `ssid=log`
- 

### Changing hostname to 'log'
https://www.tomshardware.com/how-to/raspberry-pi-change-hostname
`sudo nano /etc/hosts`
- change line `127.0.1.1       raspberrypi`
- to `127.0.1.1       log`

`sudo nano /etc/hostname`
- change `raspberrypi` to `log`

reboot

Now can ssh to pi@log.local
and can get on SSID log with no pw


### Installing PHP
`sudo apt install php7.3 libapache2-mod-php7.3 php7.3-mbstring php7.3-mysql php7.3-curl php7.3-gd php7.3-zip -y`
- got lots of messages like:
` E: Unable to locate package php7.3
E: Couldn't find any package by glob 'php7.3'`
trying 
`sudo apt-get install php`
- this works, and installs php 7.4

created file /var/www/html/index.php that contains 
`<?php phpinfo(); ?>`
then I can go to:
http://log.local/index.php
and see all the details.

get rid of /var/www/html/index.html, so /var/www/html/index.php is the default
`sudo mv /var/www/html/index.html /var/www/html/index.html.bak`



### Next Steps
- Set up a simple db or log file, and a simple web form for capturing input and displaying what previous people have entered. Try to make it as SIMPLE as possible. (maybe use a logging library?)
- check out SQLite:
    - https://www.php.net/manual/en/book.sqlite3.php
    - https://pimylifeup.com/raspberry-pi-sqlite/
    - https://forums.raspberrypi.com/viewtopic.php?t=268454 

## Nov 27 2021

Change wifi name to log.local, so it's the same as the website name.

### Changed SSID name to 'log.local'
- edited /etc/hostapd/hostapd.conf
- change line to `ssid=log.local`
- rebooted
- now network shows up as log.local

### trying to copy files from pi to local desktop, via scp
`scp pi@log.local:/home/pi/testmove.txt ./ `
- that copies the one file
`scp -r pi@log.local:/home/pi/testdir/ ./`
- copies entire dir
`scp pi@log.local:/etc/hostapd/hostapd.conf ./`
- worked for copying a file that user pi doesn't own.

However, I want to copy specific file and recreate their full paths in a local directory, though not copy that entire directory.

see notes in ModifiedFiles.md for lists of files to copy, and script for doing it.

### working on website
create new, blank index.php page
now to remember php!
sqlite3 tutorial here: 
https://www.tutorialspoint.com/sqlite/sqlite_php.htm

### Install SQLite3 for php
`sudo apt-get install php-sqlite3`
reboot


### fix some permissions for pi and apache
Add pi to www-data group
`sudo usermod -a -G www-data pi` 
make sure www-date owns everything in /var/www/html
`sudo chown -R -f www-data:www-data /var/www/html`


### Basic Page Running
list of questions, selected from at random
stored in db
all answers displayed in descending date order (most recent first)


### Git repo created
https://github.com/donundeen/loginlog/

### Backup Script
done. simple rsync command that pulls a list of files from the pi to local computer, then github command that pushes repo to github.

see modifiedfiles.md for shell command


### Text Formatted in web page
mostly using css vw proportions for making text size relative to the view window.



# Dec 26
creating a second Log pi, using journal entries

new image installed using raspberry Pi Imager. Settings:
    - Rasbian Lite OS
    - command-shift-X for advanced options
        - auto-fill wifi from keychain
        - set hostname log.local
        - enable SSH
        - password for pi user: my usual makerspace pw
        - Wifi Country: CA
        - Locale Settings
            - Timezone: Canada/Eastern
        - Skip first-run wizard: yes
    - Write to Disk (takes some time)
- Remove SD card, re-insert
- Edits on SD card to support USB connect
    - open terminal window on laptop
        - right click on a file in the SD card, select "open iterm here"
    - in boot folder, type `touch ssh` to create empty file called ssh
    - edit config.txt, adding     dtoverlay=dwc2 at the end, under [all]
    - Now, edit the file called `cmdline.txt`. Look for `rootwait`, and add `modules-load=dwc2,g_ether` immediately after. (with a space after rootwait and after g_ether)
- Eject SD card
    - hrm, have to do a "force eject"
- put SD Card in rPi 
- inserted micro usb from latop to Pi, using Inner usb port on pi
    - Longer wait the first time, to connect
- open terminal in laptop, type:
    - `ssh pi@log.local`
    - waiting, waiting.. did the force eject screw up the card, or do I need to connect to the other USB port, or do I just need to wait longer?
    - unplug, replug, try again…
    - ok, let's try re-formatting the SD card…
- reformatted card and went through process again
    - still not able to ssh in.…
    - 
 
# Dec 26
- trying again, with the other Pi 0
- new image installed using raspberry Pi Imager. Settings:
    - Rasbian Lite OS
    - command-shift-X for advanced options
        - auto-fill wifi from keychain
        - set hostname raspberrypi.local
            - I'll change this to log.local later; maybe it was a problem?
        - enable SSH
        - password for pi user: my usual makerspace pw
        - Wifi Country: CA
        - Locale Settings
            - Timezone: Canada/Eastern
        - Skip first-run wizard: yes
    - Write to Disk (takes some time)
- Remove SD card, re-insert
- Edits on SD card to support USB connect
    - open terminal window on laptop
        - right click on a file in the SD card, select "open iterm here"
    - in boot folder, type `touch ssh` to create empty file called ssh
    - edit config.txt, adding     dtoverlay=dwc2 at the end, under [all]
    - Now, edit the file called `cmdline.txt`. Look for `rootwait`, and add `modules-load=dwc2,g_ether` immediately after. (with a space after rootwait and after g_ether)
- inserted micro usb from latop to Pi, using INNER usb port on pi
    - wait a while…
- SSH to pi@raspberrypi.local - it works!!
- got error about The ECDSA host key for raspberrypi.local has changed,because I use multiple devices with the same hostname
- have to go into file /Users/donaldundeen/.ssh/known_hosts and delete the line with raspberrypi.local (and the one with log.local for good measure)
- Forget to set the custom password, doing that now
    - command is `passwd`

Testing wifi access
`ping google.com`
- "temporary failure in name resolution"
- so wifi not working this time?
- seems like i forgot to configure it?
- using the instructions here: http://codefoster.com/pi-wifi/ to use the wpa_cli command-line interface for setting up networking
- hm, it doesn't seem to like it when the SSID doesn't have a password.
- let's try the one that DOES have a password
- hrm, that didn't work either, and wpa_supplicant hasn't changed… 
- trying raspi-config
    - `sudo raspi-config` 
    - System settings
    - set ssid and password
- NOW I can ping google.com!
- 

update and upgrade:
`
sudo apt-get update && sudo apt-get upgrade
`
- this part always takes a while…

### setting up AP
need to install hostapd and dnsmasq:
`
sudo apt-get install hostapd
sudo apt-get install dnsmasq
`

Then turn them off:
`    
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq
`

from: https://github.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP

```shell
curl https://raw.githubusercontent.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP/master/ap_sta_config.sh | sudo bash -s -- --ap log.local loglogloglog --client JJandJsKewlPad MyWifiPw --country CA --ip 10.0.0.202
```

edited 
sudo nano /etc/udev/rules.d/70-persistent-net.rules

looks like there was a missing quote at the end?
```
SUBSYSTEM=="ieee80211" \
, ACTION=="add|change" \
, ATTR{macaddress}=="b8:27:eb:5c:1f:0e", \
KERNEL=="phy0", \
RUN+="/sbin/iw phy phy0 interface add ap0 type __ap", \
RUN+="/bin/ip link set ap0 address b8:27:eb:5c:1f:0e"
```

- Rebooting
- I can see the network log.local, and connect with the password!

### removing pw for the AP
- edited /etc/hostapd/hostapd.conf
- commented out all lines that start with wpa_ or rsn_


### Changing hostname to 'log'
https://www.tomshardware.com/how-to/raspberry-pi-change-hostname
`sudo nano /etc/hosts`
- change line `127.0.1.1       raspberrypi`
- to `127.0.1.1       log`

`sudo nano /etc/hostname`
- change `raspberrypi` to `log`


Reboot
- can local in to log.local SSID without password now
- can ssh pi@log.local
- hrm, but when I SSH in, I can't ping google.com (no internet?)
    - and I can't use raspi-config to setup netowkring (can't access wpa_supplicant)

### debugging no internet connection
pi@log:~ $ sudo ifup wlan0
wpa_supplicant: /sbin/wpa_supplicant daemon failed to start
run-parts: /etc/network/if-pre-up.d/wpasupplicant exited with return code 1
ifup: failed to bring up wlan0

pi@log:~ $ tail /var/log/daemon.log
Dec 27 22:30:00 log wpa_supplicant[1170]: ctrl_iface exists and seems to be in use - cannot override it
Dec 27 22:30:00 log wpa_supplicant[1170]: Delete '/var/run/wpa_supplicant/wlan0' manually if it is not used anymore
Dec 27 22:30:00 log wpa_supplicant[1170]: Failed to initialize control interface 'DIR=/var/run/wpa_supplicant GROUP=netdev'.#012You may have another wpa_supplicant process already running or the file was#012left by an unclean termination of wpa_supplicant in which case you will need#012to manually remove this file before starting wpa_supplicant again.
Dec 27 22:30:00 log wpa_supplicant[1170]: nl80211: deinit ifname=wlan0 disabled_11b_rates=0

pi@log:~ $ sudo rm /var/run/wpa_supplicant/wlan0
pi@log:~ $ sudo ifup wlan0
pi@log:~ $ ping google.com
PING google.com(yul02s04-in-x0e.1e100.net (2607:f8b0:4020:804::200e)) 56 data bytes

so now I've got internet on the pi. 

rebooting
- no internet

**in the /var/log/daemon.log:**
Dec 27 22:41:18 log wpa_supplicant[914]: ctrl_iface exists and seems to be in use - cannot override it
Dec 27 22:41:18 log wpa_supplicant[914]: Delete '/var/run/wpa_supplicant/wlan0' manually if it is not used anymore
Dec 27 22:41:18 log wpa_supplicant[914]: Failed to initialize control interface 'DIR=/var/run/wpa_supplicant GROUP=netdev'.#012You may have another wpa_supplicant process already running or the file was#012left by an unclean termination of wpa_supplicant in which case you will need#012to manually remove this file before starting wpa_supplicant again.

so it seems like there's another wpa_supplicant process set up somehow/somewhere?


pi@log:~ $ more /bin/rpi-wifi.sh
#!/bin/bash
echo 'Starting Wifi AP and STA client...'
/usr/sbin/ifdown --force wlan0
/usr/sbin/ifdown --force ap0
/usr/sbin/ifup ap0
/usr/sbin/ifup wlan0
/usr/sbin/sysctl -w net.ipv4.ip_forward=1
/usr/sbin/iptables -t nat -A POSTROUTING -s 10.0.0.0/24 ! -d 10.0.0.0/24 -j MASQUERADE
/usr/bin/systemctl restart dnsmasq
echo 'WPA Supplicant reconfigure in 5sec...'
/usr/bin/sleep 5
/usr/sbin/wpa_cli -i wlan0 reconfigure

**there is no /usr/sbin/iptables file.**
(this full path added in a github change on the source script a few weeks ago...)
- iptables tool is not installed.…


### installing iptables
pi@log:~ $ sudo apt-get install iptables



reboot…
- still can't get internet…
- I think it's got something to do with how I set up networking before I did ran the curl script (ie using raspi-config and the wpa_cli tool)
- not sure how to un-ring that bell, probably best to start from scratch again.…



### re-running ap script
```shell
curl https://raw.githubusercontent.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP/master/ap_sta_config.sh | sudo bash -s -- --ap log.local loglogloglog --client JJandJsKewlPad MyWifiPw --country CA --ip 10.0.0.202
```

edited 
sudo nano /etc/udev/rules.d/70-persistent-net.rules

looks like there was a missing quote at the end?
```
SUBSYSTEM=="ieee80211" \
, ACTION=="add|change" \
, ATTR{macaddress}=="b8:27:eb:5c:1f:0e", \
KERNEL=="phy0", \
RUN+="/sbin/iw phy phy0 interface add ap0 type __ap", \
RUN+="/bin/ip link set ap0 address b8:27:eb:5c:1f:0e"
```

– rebooting.…
– still no good; will start again from scratch…


# Dec 28
Starting over, using the instructions I'm maintaining and updating in the InstallationGuide.md file

[Journal notes here will cover any debugging or special cases]

Got through the "run the install ap/sta script" part, and when I log back in, I can't get to the internet.

## debugging

noting root cron line:
@reboot sleep 20 && /bin/bash /bin/rpi-wifi.sh >> /var/log/ap_sta_wifi/on_boot.log 2>&1


`
pi@raspberrypi:~ $ more /var/log/ap_sta_wifi/on_boot.log
wpa_supplicant: /sbin/wpa_supplicant daemon failed to start
run-parts: /etc/network/if-pre-up.d/wpasupplicant exited with return code 1
ifup: failed to bring up wlan0
`

wpa_supplicant not starting…

Ran
`journalctl -b` to see boot debugging messages

### clues
` wpa_supplicant[380]: nl80211: kernel reports: Match already configured`

related articles:
https://bbs.archlinux.org/viewtopic.php?id=170353
https://bbs.archlinux.org/viewtopic.php?id=257874 

pi@raspberrypi:~ $ more /var/log/ap_sta_wifi/ap0_mgnt.log
Check if hostapd.service is hang cause ap0 exist...
/bin/manage-ap0-iface.sh: line 6: service: command not found
ap0 interface does not exist, the faillure is elsewhere
Check if hostapd.service is hang cause ap0 exist…

Downloaded and ran an older version of the install script. It didn't work, but will try to reinstall fro scratch using this older version, which I'll keep in the log repo

Ran the older version, rebooted, and it worked!

So, I need to change the install instructions to clone my repo then run the script from my repo instead…


## More Testing
- there are issues around browsers and https, since this device doesn't use http
- Can I get https working on this?

## getting it connected to a screen and keyboard

- It connects to a screen and shows the login prompt, but doens't respond to the keyboard


## can I get it to make sound through HDMI?
added this line to /boot/config.txt
`hdmi\_force\_edid_audio=1`
should force hdmi audio
but no audio coming from my monitor. 
BUT, my screen is acting weird, so the fault could be there…









#  March 12 2022 : WiFiZomatic Network
Installation work for WifiZomatic Network
- Started with same install as Log

Installed drivers for adadfruit speak bonnet
https://learn.adafruit.com/adafruit-speaker-bonnet-for-raspberry-pi/raspberry-pi-usage 
`curl -sS https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/i2samp.sh | bash`

Downloading mp3s and converting them to wav files, using VLC on the Mac: https://www.vlchelp.com/convert-audio-format/
now can play music with aplay

free audio source:
https://freesound.org/browse/tags/sound-effects/ 

installing tshark, so we can try to watch the network and turn packets into sound as they pass through the system.
https://tshark.dev/setup/install/ 
https://tshark.dev/capture/tshark/ 

`sudo apt-get install tshark`

this command is interesting:
`sudo tshark -i wlan0 -l -x`
output like


0000  33 33 00 00 00 16 b8 27 eb e7 e6 e7 86 dd 60 00   33.....'......`.
0010  00 00 00 60 00 01 fe 80 00 00 00 00 00 00 3e 69   ...`..........>i
0020  54 90 b5 df a1 60 ff 02 00 00 00 00 00 00 00 00   T....`..........
0030  00 00 00 00 00 16 3a 00 05 02 00 00 01 00 8f 00   ......:.........
0040  fd 4c 00 00 00 04 04 00 00 00 ff 02 00 00 00 00   .L..............
0050  00 00 00 00 00 01 ff 00 3e 71 04 00 00 00 ff 02   ........>q......
0060  00 00 00 00 00 00 00 00 00 01 ff e7 9f a3 04 00   ................
0070  00 00 ff 02 00 00 00 00 00 00 00 00 00 00 00 00   ................
0080  00 fb 04 00 00 00 ff 02 00 00 00 00 00 00 00 00   ................
0090  00 01 ff df a1 60                                 .....`

could turn to music?
This command strips out everything but the hex numbers:
`cut -d\  -f 2-18`
`sudo tshark -i wlan0 -l -x |  cut -d\  -f 2-18 | sed 's/^ *//'`


needed to install bc:
` sudo apt-get install bc`

Experimenting with ways to generate sound…
`sudo apt-get install beep`
running this gives me error
beep: Error: Could not open any device

so trying sOx
https://linux.die.net/man/1/play
`sudo apt-get install sox`
now this makes an organ sound:
`play -n -c1 synth sin %-12 sin %-9 sin %-5 sin %-2 fade q 0.1 1 0.1`


Made a script that turns that stream of hex values into chords that play!
(probably a little slow...)

`sudo tshark -i wlan0 -l -x |  cut -d\  -f 2-18 | ./sharkToSound.sh`
`sudo tshark -i ap0 -l -x |  cut -d\  -f 2-18 | ./sharkToSound.sh`
or to just get the web page data
`sudo tshark -i ap0 -l -f "tcp port 80" -x |  cut -d\  -f 2-18 | ./sharkToSound.sh`
Other filters:
`-f "udp port 9003 or udp port 9002"`
`sudo tshark -i ap0 -l -f "udp port 9003 or udp port 9002" -x |  cut -d\  -f 2-18 | ./sharkToSound.sh`

# March 14

## running tshark as pi, not root
`sudo usermod -a -G wireshark $USER`
reboot

## turning network sound thing into one script
basically put some version of 
`sudo tshark -i ap0 -l -f "udp port 9003 or udp port 9002" -x |  cut -d\  -f 2-18 | ./sharkToSound.sh`
into a single script
created networksound_udp.sh - may also make a networksound_tcp, so it can sound different
- this script also plays a 3 second tone at startup, to confirm it's working

## configuring scripts to start at startup
https://www.baeldung.com/linux/run-script-on-startup
- going to try cron first
- this worked.

## launch fart sound from PHP...
/var/www/html/index.php is the file
needs to run 
`aplay /home/pi/WifiZomaticNetwork/audio/fs1.wav`

- struggling with permssions for www-data user to access sound
- added www-data to groups: audio i2c gpio
- `usermod -a -G audio www-data`
- `usermod -a -G i2c www-data`
- `usermod -a -G gpio www-data`

# March 16, 2022


## Change tones for tcp vs udp data
- created script sharkToSound_tcp.sh to play with different tones
- now have sharkToSound_tcp.sh and sharkToSound_udp.sh
- and full scripts networkSound_tcp.sh and networkSound_udp.sh, which get put in cron



# 2022/03/17 
 
## Test behaviour when not on home network
- basic connection from phone works

## make backup system for www folder
- `sudo cp -R /var/www/html/* ~/WifiZomaticNetwork/webserver/`
- then commit, or copy to laptop and rcync then commit
- `cd Documents/htdocs/WifiZomaticNetwork/ ; rsync -avz pi@log.local:/home/pi/WifiZomaticNetwork/webserver ./`
- `git pull; git  add --all ; git commit -m"new notes" ;  git push`

## change index.php for this app.…
- just with poop emoji to trigger fart sound
- add more fart sounds?
- display info about plant levels and other interactions? 
    - or keep the mystery…
    - 

## redirecting all traffic to the index.php page
- trying: to /etc/dnsmasq.conf, adding line:
`address=/#/10.0.0.203`
- which SHOULD redirect all traffic to the webserver (hrm, will this mess up the UDP communication?)
- that didn't work.

## improve connection from phone
- QR code for url
![b6d6add5b48da3785a9a75f0792be048.png](b6d6add5b48da3785a9a75f0792be048.png)
this seemed to help with some android phones. Maybe that's enough?

- https?
- popup/capture (that allows devices also)


## enababling https:
https://variax.wordpress.com/2017/03/18/adding-https-to-the-raspberry-pi-apache-web-server/comment-page-1/
- this is complicated, maybe don't need if the QR code helps....

- 




# March 28 2022
working on getting node code functioning as UDP relay between devices
in nodeserver/index.js

error 
if (e.code !== 'ERR_SOCKET_DGRAM_NOT_RUNNING') throw e;
TypeError: That Message Just Doesn't Seem Right
[lol!]
- maybe trying to send before socket is opened?
-  ah was sending with channel set to "undefined"

## Communication plan:
Star network:

// devices get messages from central server (the rhizomes), and send messages from central server
// central server takes a message and rebroadcasts to everyone except the device that sent it
- or should we be more rhizomatic, less cnetralized?
- that would require broadcasting all messages to all, which would make it harder to sonify?
- or, server is jsut another node, and everyone send their messages to everyone, but no-one re-broadcasts. This would be more robust, keeps working if server fails
- the Wifi is the rhizome. 


# March 29, 2022

## configure for auto-start index.js
installed forever
https://blog.logrocket.com/running-node-js-scripts-continuously-forever/
`npm install -g forever`

then you run 
/usr/local/bin/forever start /home/pi/WifiZomaticNetwork/nodeserver/index.js
 
 stop stop it's
 `forever stop X`
 where x is 
 - script name
 - process ID
 - index in the list from `forever list` (note, this command takes a while to return)
 - uid from list returned from `forever list`
 
 Then adding this line to cron:
 `@reboot /usr/local/bin/forever start /home/pi/WifiZomaticNetwork/nodeserver/index.js`
 
 
 
# March 30

## play fart sound from node
https://npmjs.com/package/aplay
`npm install aplay --save`
in code:
new Sound().play("/home/pi/WifiZomaticNetwork/audio/fs1.wav");
 
 
 
 
# April 6
Going to try to switch to MQTT protocol, to hopefully lose fewer packets and improve message delivery

need to install an MQTT server/broker on rpi

`sudo apt-get install mosquitto mosquitto-clients`
edit /etc/mosquitto/mosquito.conf, change last line to 
`
upgrade_outgoing_qos true
listener 1883
allow_anonymous true
`
then also run
`sudo ifconfig down wlan0`

so the device isn't connecting to the internet. This seems to help with the esp32s not dropping their subscription connections





 
 
## SQLLite for Nodejs
- so it can interact with the webserver
https://www.sqlitetutorial.net/sqlite-nodejs/

