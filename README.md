INEPLEM
=======

**INEPLEM** for *Italc Ne sEra PLus dE ce Monde* (Yes, I know this is not a valid acronym...) which means in French *iTalc will no longer be of this world* is a temporary Windows service installable on the student's computer to exploit the teacher's iTalc.
This repository is mainly in French but translation will be done progressively.

Features
--------

- Installs Windows service via bootable Alpine Linux drive to bypass Administrator requirement
- Keep iTalc student service closed
- Broadcasts custom image to teacher's iTalc (PNG or GIF)
- Minimal support of animated GIF
- Broadcasts custom username to teacher's iTalc
- *Ghost mode* for looking like a turned off computer
- Windows Firewall bypass
- Prevent remote access
- Captures teacher's credentials when ACL authentification is used
- Automatic cleaning at computer shutdown
- Allows launching custom processes as `NT AUTHORITY\SYSTEM`
- Free spelling errors, grammar errors, etc. (in French and in English)

Building
--------

For building INEPLEM you need to build the `windows-service` before the `linux-loader`.
Read the `README.md` file in respective folders to get builds instructions.
