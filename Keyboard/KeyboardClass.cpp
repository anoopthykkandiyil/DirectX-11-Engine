#include "KeyboardClass.h"

KeyboardClass::KeyboardClass() {
	for (int i = 0; i < 256; i++) {
		this->keyStates[i] = false; // Initialize all key states to off
	}
}

bool KeyboardClass::KeyIsPressed(const unsigned char keycode) {
	return this->keyStates[keycode];
}

bool KeyboardClass::KeyBufferIsEmpty() {
	return this->keyBuffer.empty();
}

bool KeyboardClass::CharBufferIsEmpty() {
	return this->charBuffer.empty();
}

KeyboardEvent KeyboardClass::ReadKey() {
	if (this->keyBuffer.empty()) { // if no key is to be read
		return KeyboardEvent(); // return empty keyboard event (Invalid keyboard event)
	}
	else {
		KeyboardEvent e = this->keyBuffer.front(); //get first keyboard event from queue
		this->keyBuffer.pop(); // remove first element from queue
		return e;
	}
}

unsigned char KeyboardClass::ReadChar() {
	if (this->charBuffer.empty()) {  // If no keys to be read
		return 0u; // return 0 or null character
	}
	else {
		unsigned char e = this->charBuffer.front(); // Get first character from queue
		this->charBuffer.pop(); // remove first char from queue
		return e; // return char
	}
}

void KeyboardClass::OnKeyPressed(const unsigned char key) {
	this->keyStates[key] = true;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void KeyboardClass::OnKeyReleased(const unsigned char key) {
	this->keyStates[key] = false;
	this->keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void KeyboardClass::OnChar(const unsigned char key) {
	this->charBuffer.push(key);
}

void KeyboardClass::EnableAutoRepeatKeys() {
	this->autoRepeatKeys = true;
}

void KeyboardClass::DisableAutoRepeatKeys() {
	this->autoRepeatKeys = false;
}

void KeyboardClass::EnableAutoRepeatChars() {
	this->autoRepeatChars = true;
}

void KeyboardClass::DisableAutoRepeatChars() {
	this->autoRepeatChars = false;
}

bool KeyboardClass::IsKeysAutoRepeat() {
	return this->autoRepeatKeys;
}

bool KeyboardClass::IsCharsAutoRepeat() {
	return this->autoRepeatChars;
}

