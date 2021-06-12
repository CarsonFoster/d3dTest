#include "CwfException.h"
#include "Mouse.h"
#include <optional>
#include <queue>
#include <utility>
#include <Windows.h>

Mouse::Mouse() noexcept : leftPressed{ false }, middlePressed{ false }, 
	rightPressed{ false }, inClientRegion{ false }, x{ 0 }, y{ 0 },
	wheelDeltaAccumulator{ 0 }, eventQueue{} {}

int Mouse::getX() const noexcept {
	return x;
}

int Mouse::getY() const noexcept {
	return y;
}

std::pair<int, int> Mouse::getPos() const {
	return std::make_pair(x, y);
}

bool Mouse::isLeftPressed() const noexcept {
	return leftPressed;
}

bool Mouse::isMiddlePressed() const noexcept {
	return middlePressed;
}

bool Mouse::isRightPressed() const noexcept {
	return rightPressed;
}

bool Mouse::isInClientRegion() const noexcept {
	return inClientRegion;
}

std::optional<Mouse::Event> Mouse::pollEventQueue() {
	if (!eventQueue.empty()) {
		Mouse::Event e = eventQueue.front();
		eventQueue.pop();
		return e;
	}
	return {};
}

bool Mouse::isEventQueueEmpty() const noexcept {
	return eventQueue.empty();
}

void Mouse::clearEventQueue() {
	// same complexity as std::deque::clear(),
	// but don't have to expose the std::deque
	eventQueue = std::queue<Event>{};
}

void Mouse::clearButtonStates() noexcept {
	leftPressed = false;
	rightPressed = false;
	middlePressed = false;
	wheelDeltaAccumulator = 0;
}

inline void Mouse::manageQueueSize() {
	// called before any event is inserted into queue,
	// so only need to check once
	if (eventQueue.size() == MAX_QUEUE_SIZE)
		eventQueue.pop();
}

void Mouse::buttonPressed(Mouse::Event::Button b, int x, int y) {
	switch (b) {
	case Event::Button::LEFT:
		leftPressed = true;
		break;
	case Event::Button::MIDDLE:
		middlePressed = true;
		break;
	case Event::Button::RIGHT:
		rightPressed = true;
		break;
	}
	if (b != Event::Button::OTHER) {
		manageQueueSize();
		eventQueue.emplace(Event::Type::PRESSED, b, x, y);
	}
}

void Mouse::buttonReleased(Mouse::Event::Button b, int x, int y) {
	switch (b) {
	case Event::Button::LEFT:
		leftPressed = false;
		break;
	case Event::Button::MIDDLE:
		middlePressed = false;
		break;
	case Event::Button::RIGHT:
		rightPressed = false;
		break;
	}
	if (b != Event::Button::OTHER) {
		manageQueueSize();
		eventQueue.emplace(Event::Type::RELEASED, b, x, y);
	}
}

void Mouse::buttonDoubleClicked(Mouse::Event::Button b, int x, int y) {
	if (b != Event::Button::OTHER) {
		manageQueueSize();
		eventQueue.emplace(Event::Type::DOUBLECLICK, b, x, y);
	}
}

void Mouse::scrolled(WPARAM packedDelta, int x, int y) {
	int delta = GET_WHEEL_DELTA_WPARAM(packedDelta);
#ifndef NDEBUG
	if (delta % WHEEL_DELTA != 0 && WHEEL_DELTA % delta != 0)
		throw CWF_EXCEPTION(CwfException::CwfExceptionType::WINDOWS,
			L"Passed wheel delta is not a multiple or factor of WHEEL_DELTA");
#endif
	wheelDeltaAccumulator += delta;

	while (wheelDeltaAccumulator >= WHEEL_DELTA) {
		manageQueueSize();
		eventQueue.emplace(Event::Type::SCROLL_UP,
			Event::Button::MIDDLE, x, y);
		wheelDeltaAccumulator -= WHEEL_DELTA;
	}

	while (wheelDeltaAccumulator <= -WHEEL_DELTA) {
		manageQueueSize();
		eventQueue.emplace(Event::Type::SCROLL_DOWN,
			Event::Button::MIDDLE, x, y);
		wheelDeltaAccumulator += WHEEL_DELTA;
	}
}

void Mouse::moved(int aX, int aY) {
	x = aX;
	y = aY;
	manageQueueSize();
	eventQueue.emplace(Event::Type::MOVE, Event::Button::OTHER,
		x, y);
}

void Mouse::entered(int x, int y) {
	inClientRegion = true;
	manageQueueSize();
	eventQueue.emplace(Event::Type::ENTER_CLIENT,
		Event::Button::OTHER, x, y);
}

void Mouse::left(int x, int y) {
	inClientRegion = false;
	manageQueueSize();
	eventQueue.emplace(Event::Type::LEAVE_CLIENT,
		Event::Button::OTHER, x, y);
}