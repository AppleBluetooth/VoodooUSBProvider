# VoodooUSBProvider

## Introduction
Developing a Qualcomm Bluetooth kernel extension for macOS recently, I realized how "terrible" the original bluetooth provider (IOUSBHostController) is - it has barely any compatibility for systems older than El Captain and many functions require wrapping up (AKA it is leaving too much space for developers, which is inconvenient). The purpose of this project is to provide an enhanced macOS bluetooth provider based on the apple one, in order to avoid certain trivial works being redone repeatedly.

## Requirements
To use this project, MacKernelSDK is needed as apple has removed a lot of code supporting older systems. See the acidanthera project for more details.

## How to use?
To use this project, copy the header and c++ files into your project directory. You could add this into your project's Header Search Paths via Xcode at your own preference. When you need to use these functions, include the header file. Instead of using IOUSBHostDevice *, IOUSBHostInterface *, and IOUSBHostPipe * to declare new USB devices, please use VoodooUSBDevice *, VoodooUSBInterface *, and VoodooUSBPipe *.

## TO-DO
1. Add more HCI commands </br>
2. Add more vendor requests </br>
3. Add documentations </br>

## Acknowledgements
[Apple](https://www.apple.com) for macOS </br>
[Acidanthera](https://www.github.com/acidanthera) for BrcmPatchRam and MacKernelSDK </br>
[cjiang](https://www.github.com/CharlieJiangXXX) for developing this project </br>
