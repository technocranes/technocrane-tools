# Technocrane Tools
 Tools that help to emulate data packets or analyze incoming packets data
The binaries could be found in Release section of the repository.

<p align="left">
  <a href="https://github.com/technocranes/technocrane-tools/Projects"><img alt="GitHub Actions status" src="https://github.com/technocranes/technocrane-tools/workflows/Main%20workflow/badge.svg"></a>
</p>
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

## Sources

  Sources are designed to be compiled under Windows and Unit systems.
  There are VS 2017/2019 project files to compile application for Windows. Original applications has been made for Unix system and to make them work on Windows I added some open source analog libraries.

# Contact

  Please post issues and feature requests to this [github repository issues section](https://github.com/technocranes/technocrane-unreal/issues)
