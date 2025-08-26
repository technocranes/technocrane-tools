# Technocrane Tools
 Tools that help to emulate data packets or analyze incoming packets data
The binaries could be found in Release section of the repository.

[![Downloads](https://img.shields.io/github/downloads/technocranes/technocrane-tools/1.0/total.svg)](https://github.com/technocranes/technocrane-tools/releases/tag/1.0)
[![Main workflow](https://github.com/technocranes/technocrane-tools/actions/workflows/ccpp.yml/badge.svg)](https://github.com/technocranes/technocrane-tools/actions/workflows/ccpp.yml)

 Main workflow is building Windows release version of data display and data generator after each submit. You can find binaries in articasts list inside an actions workflow page.
 Also binaries are presented in Release tab of the repository.

NOTE! Technocrane communication has two options: serial port and network connection.

## Data Display

 This is a tiny COM trace application that could help to see what data is receving on a specified com port.

Usage example from command line
datadisplay.exe -d COM1

To display a network incoming packets use -n argument.
For a detailed description please run the application with argument -h

## Data Generator

 This is a console application that could be used to stream procedural generated or raw cgi file data packets.

Usage example to stream packets from *.cgi file
datagenerator.exe --open "D:\\Work\\Technocrane\\Result\\test-cgi.cgi" --timecode

To stream with network connection, you can use argument -n
For a detailed description please run the application with argument -h

 In case of serial communication by default the generator application will try to stream into COM1 port. The port must be open for writing. If you want to choose another port, use --device argument. Here is an example of additional arguments --device "COM4"

## CGI/FBX Convertion

 This is a Windows console application that can help to convert between cgi/fbx into cgi/fbx for different rotation order and file version (for example, new fbx into old version of fbx, etc.)
 Application is presented in a Release tab of the repository.
 
 Please use -h argument to get details about application arguments

List of arguments
 * -r  --frame_rate <double>      Specify a file frame rate
 * -f --file <file_name> 	File Name to Convert (cgi or fbx)
 * -o --output <file_name> File Name to Save A Convertion Into
 * -v --version <fbx format desc> Specify a fbx save format
 * --rotation_order <order desc> Specify a rotation order for fbx export (XYZ, XZY)

## Sources

  Sources are designed to be compiled under Windows and Unix platforms.
  There are VS 2017/2019 project files to compile application for Windows and makefiles for linux and windows mingw. Original applications has been made for Unix system and to make them work on Windows I added some open source analog libraries.

# Contact

  Please post issues and feature requests to this [github repository issues section](https://github.com/technocranes/technocrane-unreal/issues)
