/**
* SerialCommand - A Wiring/Arduino library to tokenize and parse commands
* received over a serial port.
*
* Copyright (C) 2015 Karl Mohring
* Copyright (C) 2012 Stefan Rado
* Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
*                    http://husks.wordpress.com
*
* Version 20120522
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ArduinoCommand_h
#define ArduinoCommand_h

#include <Arduino.h>

// Size of the command list buffer (maximum number of commands that can be stored)
#define COMMAND_LIST_SIZE 12

const char EMPTY = 0;
const char NOT_FOUND = -1;

class CommandHandler {
	public:
	CommandHandler(char* cache, int cacheSize);      // Constructor
	
	// Clear the command list
	void clearCommandList();
	
	// Remove a command from the command list
	void removeCommand(char command);
	
	// Add a command to the command list
	void addCommand(char command, void(*function)());
	
	// Set the handler to call when an invalid command is received
	void setDefaultHandler(void (*function)(char));  
	
	// Main entry point for the function - Pass in a character to be processed
	void readIn(char inChar);
	
	// Clear the input cache
	void clearCache();
	
	// Get the next character out of the input cache - used to get arguments
	char next();
	
	// Set the cache termination character that triggers processing
	void setTerminator(char terminator);

	// Check the serial buffer for commands. Serial must be configured beforehand
	void checkSerial();

	private:
	// Find a slot in the command list not already taken
	int findFreeCommandSlot();
	
	// Run through the input cache and process any valid commands
	void processCommands();
	
	// Find the slot number of a command in the command list
	int findCommand(char command);
	
	// Read the next character in from the cache
	char readCache();    
	
	// Write a character into the cache
	void writeCache(char in);
	
	// Determine if there are any characters to be read from the cache
	bool isCacheAvailable();
		
	// Command/handler dictionary
	struct ArduinoCommandCallback {
		char command;
		void (*function)();
	};

	// Pointer to the default handler function
	void (*defaultHandler)(const char);

	char _terminator;
	char* _inputCache;
	int _cacheSize;

	int _putter;
	int _getter;

	ArduinoCommandCallback _commandList[COMMAND_LIST_SIZE];	
};

#endif //SerialCommand_h
