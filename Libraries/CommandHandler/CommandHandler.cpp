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
#include "CommandHandler.h"

//////////////////////////////////////////////////////////////////////////
// Constructor(s)

/**
* Constructor makes sure some things are set.
*/
CommandHandler::CommandHandler(char* cache, int cacheSize){
	_inputCache = cache;
	_cacheSize = cacheSize;

	_terminator = '$';

	clearCommandList();
	clearCache();
}


//////////////////////////////////////////////////////////////////////////
// Public methods

/**
/* Remove all commands from the list
*/
void CommandHandler::clearCommandList(){
	for (int i = 0; i < COMMAND_LIST_SIZE; i++){
		_commandList[i].command = 0;
		_commandList[i].function = 0;
	}
}


/**
* Remove command from the command list, effectively disabling it 
*/
void CommandHandler::removeCommand(char command){
	int commandSlot = findCommand(command);

	if(commandSlot >= 0){
		_commandList[commandSlot].command = 0;
		_commandList[commandSlot].function = 0;
	}
}


/**
* Adds a "command" and a handler function to the list of available commands.
* This is used for matching a found token in the buffer, and gives the pointer
* to the handler function to deal with it.
*/
void CommandHandler::addCommand(const char command, void (*function)()) {
	// Only write the command if it doesn't already exist
	if (findCommand(command) == NOT_FOUND){	
		int freeSlot = findFreeCommandSlot();

		if (freeSlot >= 0 && command != EMPTY){
			_commandList[freeSlot].command = command;
			_commandList[freeSlot].function = function;
		}
	}
}


/**
* This sets up a handler to be called in the event that the received command string
* isn't in the list of commands.
*/
void CommandHandler::setDefaultHandler(void (*function)(const char)) {
	defaultHandler = function;
}


/**
* Read in a character to be read by the command handler
* A terminator character prompts the buffer to be processed
*/
void CommandHandler::readIn(char inChar) {
	
	if (inChar == _terminator){     // Check for the terminator (default '\r') meaning end of command
		processCommands();
	}

	else{
		writeCache(inChar);
	}
}


/**
* Retrieve the next token byte from the command buffer.
* Used to grab arguments for commands
*/
char CommandHandler::next() {
	char next = 0;

	if (isCacheAvailable()){
		next = readCache();
	}

	return next;
}


/**
* Set the termination character
*/
void CommandHandler::setTerminator(char terminator){
	_terminator = terminator;
}


//////////////////////////////////////////////////////////////////////////
// Private Methods

/**
/* Find the first free slot in the command list
*/
int CommandHandler::findFreeCommandSlot(){
	return findCommand(EMPTY);
}


/**
* Process the buffer to find commands
*/
void CommandHandler::processCommands(){
	while (isCacheAvailable()){
		char command = readCache();
		int commandSlot = findCommand(command);
	
		// Command found; do the thing
		if (commandSlot >= 0){
			_commandList[commandSlot].function();
			
		}
		// Command not found; do the default thing
		else{
			(*defaultHandler)(command);
		}
	}
	clearCache();
}


/**
* Find the address of a given command
* Returns a -1 if the command is not found
*/
int CommandHandler::findCommand(char command){
	bool found = false;
	int slot = -1;

	for (int i = 0; i < COMMAND_LIST_SIZE && !found; i++){
		if (_commandList[i].command == command){
			slot = i;
			found = true;
		}
	}

	return slot;
}


/*
* Clear the input buffer.
*/
void CommandHandler::clearCache() {
	_putter = 0;
	_getter = 0;

}


/**
* Determine if there are characters to be read from the buffer
*/
bool CommandHandler::isCacheAvailable(){
	bool available = false;

	if (_getter < _putter){
		available = true;
	}
	
	return available;
}


/**
* Write a character to the cache (if there's room)
*/
void CommandHandler::writeCache(char in){
	if (_putter < _cacheSize){
		_inputCache[_putter] = in;
		_putter++;
	}
}


/**
* Read a character from the cache (if available)
*/
char CommandHandler::readCache(){
	char out;

	if (_getter < _putter){
		out = _inputCache[_getter];
		_getter++;
	}

	else{
		out = 0;
	}

	return out;
}

void CommandHandler::checkSerial(){
	while (Serial.available()){
		char inChar = Serial.read();
		readIn(inChar);
	}
}
