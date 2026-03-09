# Linux Kernel Driver and MSP430 FR4133 FRAM Temperature System
## Project Overview

This repository contains the implementation of Assignment 2, which includes:

1. Linux Kernel Driver development and kernel recompilation

2. MSP430 FR4133 temperature sensing and FRAM data storage

## Part 1: Linux Kernel Driver

* Writing a simple Hello World kernel module

* Compiling using Makefile

* Loading and unloading module using insmod / rmmod

* Checking kernel log using dmesg

* Rebuilding Linux kernel with built-in module support


### Build Commands
```
make
sudo insmod hello.ko
dmesg | tail
sudo rmmod hello
```

## Part 2: MSP430 FR4133 Temperature and FRAM

* Reading internal temperature sensor using ADC12

* Storing temperature data into FRAM

* Recovering stored data after power loss

* LED threshold indication for overheating detection

### Development Environment

* Code Composer Studio (CCS)

* MSP430 FR4133 LaunchPad