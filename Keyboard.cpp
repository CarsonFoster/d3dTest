#include "Keyboard.h"
#include <bitset>
#include <optional>
#include <queue>

Keyboard::Keyboard() : keyStates{}, characterBuffer{}, autorepeat{ true } {}

bool Keyboard::isKeyPressed(unsigned char key) {
	if (key <= 0 || key >= VIRTUAL_KEYS) return false;
	return keyStates[key];
}

bool Keyboard::isEventQueueEmpty() const noexcept {
	return keyEvents.empty();
}

std::optional<Keyboard::Event> Keyboard::pollEventQueue() {
	if (!keyEvents.empty()) {
		Event e = keyEvents.front();
		keyEvents.pop();
		return e;
	} else return {};
}

void Keyboard::clearEventQueue() {
	// although the underlying std::deque does have a clear()
	// operation, it's the same complexity to just reassign,
	// and now we can keep std::queue and abstract away the implementation
	keyEvents = std::queue<Event>{};
}

bool Keyboard::isCharQueueEmpty() const noexcept {
	return characterBuffer.empty();
}

std::optional<unsigned char> Keyboard::pollCharQueue() {
	if (!characterBuffer.empty()) {
		unsigned char c = characterBuffer.front();
		characterBuffer.pop();
		return c;
	} else return {};
}

void Keyboard::clearCharQueue() {
	// although the underlying std::deque does have a clear()
	// operation, it's the same complexity to just reassign,
	// and now we can keep std::queue and abstract away the implementation
	characterBuffer = std::queue<unsigned char>{};
}

void Keyboard::enableAutorepeat() noexcept {
	autorepeat = true;
}

void Keyboard::disableAutorepeat() noexcept {
	autorepeat = false;
}

bool Keyboard::isAutorepeatEnabled() const noexcept {
	return autorepeat;
}

inline void Keyboard::manageEventQueueSize() {
	// we call this everytime we insert a new event,
	// so the only time we need to make room is if
	// the size is equal to the max size
	if (keyEvents.size() == MAX_QUEUE_SIZE)
		keyEvents.pop();
}

inline void Keyboard::manageCharQueueSize() {
	// we call this everytime we insert a new char,
	// so the only time we need to make room is if
	// the size is equal to the max size
	if (characterBuffer.size() == MAX_QUEUE_SIZE)
		characterBuffer.pop();
}

void Keyboard::keyPressed(unsigned char key) {
	if (key >= 0 && key < VIRTUAL_KEYS) {
		manageEventQueueSize();
		keyStates[key] = true;
		keyEvents.emplace(key, Event::Type::PRESSED);
	}
}

void Keyboard::keyReleased(unsigned char key) {
	if (key >= 0 && key < VIRTUAL_KEYS) {
		manageEventQueueSize();
		keyStates[key] = false;
		keyEvents.emplace(key, Event::Type::RELEASED);
	}
}

void Keyboard::characterTyped(unsigned char character) {
	// no arg checking, only available to Window
	manageCharQueueSize();
	characterBuffer.push(character);
}

void Keyboard::clearKeyStates() {
	keyStates.reset();
}