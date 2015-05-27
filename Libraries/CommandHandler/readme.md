ArduinoCommand
=============
A Wiring/Arduino library to tokenize and parse commands through an arbitrary input.

This library has been modified from the SerialCommand library written by [Steven Cogswell](http://husks.wordpress.com), then modified by [Stefan Rado](https://github.com/kroimon/Arduino-SerialCommand).

Rather than pulling commands directly from the Serial interface, this modified library allows characters to be input manually from any source.
This modification allows the library to be used with other communication interfaces, such as byte buffers used in RF24 projects.
