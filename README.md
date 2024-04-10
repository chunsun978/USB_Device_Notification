# USB_Device_Notification
--------------------------------
The original code is from 
Embedded-Programming-with-Modern-CPP-Cookbook  

Built and tested in Raspberry PI 4,
and it works well.   

How to build:  
cd build
cmade ../src  
make  


If libudev.h can not be found, 
$sudo apt-get install libudev-dev  

run: 
./usb 

The printout:  

$ ./usb
Check [:] action: unbind  
Check [:] action: remove  
Check [:] action: unbind  
Check [:] action: remove  
Check [1f75:0918] action: add  
Check [:] action: add  
Check [:] action: bind  
Check [1f75:0918] action: bind  
STORAGE DEVICE connected, uses up to 96mA  
