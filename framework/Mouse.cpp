#include "CwfException.h"
#include "Mouse.h"
#include <optional>
#include <queue>
#include <utility>
#include <Windows.h>

Mouse::Mouse() noexcept : m_leftPressed{ false }, m_middlePressed{ false }, 
	m_rightPressed{ false }, m_inClientRegion{ false }, m_x{ 0 }, m_y{ 0 },
	m_wheelDeltaAccumulator{ 0 }, m_eventQueue{} {}

int Mouse::getX() const noexcept {
	return m_x;
}

int Mouse::getY() const noexcept {
	return m_y;
}

std::pair<int, int> Mouse::getPos() const {
	return std::make_pair(m_x, m_y);
}

bool Mouse::isLeftPressed() const noexcept {
	return m_leftPressed;
}

bool Mouse::isMiddlePressed() const noexcept {
	return m_middlePressed;
}

bool Mouse::isRightPressed() const noexcept {
	return m_rightPressed;
}

bool Mouse::isInClientRegion() const noexcept {
	return m_inClientRegion;
}

std::optional<Mouse::Event> Mouse::pollEventQueue() {
	if (!m_eventQueue.empty()) {
		Mouse::Event e = m_eventQueue.front();
		m_eventQueue.pop();
		return e;
	}
	return {};
}

bool Mouse::isEventQueueEmpty() const noexcept {
	return m_eventQueue.empty();
}

void Mouse::clearEventQueue() {
	// same complexity as std::deque::clear(),
	// but don't have to expose the std::deque
	m_eventQueue = std::queue<Event>{};
}

void Mouse::clearButtonStates() noexcept {
	m_leftPressed = false;
	m_rightPressed = false;
	m_middlePressed = false;
	m_wheelDeltaAccumulator = 0;
}

inline void Mouse::manageQueueSize() {
	// called before any event is inserted into queue,
	// so only need to check once
	if (m_eventQueue.size() == MAX_QUEUE_SIZE)
		m_eventQueue.pop();
}

void Mouse::buttonPressed(Mouse::Event::Button button, int x, int y) {
	switch (button) {
	case Event::Button::LEFT:
		m_leftPressed = true;
		break;
	case Event::Button::MIDDLE:
		m_middlePressed = true;
		break;
	case Event::Button::RIGHT:
		m_rightPressed = true;
		break;
	}
	if (button != Event::Button::OTHER) {
		manageQueueSize();
		m_eventQueue.emplace(Event::Type::PRESSED, button, x, y);
	}
}

void Mouse::buttonReleased(Mouse::Event::Button button, int x, int y) {
	switch (button) {
	case Event::Button::LEFT:
		m_leftPressed = false;
		break;
	case Event::Button::MIDDLE:
		m_middlePressed = false;
		break;
	case Event::Button::RIGHT:
		m_rightPressed = false;
		break;
	}
	if (button != Event::Button::OTHER) {
		manageQueueSize();
		m_eventQueue.emplace(Event::Type::RELEASED, button, x, y);
	}
}

void Mouse::buttonDoubleClicked(Mouse::Event::Button button, int x, int y) {
	if (button != Event::Button::OTHER) {
		manageQueueSize();
		m_eventQueue.emplace(Event::Type::DOUBLECLICK, button, x, y);
	}
}

void Mouse::scrolled(WPARAM packedDelta, int x, int y) {
	int delta = GET_WHEEL_DELTA_WPARAM(packedDelta);
#ifndef NDEBUG
	if (delta % WHEEL_DELTA != 0 && WHEEL_DELTA % delta != 0)
		throw CWF_EXCEPTION(CwfException::Type::WINDOWS,
			L"Passed wheel delta is not a multiple or factor of WHEEL_DELTA");
#endif
	m_wheelDeltaAccumulator += delta;

	while (m_wheelDeltaAccumulator >= WHEEL_DELTA) {
		manageQueueSize();
		m_eventQueue.emplace(Event::Type::SCROLL_UP,
			Event::Button::MIDDLE, x, y);
		m_wheelDeltaAccumulator -= WHEEL_DELTA;
	}

	while (m_wheelDeltaAccumulator <= -WHEEL_DELTA) {
		manageQueueSize();
		m_eventQueue.emplace(Event::Type::SCROLL_DOWN,
			Event::Button::MIDDLE, x, y);
		m_wheelDeltaAccumulator += WHEEL_DELTA;
	}
}

void Mouse::moved(int x, int y) {
	m_x = x;
	m_y = y;
	manageQueueSize();
	m_eventQueue.emplace(Event::Type::MOVE, Event::Button::OTHER,
		m_x, m_y);
}

void Mouse::entered(int x, int y) {
	m_inClientRegion = true;
	manageQueueSize();
	m_eventQueue.emplace(Event::Type::ENTER_CLIENT,
		Event::Button::OTHER, x, y);
}

void Mouse::left(int x, int y) {
	m_inClientRegion = false;
	manageQueueSize();
	m_eventQueue.emplace(Event::Type::LEAVE_CLIENT,
		Event::Button::OTHER, x, y);
}