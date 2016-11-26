SystemdGenie
============

SystemdGenie is a systemd management utility based on KDE technologies. 
It provides a graphical frontend for the systemd daemon, which allows for
viewing and controlling systemd units, logind sessions as well as easy 
modification of configuration and unit files.


Installation
------------
    mkdir build  
    cd build  
    cmake -DCMAKE_INSTALL_PREFIX=\`kf5-config --prefix\` ..  
    make  
    make install  


Dependencies
------------
*   Qt >= 5.4
*   KF5Auth
*   KF5CoreAddons
*   KF5Crash
*   KF5I18n
*   Systemd >= 209


Execution
---------
SystemdGenie can be accessed through the application menu or by issuing
the command `systemdgenie` from a terminal.


Developed by:
* Ragnar Thomsen <rthomsen6@gmail.com>


Source packages can be downloaded at:
http://download.kde.org/stable/systemdgenie/

Please report bugs and feature requests at:
https://bugs.kde.org/

Git repository can be found at:
http://cgit.kde.org/systemdgenie.git
