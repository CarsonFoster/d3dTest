#ifndef CWF_MOUSE_H
#define CWF_MOUSE_H
 
#include <optional>
#include <queue>
#include <utility>
#include <Windows.h>

class Window;

class Mouse {
public:
	friend class Window;
	class Event {
	public:
		enum class Type {
			PRESSED, RELEASED, DOUBLECLICK, MOVE, SCROLL, OTHER
		};
		enum class Button {
			LEFT, MIDDLE, RIGHT, OTHER
		};
	private:
		Type t;
		Button b;
		int x;
		int y;
	public:
		Event(Type type, Button btn, int aX, int aY) noexcept : t{ type }, b{ btn }, x{ aX }, y{ aY } {}
		~Event() = default;
		Type getType() const noexcept { return t; }
		Button getButton() const noexcept { return b; }
		int getX() const noexcept { return x; }
		int getY() const noexcept { return y; }
		std::pair<int, int> getPos() const { return std::make_pair(x, y); }
	};
private:
	static constexpr unsigned int MAX_QUEUE_SIZE = 16u;
	bool isLeftPressed;
	bool isMiddlePressed;
	bool isRightPressed;
	bool isInWindow;
	int x;
	int y;
	int wheelDeltaAccumulator;
	std::queue<Event> eventQueue;
public:
	Mouse() noexcept;
	~Mouse() = default;
	// no copy init/assign
	Mouse(const Mouse& o) = delete;
	Mouse& operator=(const Mouse& o) = delete;

	int getX() const noexcept;
	int getY() const noexcept;
	std::pair<int, int> getPos() const;

	bool isLeftPressed() const noexcept;
	bool isMiddlePressed() const noexcept;
	bool isRightPressed() const noexcept;
	bool isInWindow() const noexcept;

	std::optional<Event> pollEventQueue();
	bool isEventQueueEmpty() const noexcept;

private:
	inline void manageQueueSize();
	void buttonPressed(Event::Button b, int x, int y);
	void buttonReleased(Event::Button b, int x, int y);
	void buttonDoubleClicked(Event::Button b, int x, int y);
	void scrolled(WPARAM packedDelta, int x, int y);
	void moved(int x, int y);
};

#endif