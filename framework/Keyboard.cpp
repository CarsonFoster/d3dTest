#include "Keyboard.h"
#include <bitset>
#include <optional>
#include <queue>

Keyboard::Keyboard() : m_keyStates{}, m_characterBuffer{}, m_autorepeat{ true } {}

bool Keyboard::isKeyPressed(unsigned char key) {
	if (key <= 0 || key >= VIRTUAL_KEYS) return false;
	return m_keyStates[key];
}

bool Keyboard::isEventQueueEmpty() const noexcept {
	return m_keyEvents.empty();
}

std::optional<Keyboard::Event> Keyboard::pollEventQueue() {
	if (!m_keyEvents.empty()) {
		Event e = m_keyEvents.front();
		m_keyEvents.pop();
		return e;
	} else return {};
}

void Keyboard::clearEventQueue() {
	// although the underlying std::deque does have a clear()
	// operation, it's the same complexity to just reassign,
	// and now we can keep std::queue and abstract away the implementation
	m_keyEvents = std::queue<Event>{};
}

bool Keyboard::isCharQueueEmpty() const noexcept {
	return m_characterBuffer.empty();
}

std::optional<unsigned char> Keyboard::pollCharQueue() {
	if (!m_characterBuffer.empty()) {
		unsigned char c = m_characterBuffer.front();
		m_characterBuffer.pop();
		return c;
	} else return {};
}

void Keyboard::clearCharQueue() {
	// although the underlying std::deque does have a clear()
	// operation, it's the same complexity to just reassign,
	// and now we can keep std::queue and abstract away the implementation
	m_characterBuffer = std::queue<unsigned char>{};
}

void Keyboard::enableAutorepeat() noexcept {
	m_autorepeat = true;
}

void Keyboard::disableAutorepeat() noexcept {
	m_autorepeat = false;
}

bool Keyboard::isAutorepeatEnabled() const noexcept {
	return m_autorepeat;
}

inline void Keyboard::manageEventQueueSize() {
	// we call this everytime we insert a new event,
	// so the only time we need to make room is if
	// the size is equal to the max size
	if (m_keyEvents.size() == MAX_QUEUE_SIZE)
		m_keyEvents.pop();
}

inline void Keyboard::manageCharQueueSize() {
	// we call this everytime we insert a new char,
	// so the only time we need to make room is if
	// the size is equal to the max size
	if (m_characterBuffer.size() == MAX_QUEUE_SIZE)
		m_characterBuffer.pop();
}

void Keyboard::keyPressed(unsigned char key) {
	if (key >= 0 && key < VIRTUAL_KEYS) {
		manageEventQueueSize();
		m_keyStates[key] = true;
		m_keyEvents.emplace(key, Event::Type::PRESSED);
	}
}

void Keyboard::keyReleased(unsigned char key) {
	if (key >= 0 && key < VIRTUAL_KEYS) {
		manageEventQueueSize();
		m_keyStates[key] = false;
		m_keyEvents.emplace(key, Event::Type::RELEASED);
	}
}

void Keyboard::characterTyped(unsigned char character) {
	// no arg checking, only available to Window
	manageCharQueueSize();
	m_characterBuffer.push(character);
}

void Keyboard::clearKeyStates() {
	m_keyStates.reset();
}