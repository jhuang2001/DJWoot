# How to bind keyboard to WSL from Windows

In powershell run:
```
usbipd list
```

Get the bus-id for the device with VID 31e3 (Wooting VID)

Run:
```
sudo usbipd bind --busid <id>
sudo usbipd attach --wsl --busid <id>
```

This will disconnect the device from windows and attach it to WSL

To reconnect device to Windows and remove it from WSL, run the reverse:
```
sudo usbipd detach --wsl --busid <id>
sudo usbipd unbind --busid <id>
```