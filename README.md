# USB-timing-fingerprint
This USB timing fingerprint authenticator is for Linux. 
## Introduction
To create timing fingerprint, we need to issue a series of read operations to the device and precisely record the device’s response latency.
## Build
### Prerequisites
Install usb package
```
sudo apt-get install libusb-dev
```

Install pcap library
```
sudo apt-get insatll libpcap-dev
```

Install udev library
```
sudo apt-get install libudev-dev
```
Install tensorflow package
```
pip install tensorflow
```
Enable usb module in the kernel
```
sudo modprobe usbmon
```

### Build Artifacts
To run the authenticator
```
bash run.sh
```
### Features
1. Create fingerprint for USB device
2. Fingerprint authentication
3. Packet filter

