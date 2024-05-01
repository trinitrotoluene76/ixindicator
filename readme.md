[script]:script_PC.py
[prez]:presentation.gif
# Description
iXindicator is a simple USB display for notification purpose. It's based on dongle T-display S3 (ESP32).
A control script (PC side) send requests to the dongle and the dongle display informations.
There are 5 modes:
* Chronometer (mode 1)
* PASSED/FAILED message (modes 4,5)
* PASSED/FAILED message with elapsed time (modes 4,5)
* watchdog for periodical message (mode 2)
* timeout counter for periodical message (mode 3)

![prez]
# Author:
Amaury Froment

# Repo/Branches
https://github.com/trinitrotoluene76/ixindicator.git
* master (stable)
* develop

# How to
* plug the device in any PC USB port available
* start a script like [script_PC.py][script] to send command to de device
* see the result on the device

# Hashtags
![](https://img.shields.io/badge/%23automation-blue) ![](https://img.shields.io/badge/%23notification-blue) ![](https://img.shields.io/badge/%23testing-blue) ![](https://img.shields.io/badge/%23monitoring%20app%20or%20server-blue) ![https://blink1.thingm.com/](https://img.shields.io/badge/%23blink(1)%20inspired-blue)
