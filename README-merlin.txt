Asuswrt-Merlin - build 3.0.0.4.246.20 (14-Nov-2012)
===================================================

About
-----
Asuswrt is the firmware developped by Asus for their newer routers.  They are 
also porting it to some of their older models, like the RT-56U and RT-N16.  
While originally based on Tomato-RT, Asus has disabled some of the 
original Tomato features, and added others.

Asuswrt-merlin is a customized version, which I am developping.
The goal is to do some bugfixes and minor enhancements to Asus's firmware, 
without targeting at full-blown advanced featuresets such as provided by 
excellent projects like Tomato or DD-WRT.  Some of the features 
that had been disabled by Asus have also been re-enabled.
This aims to be a more restrained alternative for those who prefer to stay 
closer to the original firmware, with limited risks of seeing new 
features bring in new stability issues.  I value stability over 
performance, and performance over features.



Supported Devices
-----------------
Supported devices are:
 * RT-N66U and RT-N66R
 * RT-AC66U and RT-AC66R

These devices have experimental support (because I don't own one to test it):
 * RT-N16



Features
--------
Here is a list of features that Asuswrt-merlin brings over the original firmware:

- Based on the source code of release 3.0.0.4.246
- Various bugfixes (like the crash on VPN/NAT Loopback access of LAN devices)
- WakeOnLan web interface (with user-entered preset targets)
- Persistent JFFS partition
- User scripts that run on specific events
- SSHD (through dropbear)
- Crond
- Clicking on the MAC address of an unidentified client will do a lookup in
  the OUI database (ported from DD-WRT).
- Optionally save traffic stats to disk (USB or JFFS partition)
- Display monthly traffic reports
- Display active/tracked network connections
- Allows tweaking TCP/UDP connection tracking timeouts
- CIFS client support (for mounting remote SMB share on the router)
- Layer7 iptables matching
- User-defined options for WAN DHCP queries (required by some ISPs)
- Name field on the DHCP reservation list and Wireless ACL list
- Improved NAT loopback (based on code from phuzi0n from the DD-WRT forums)
- Dual WAN support (both failover and load 
  balancing supported) (EXPERIMENTAL) (RT-N66U, RT-AC66U)
- Disk spindown after user-configurable inactivity timeout
- System info summary page
- Wireless client IP, hostname, rate and rssi on the Wireless Log page
- OpenVPN client and server, based on code originally written by
  Keith Moyer for Tomato and reused with his 
  permission. (RT-N66U, RT-AC66U)
- Customized config files for router services
- LED control - put your Dark Knight in Stealth Mode by turning off all LEDs


A few features that first debuted in Asuswrt-Merlin have since been 
integrated/enabled in the official firmware:

- 64K NVRAM (RT-N66U)
- HTTPS
- Turning WPS button into a radio on/off toggle



Installation
------------
Simply flash it like any regular update.  You should not need to reset to 
factory defaults, unless coming from a version that used a different 
nvram size.  You can revert back to an original Asus firmware at any time just
by flashing one.

NOTE: If you were still running a 32KB nvram firmware on an RT-N66U (which 
usually mean an original firmware older than 3.0.0.4.220), the first time 
you flash a 64KB-enabled firmware (such as Asuswrt-merlin) it will 
wipe ALL your current settings and revert back to factory default!
This is required to upgrade the nvram storage to 64 KB.


Usage
-----

* JFFS *
JFFS is a writable section of the flash memory which will allow you to store small 
files (such as scripts) inside the router without needing to have a USB disk 
plugged in.  This space will survive reboot (but it *MIGHT NOT survive 
firmware flashing*, so back it up first before flashing!).  It will also be 
available fairly early at boot (before USB disks).

To enable this option, go to the Administration page, under the System tab.

First time you enable JFFS, it must be formatted.  This can be done through 
the web page, same page where you enable it.  Enabling/Disabling/Formating 
JFFS requires a reboot to take effect.

I do not recommend doing frequent writes to this area, as it will 
prematuraly wear out the flash storage.  This is a good place to 
put files that are written once like scripts or kernel modules, or 
that rarely get written to (like once a day).  Storing files that 
constantly get written to (like logfiles) is NOT recommended - use
a USB disk for that.



* User scripts *
These are shell scripts that you can create, and which will be run when 
certain events occur.  Those scripts must be saved in /jffs/scripts/ 
(so, JFFS must be enabled and formatted).  Available scripts:

- services-start:  Services are started (boot)
- services-stop:  Services are stopped (reboot)
- wan-start:  WAN interface just come up (includes if it went down and back up)
- firewall-start:  Firewall is started (filter rules have been applied)
- nat-start: nat rules (i.e. port forwards and such) have been applied (nat table)
- init-start:  Right after jffs is mounted, before any of the services get started
- pre-mount:  Just before a partition is mounted.  Be careful with 
  this script.  This is run in a blocking call and will block the mounting of the 
  partition  for which it is invoked till its execution is complete. This is done 
  so that it can be used for things like running e2fsck on the partition before 
  mounting. This script is also passed the device path being mounted as an 
  argument which can be used in the script using $1.
- post-mount:  Just after a partition is mounted
- unmount: Just before unmounting a partition.  This is a blocking script, so be
  careful with it.  The mount point is passed as an argument to the script.

Don't forget to set them as executable:

   chmod a+rx /jffs/scripts/*

And like any Linux script, they need to start with a shebang:

   #!/bin/sh



* WakeOnLan *
There's a WOL tab under the new Tools menu.  From there you can enter a
target computer's MAC address to send it a WakeOnLan packet.  You can also
create a list of MAC addresses that will be stored in nvram, and on
which you can click afterward to wake up one of the listed computers, without 
having to remember their MAC addresses.



* SSHD *
SSH support (through Dropbear) was re-enabled.  Password-based login will use 
the same username and password as telnet/web access.  You can also optionally 
insert a RSA public key there for keypair-based authentication.  There 
is also an option to make ssh access available over WAN.



* Crond *
Crond will automatically start at boot time.  You can 
put your cron tasks in /var/spool/cron/crontabs/ .  The file 
must be named "admin" as this is the name of the system user.
Note that this location resides in RAM, so you would have to 
put your cron script somewhere such as in the jffs partition, 
and at boot time copy it to /var/spool/cron/crontabs/ using 
an init-start user script.

A simple way to manage your cron jobs is through the
included "cru" command.  Just run "cru" to see the 
usage information.  You can then put your "cru" 
commands inside a user script to re-generate your cron jobs 
at boot time.


* Traffic history saving *
Under Tools -> Other Settings are options that will allow you 
to save your traffic history to disk, preserving it between 
router reboots (by default it is currently kept in RAM, 
so it will disappear when you reboot).

You can save it to a custom location (for 
example, "/jffs/" if you have jffs enabled), or 
/mnt/sda1/ if you have a USB disk plugged in.
Save frequency is also configurable - it is recommended 
to keep that frequency lower (for example, once a day) 
if you are saving to jffs, to reduce wearing out 
your flash memory.  Make sure not to forget the trailing 
slash ad the end of the path.

Also, a new "Monthly" page has been added to the Traffic 
Monitor pages.



* Display active connections *
There is a new tab under System Log called "Connections".
This page will list the currently tracked network connections.
You can enable name resolution for IPs on the Tools menu,
under "Other Settings".  Note that name resolution can 
slow down the loading of this page, especially if you have 
a lot of tracked connections (for instance while torrenting).



* Adjust TCP/IP connection tracking settings *
Under Tools -> Other Settings there are various parameters 
that lets you tweak the timeout values related to connection 
tracking for TCP and UDP connections.  You should be careful with 
those settings.  Most commonly, people will tweak the UDP timeout 
values to make them more VoIP-friendly, by using smaller timeouts.
Timeout values are in seconds.



* Mounting remote CIFS shares on the router *
You can mount remote SMB shares on your router.  The syntax will 
be something like this:

mount \\\\192.168.1.100\\ShareName /cifs1 -t cifs -o "username=User,password=Pass"

(backslashes must be doubled.)



* Dual WAN (EXPERIMENTAL) *
Asuswrt originally support using a USB 3G/4G modem as a 
failover Internet connection.  Dual WAN is the next step, also 
developped by Asus but left disabled so far in their official 
releases (probably because this is still work in progress).  

The first improvement over USB failover is that it works not only 
with USB but with other ethernet devices, which can be plugged 
on one of the LAN ports that you will select as the secondary WAN 
interface.  The second difference is that in addition to failover 
mode, Dual WAN also supports a load balancing mode, allowing 
you to share both connections at once.

Keep in mind that Dual WAN is still an EXPERIMENTAL and UNSUPPORTED 
feature, until the time Asus finishes developping and testing it.  
Some things are expected to not work properly.



* Disk Spindown when idle *
Jeff Gibbons's sd-idle-2.6 has been added to the firmware, allowing you 
to configure a timeout value (in seconds) on the Tools -> Other Settings 
page.  Plugged hard drives will stop spinning after being inactive 
for that specified period of time.  Note that services like Download Master 
might be generating background disk activity, preventing it from idling.



* OpenVPN (client and server) *
OpenVPN is an SSL-based VPN technology that is provided as a 
secure alternative to the PPTP VPN already included by Asus.
OpenVPN is far more secure and more flexible, however it is 
not as easy to configure, and requires the installation of 
a client software on your computer client.  The client 
can be obtained through this download page:

http://openvpn.net/index.php/open-source/downloads.html

Explaining the details of OpenVPN are beyond the scope of this 
documentation, and I am in no way an expert on OpenVPN.
Fortunately, there is a lot of available documentation and 
Howto guides out there.  I tried to stick to the same option 
descriptions as used by Tomato, so about any guide written 
for Tomato can easily be used to guide you on 
Asuswrt-Merlin.  For pointers, check the Wiki on the Asuswrt-Merlin 
Github repository.



* Customized config files *
You can append content to various configuration files that are 
created by the firmware, or even completely replace them with 
custom config files you have created.  Those config override 
files must be stored in /jffs/configs/.  To have a config 
file appended to the one created by the firmware, simply 
add ".add" at the end of the file listed below.  For 
example, /jffs/configs/dnsmasq.conf.add will be added at the 
end of the dnsmasq configuration file that is created by 
the firmware, while /jffs/configs/dnsmasq.conf would completely 
replace it.

Note that replacing a config file with your own implies that you 
properly fill in all the fields usually dynamically created by 
the firmware.  Since some of these entries require 
dynamic parameters, you might want to dynamically generate
the config file through an init-start script which 
could retrieve parameters from nvram, and insert them in 
your custom config.

The list of available config overrides:

* dnsmasq.conf
* vsftpd.conf
* pptpd.conf
* dhcp6s.conf
* hosts (for /etc/hosts)
* smb.conf
* minidlna.conf
* profile (shell profile, only profile.add suypported)
* upnp (for miniupnpd)
* radvd.conf



Source code
-----------
The source code with all my modifications can be found 
on Github, at:

https://github.com/RMerl/asuswrt-merlin



History
-------
3.0.0.4.246.20:
   - NEW: Wifi status icon will be half colored if only one radio is enabled.
   - NEW: Wifi status icon popup will report the state of each radios.
   - NEW: upnp custom config file for miniupnpd
   - NEW: unmount user script
   - NEW: led_ctrl and makemime (for use in conjunction with sendmail) applets.
   - NEW: Implemented control for network switch LEDs (all four at once)
   - NEW: Stealth Mode: option to disable all LEDs
   - NEW: Added CONFIG_IP_NF_RAW and CONFIG_NETFILTER_XT_TARGET_NOTRACK modules.
   - FIXED: Radio toggle through WPS button would be overriden by a scheduled
            radio.  Reverted "switch" to "toggle" code to prevent this.
   - FIXED: You couldn't disable DMZ by clearing the IP field.
   - FIXED: You couldn't edit entered text in DHCP/MAC/etc name field
   - FIXED: clientid passing for some ISPs requiring it (like Sky UK)
            was broken with the DHCP client change of build 220.
   - FIXED: No longer reboot the router three times during boot time if one 
            of the radios is disabled by the user. (RT-N66U)
   - FIXED: Changing the router login name to anything other than "admin"
            would prevent radvd, ecmh and the cru script from working 
            properly - they all assumed "admin".  Made then use
            http_username instead (which is tied to the superuser)
   - CHANGED: Improved SMB and vsftpd read performance by up to 30%


3.0.0.4.246.19b:
   - FIXED: Reverted wireless driver to build 220 version as the new 
            one caused various connection issues for some (RT-N66U).


3.0.0.4.246.19:
   - NEW: Rebased on 3.0.0.4.246.  Some notable changes:
            o New "Enhanced interference management" option under Wireless -> Professional.
            o Improved AiCloud webui
            o dnsmasq updated to 2.64

   - NEW: Option to enable simpler share names.  When enabled, the folder
          Share will be shared as "Share" instead of "Share (on sda1)".
          The option can be found on the Misc tab, under USB Application.
   - NEW: User customized config files for various services.  Those custom
          config entries can either be appended, or completely replace the 
          config file generated by the firmware.
   - NEW: Added Name field to the Wireless ACL page.
   - NEW: Added service applet to rc.  For example, "service restart_samba" will 
          restart the Samba service.  For advanced usage/debugging only.
   - NEW: Backported OpenSSL ASM optimization from 1.0.1, for significant performance
          improvements in applications such as OpenVPN or SSH when using AES.
   - NEW: Report the current CFE/Bootloader version on the Sysinfo page.
   - FIXED: Minor tweaks to the AiCloud pages so they can fit on a 15" laptop screen
            (some close buttons at the bottom were unreachable)
   - FIXED: Enabling SSH access from WAN didn't work if DualWAN
            was set to load-balancing.
   - FIXED: Removed MAC Filter page, as it doesn't work (not compatible
            with Parental Control).
   - FIXED: OpenVPN Client "Username Auth only" option was broken.
   - FIXED: Limit valid characters in a DHCP/WOL description to prevent 
            breaking the webui by using invalid ones such as quotes.
   - FIXED: OpenVPN Client wasn't properly applying DNS settings that
            the server was pushing to us.
   - FIXED: Wireless client list alignment in AP mode.
   - CHANGED: Less strict rules when validating user-entered MAC hwaddr.



3.0.0.4.220.18b:
   - NEW: Report both rx and tx rates on wifi connections
   - FIXED: Handle cases where the wireless driver returns a speed of -1
   - FIXED: Removed rssi retrieval retries, as it would make the first access to
            the wireless page take forever if you had multiple connected clients.
            You will have to manually refresh the page the first time you access it
            if the RSSI is reported as "??".


3.0.0.4.220.18:
   - NEW: Added OpenVPN logging verbosity setting (vpn_loglevel, must be
          manually set to a value between 0 and 15, with 3 being the default).
   - FIXED: Buffer overrun in init code that would crash the router when 
            too many features were enabled at compile time.
   - FIXED: Re-enabled DualWAN (RT-N66U, RT-AC66U)
   - FIXED: Re-enabled Beceem (Wimax) support in RT-AC66U.
   - FIXED: OpenVPN 'Start with WAN' and 'Respond to DNS' settings were 
            not properly saved.
   - FIXED: First time a client's rssi is polled it would return 0.
   - FIXED: post-mount user script wasn't executed (regression in 220.17)
   - CHANGED: Added some info to the OpenVPN server and client pages.
   - CHANGED: Improved load time of the VPN Status page.


3.0.0.4.220.17:
   - NEW: Rebased on 3.0.0.4.220, which includes:
            * Fixes to IPv6 6rd
            * Fixes to AC66U Wifi + QoS
            * AiCloud
            * Interference mode once again enabled
  - NEW: Display last received rate and rssi for each clients on Wireless Log page.
  - FIXED: dnsmasq not listening to DNS requests from OpenVPN clients
           if you had just enabled the option on the webui.
  - FIXED: PPTP clients not always showing on VPN Status page.
  - CHANGED: Disabled DualWAN as it's currently broken in 220.
  - CHANGED: Disabled Beceem Wimax support in RT-AC66U as it bricks
             the router.
  - CHANGED: Removed firmware update checker to avoid accidental
             revert to original FW.


3.0.0.3.178.16 Beta:
   - NEW: (RT-N66U, RT-AC66U) Implemented OpenVPN, based on code written by
          Keith Moyer (from the Tomato project).
   - NEW: Added crontab command
   - FIXED: (RT-AC66U) Would crash when accessing a LAN device through either 
            VPN or the NAT Loopback (GRO is now disabled for that device)
   - FIXED: dnsmasq was listening to all interfaces by default, allowing 
            even dhcp requests to be serviced from the wan side if you
            had the firewall disabled (Asus bug) (fixed by dev0id)
   - FIXED: Default disk idle spindown now set to 0 (disabled).
   - FIXED: Corrupted WOL list when using IE.
   - CHANGED: Upgraded openssl to 1.0.0j.
   - CHANGED: Included fully functional openssl command (will allow you to
              create keypairs and certificates from the router).
   - CHANGED: Removed power adjustments from the Performance page, as they
              are redundant, and not as reliable.
   - CHANGED: (RT-N16) Disabled Dual WAN, as it exhibited many issues, and I 
              am unable to work on them without an actual router.


3.0.0.3.178.15:
   - NEW: Rebased on 3.0.0.3.178.  Notable fixes by Asus:
           * Radio turns back on based on schedule
           * Reorganized QoS pages
           * Turning WAN DHCP connection off will first release current DHCP lease
   - NEW: RT-AC66U officialy supported, with all the same features as the RT-N66U.
   - NEW: (RT-AC66U) Implemented JFFS support.  Limiting partition to 32 MB
          max, as using the whole 90+ MB available makes little sense for 
          JFFS, and was also displaying some issues.
   - NEW: Added nat-start user script, as NAT rules get applied separately from
          other firewall rules (firewall-start changes to the nat table are 
          being overwritten when the router starts NAT)
   - NEW: Added additional info to Sysinfo page
   - NEW: Added chroot applet
   - NEW: Option to allow SSH access from WAN
   - NEW: Option to exclude specific devices from idle spindown
   - FIXED: Performance page now uses the new Sysinfo API, and is now able
            to deal with cases where radios are disabled.


3.0.0.3.162.14b:
   - FIXED: Web server would crash for some people when accessing
            the Wireless Log page.


3.0.0.3.162.14:
   - NEW: Spin down disks after (user-configurable) inactivity timeout
          (using Jeff Gibbons' sd-idle-2.6)
   - NEW: System information page under the Tools menu.
   - NEW: Station list on the Wireless Log page will now report associated
          IP and hostnames (when possible).
   - CHANGED: Upgraded to MiniDLNA 1.0.25 (changelog:
              http://sourceforge.net/projects/minidlna/files/minidlna/1.0.25/)
   - CHANGED: Better integration of the Run Cmd page.
   - FIXED: Incorrect left menu rendering when under the Tools menu.


3.0.0.3.162.13:
   - NEW: Rebased on 3.0.0.3.162.
   - CHANGED: Switched to WPS radio toggle code Asus added,
              now on the Administration -> System tab.


3.0.0.3.157.12 Beta:
This is based on unreleased Asus code, which they have 
graciously provided me with.

   - NEW: Rebased on 3.0.0.3.157.  Notable changes from Asus:
      . IPv6 tunnel memory leak fixed
      . They fixed many issues, making some of my patches 
        no longer necessary, such as timezone DST, https auth, etc...
      . Upgraded radvd
   - NEW: Added link to the command shell page in Tools menu.
   - NEW: (RT-N16) Enabled power settings (EXPERIMENTAL)
   - NEW: Added "tee" command.
   - FIXED: NAT loopback rules would actually NAT every lan to lan
            connections instead of only those needing the loopback
            (bug in Asus's code).  Replaced with new code based on a
            suggestion from Phuzi0n on the DD-WRT forums.
   - FIXED: Accessing the WOL page would make it resend the last
            WOL request.
   - FIXED: 'cru' was using 'root' instead of 'admin'
   - CHANGED: Re-enabled Dual WAN (EXPERIMENTAL)
   - CHANGED: Made tracked connections load async from rest of the page
   - CHANGED: Increased hostname width on Connection status page
   - CHANGED: Improved WOL page functionality.


3.0.0.3.144.11 Beta:
   - NEW: Name field added to DHCP reservation list
   - NEW: Webui option to enable resolving IPs on the Connections tab
   - NEW: Store a list of computer MACs to use as WOL targets
   - CHANGED: Increased dhcp options from 32 to 128 characters
   - FIXED: Brought max PPTPD password lenght back to 32 chars (Asus had reduced
     it to 16 in recent versions)
   - FIXED: Retrieve dhcpc options for the correct wan interface


3.0.0.3.144.10:
   - NEW: Rebased on 3.0.0.3.144.
   - NEW: Support for 64K NVRAM enabled.  ***First flash will
          wipe out ALL your settings!  And you cannot restore 
          from saved settings - you must manually reconfigure 
          everything.  Be warned!***
   - NEW: Enabled support for Broadcom Wimax devices
   - NEW: Added cifs kernel module (for mounting remote SMB shares)
   - NEW: Added layer7 iptables matching
   - NEW: Added user-options for DHCP on the WAN page
   - FIXED: Router crashing when connecting to it over Wifi
            and running the newer QoS code (disabled GRO)
   - FIXED: Router crashing when connecting to a network 
            device behind the router from over a VPN
            connection (disabled GRO).
   - FIXED: Incorrect timezone set unless enabling
            manual DST.


3.0.0.3.130.9:
   - NEW: Enabled new Dual WAN support from Asus
   - FIXED: no-ip DDNS entry would revert to Asus DDNS on webui


3.0.0.3.130.8:
*** Reverting to factory defaults BEFORE and AFTER flashing
this version is strongly recommended!  The newer Asus code base 
seems to have changed quite a few settings, so you'll want to 
not only start with the new default values, but also get rid 
of obsolete settings.  Otherwise you will be wasting a 
good amount of the limited available nvram. ***

   - KNOWN ISSUE: Memory leak when using IPv6 (bug in Asus's code 
                  and/or kernel code)

   - KNOWN ISSUE: PPTP VPN can randomly reboot the router if accessing 
                  a LAN device behind the router.  Workaround is to 
                  use an IP range outside of the local LAN
                  (i.e. 10.0.0.0 instead of 192.168.1.0), and either 
                  set your VPN to use the VPN tunnel as default 
                  gateway, or manually add a route to your VPN 
                  client.

   - NEW: Rebased patches on 3.0.0.4.130 (RT-N53U sources).
          Build 130 brings various code changes to IPv6, not sure 
          what else (as I have no changelog between 112 and 130).
          The QoS code remains from build 108, as build 130 is 
          unstable.
   - NEW: Added "diff" utility
   - NEW: Keyword-based filter (new in 130)
   - FIXED: Firmware/settings can now be uploaded over HTTPS
            (bug fixed by Asus)
   - FIXED: Buffer overflow in networkmap that would cause garbled 
            device names to appear on the clists list (bug in
            Asus's code)
   - FIXED: Firewall would break when applying a game preset that 
            had multiple ports separated by a "," (bug in Asus's
            code)
   - FIXED: WOL through webui wasn't working when IPv6 is enabled
   - FIXED: Memory leak in sit.ko (backported from Linux 2.6.25.3)
   - IMPROVED: /jffs/scripts/ will be created automatically if it
               doesn't exist (you must still make any new script 
               executable using "chmod a+rx script_filename")


3.0.0.3.108.7:
   - NEW: Added no-ip.com support to DDNS (patch submitted by Igor Pavlov)
   - NEW: Added webui page under System Log to display active/tracked
          network connections.
   - NEW: Added netstat-nat command.
   - NEW: Added pre-mount and post-mount user scripts (patch submitted by 
          Shantanu Goel)
   - NEW: Allows tweaking TCP/UDP connection tracking timeouts
   - FIXED: Removed check in Asus's code that would reject txpower > 80
            unless you clicked three times on Apply (?!).
            NOTE: Still not sure power setting even works, as I get
            -80db from the other end of the house no matter if I use 
            40 or 500 mW.


3.0.0.3.108.6:
   - NEW: HTTP access list (backported from build 112)
   - NEW: PPTP VPN encryption options (backported from build 112)
   - FIXED: Traffic history location was't properly saved
            when changed in webui.
   - FIXED: Disabled traffic history saving to nvram for now,
            to avoid people accidentally filling their limited nvram space.
   - FIXED: Missing bottom pixels from the bottom of General menu
   - FIXED: Removed invalid CSS attribute
   - FIXED: typo in VPN iptables entries (bug in Asus's code)


3.0.0.3.108.5:
   - NEW: Crond starts at boot time.
   - NEW: init-start is a new user script that will be run early on
          at boot time (right after jffs is mounted, and before any 
          service gets started)
  - NEW: Can save traffic history to a custom location (USB or 
         JFFS, for instance) to preserve it between reboots.
  - NEW: Added Monthly traffic page (ported from Tomato)
  - NEW: Added the Performance Tuning page (with temperature).
  - FIXED: Webui authentication was bypassed by the web server (bug in
           Asus's code)
  - FIXED: Httpd crash when uploading a FW or settings file over
           https - should simply fail now.  For now you have to 
           use http for flashing the FW or restoring your settings
           from a saved config file.


3.0.0.3.108.4:
   - NEW: Clicking on the MAC address of an unidentified client will do a lookup in
          the OUI database (ported from DD-WRT).
   - NEW: Added HTTPS access to web interface (configurable under Administration)
   - NEW: Option to turn the WPS button into a radio on/off toggle (under Administration)
   - FIXED: sshd would start even if disabled
   - CHANGE: Switched back to wol, as people report better compatibility with it.
             ether-wake remains available over Telnet.


3.0.0.3.108.3:
   - NEW: JFFS support (mounted under /jffs)
   - NEW: services-start, services-stop, wan-start and firewall-start user scripts,
          must be located in /jffs/scripts/ .
   - NEW: SSHD support
   - IMPROVED: Fleshed out this documentation, updated Contact info with SNB forum URL
   - CHANGE: Removed wol binary, and switched to ether-wake (from busybox) instead.
   - CHANGE: Added "Merlin build" next to the firmware version on web interface.
   
          
3.0.0.3.108.2:
   - NEW: Added WakeOnLan web page

   
3.0.0.3.108.1:
   - Initial release.
   
   
Contact information
-------------------
SmallNetBuilder forums (preferred method: http://forums.smallnetbuilder.com/showthread.php?t=7047 as RMerlin)
Asus Forums (http://vip.asus.com/forum/topic.aspx?board_id=11&model=RT-N66U%20(VER.B1)&SLanguage=en-us) as RMerlin.
Website: http://www.lostrealm.ca/
Github: https://github.com/RMerl/asuswrt-merlin
Email: rmerl@lostrealm.ca
Twitter: https://twitter.com/RMerlinDev

Development news will be posted on Twitter.  You can also keep a closer eye 
on development as it happens through the Github site.

For support questions, please use the SmallNetBuilder forums whenever possible.  There's a 
dedicated Asuswrt-Merlin sub-forum there, under the Asus Wireless section.

Drop me a note if you are using this firmware and are enjoying it.  If you really like it and want 
to give more than a simple "Thank you", there is also a Paypal donation button on my website.

I want to give my special thanks to Asus for showing an interest in this project, 
and also providing me with support when needed.  Also, thank you everyone who has 
donated through Paypal.  Much appreciated!



--- 
Eric Sauvageau
