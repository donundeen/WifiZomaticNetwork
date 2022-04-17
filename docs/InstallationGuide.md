# Notes:
commands that you should type in are shown in backticks, `like this`
DON'T include the backticks in the command when you type it in.

# Setting up SD Card

use an Micro SD card with at least 8GB

## Burning SD Card
new image installed using Raspberry Pi Imager. Settings:
    - Rasbian Lite OS
    - command-shift-X for advanced options
        - auto-fill wifi from keychain
        - set hostname raspberrypi.local
            - it will get changed to log.local later.
        - enable SSH
        - password for pi user: my usual makerspace pw
        - Wifi Country: CA
        - Locale Settings
            - Timezone: Canada/Eastern
        - Skip first-run wizard: yes
    - Write to Disk (takes some time)
- Remove SD card, re-insert
- 
## Edits on SD card to support USB connect
- open terminal window on laptop
- right click on a file in the SD card, select "open iterm here"
- in boot folder, type `touch ssh` to create empty file called ssh
- edit config.txt, adding     dtoverlay=dwc2 at the end, under [all]
- Now, edit the file called `cmdline.txt`. Look for `rootwait`, and add `modules-load=dwc2,g_ether` immediately after. (with a space after rootwait and after g_ether)


# First run in RPi


## connecting to the pi
inserted micro usb from laptop to Pi, using INNER usb port on pi

this takes a while…

Open a terminal window on laptop

try
`ssh pi@raspberrypi.local`
- once that connects, it means the pi is ready

you may get an error about The ECDSA host key for raspberrypi.local has changed. This happens if you are setting up multiple devices with the same hostname, or RE-intializing the same device.
- go into file ~/.ssh/known_hosts and delete the line with raspberrypi.local (and the one with log.local for good measure)

## test internet

run command
`ping google.com`
you should get results back.

## updates
run commands: 
`sudo apt-get update 
sudo apt-get upgrade`
- this takes a while.


# clone the log repo

`sudo apt-get install git`

## set up key stuff
https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent

`
ssh-keygen -t ed25519 -C "donundeen@google.com"
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519
cat ~/.ssh/id_ed25519.pub
    # Then select and copy the contents of the id_ed25519.pub file
    # displayed in the terminal to your clipboard
`

https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account

## clone the repo
`
git clone git@github.com:donundeen/loginlog.git
cd loginlog
chmod a+x ap_sta_config.sh
`

# setting up AP/STA mode

## install hostapd and dnsmasq:
`
sudo apt-get install hostapd
sudo apt-get install dnsmasq
`

Then turn them off:
`    
sudo systemctl stop hostapd
sudo systemctl stop dnsmasq
`

## install iptables

`sudo apt-get install iptables`

## run the install ap/sta script

from: https://github.com/MkLHX/AP_STA_RPI_SAME_WIFI_CHIP
- it's not perfect, and it's being maintained and updated, so results might vary from time to time
- Note: we're using the script from our own repo, so it's stable.

```shell
cd ~/loginlog
sudo ./ap_sta_config.sh --ap log.local loglogloglog --client JJandJsKewlPad WeL0veLettuce --country CA --ip 10.0.0.203
```

- the details of the above part might need to be customized to your situation, eg
    - change the client ssid and pw depending on the network you want the device to be able to connect to to reach the interst
    - the --ip value : maybe that need to be changed for diferent devices?

`sudo reboot`

## Test

if it worked, you should be able to see the wifi SSID log.local, connect via usb, and connect to the internet


# remove pw for the AP
- edit /etc/hostapd/hostapd.conf
    `sudo nano /etc/hostapd/hostapd.conf`
- comment out all lines that start with wpa_ or rsn_


# Change hostname to 'log'
https://www.tomshardware.com/how-to/raspberry-pi-change-hostname
`sudo nano /etc/hosts`
- change line `127.0.1.1       raspberrypi`
- to `127.0.1.1       log`

`sudo nano /etc/hostname`
- change `raspberrypi` to `log`

## Test

`sudo reboot`

Now can `ssh pi@log.local`

and can get on SSID log with no pw


# Set up Webserver

Instal Apache2:
`sudo apt-get install apache2` 

## test
- when you're connected to log.local, and go to web page: http://log.local/ and see the default apache webpage
- this should work when connected via USB, or when connected to the log SSID


# Install PHP

`sudo apt-get install php`

create file
`sudo nano /var/www/html/index.php` 
with contents:
`<?php phpinfo(); ?>`

## Test 
go to:
http://log.local/index.php
and see all the details of the php installation.

get rid of /var/www/html/index.html, so /var/www/html/index.php is the default
`sudo mv /var/www/html/index.html /var/www/html/index.html.bak`


# Install SQLite3 for php
`sudo apt-get install php-sqlite3`
reboot


# fix some permissions for pi and apache
Add pi to www-data group
`sudo usermod -a -G www-data pi` 
make sure www-date owns everything in /var/www/html
`sudo chown -R -f www-data:www-data /var/www/html`

# set up log app
copy www files from repo folder

```shell
cd ~/loginlog 
sudo cp ./var/www/html/index.php /var/www/html/index.php
sudo cp ./var/www/html/log.db /var/www/html/log.db
sudo mkdir /var/www/html/logmin
sudo cp ./var/www/html/logmin/index.php /var/www/html/logmin/index.php
```

reboot

`sudo reboot`

## test
go to http://log.local/index.php, confirm the log app is there
