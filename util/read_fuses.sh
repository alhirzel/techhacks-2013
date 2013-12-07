sudo avrdude -p m328p -F -P usb -c dragon_isp -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i
