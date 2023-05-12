# QVISCA over IP

A GUI tool to control a PTZ camera compatible with VISCA over IP.

The development was tested using a Sony SRG-120 and only the commands
supported by this camera was added to the UI.


# The upstream project

This project is forked from an upstream project [nemo42/libViscaQtUi](https://github.com/nemo42/libViscaQtUi).
Without the upstream project, I cannot make this program.

Hopefully, the code in this project can be merged into the upstream project.

Major changes in this project will be as below.

- Support of VISCA-over-IP
- Multithreading to avoid freeze GUI
