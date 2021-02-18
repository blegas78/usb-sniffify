# usb-sniffify
Raspberry Pi 4 USB Forwarder, Sniffer, Injector

This is example code that can be used to open USB device and duplicate messages one the USB peripheral/power port on a Raspebrry Pi 4.  This framework lets a host device recognize the Raspberry Pi as the client device connected to the Raspberry Pi.  Doing so allows an arbitrary host send and recieve all protocols while allowing usb-sniffiy to read the transmitted messages.  A few small tweaks to the code can also let you block transmissions or inject new messages.  Sniffiy is derived from Sniff + Modify.

 - Gadget Handling: raw-gadget 
 - Device Handling: libusb-1.0
 
 The original motivation of this project is to decode USB protcol from a Dualsense controller connected to a PS5 over USB, as well as modify the inputs being sent to the console for projects like chaos%.
 
 There exist some generated headers based on HID report descriptors using the following tool:
  - hidrdd
 
 The passthrough.c is the latest program that can pass a device from the PCB-closest USB 2.0 port.  This requires running the insmod.sh to have been priorly run from raw-gadget.
 
	$ cd src 
	$ make
	$ ls /sys/class/udc
	fe980000.usb
	$ sudo ./passthrough fe980000.usb fe980000.usb

 This project is very new so expect frequent updates.  Though the intent is to pass through all USB funcitonality certain limitations in raw_gadget or libusb may make perfect transparency impossible.
 
 Status:
  - passthrough.c works with dualsense and DS4 controllers, audio can be passed to the controller
  - Audio cannot be passed to the host (mic audio -> console)
 
 TODO:
  - bulk trasnfer endpoint threads
  - isochronous device -> host thread
  - Turn into a library with better interfaces
