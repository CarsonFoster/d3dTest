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
			PRESSED, RELEASED, DOUBLECLICK, MOVE, 
			SCROLL_UP, SCROLL_DOWN, ENTER_CLIENT,
			LEAVE_CLIENT, OTHER
		};
		enum class Button {
			LEFT, MIDDLE, RIGHT, OTHER
		};
	private:
		Type m_type;
		Button m_button;
		int m_x;
		int m_y;
	public:
		Event(Type type, Button button, int x, int y) noexcept : m_type{ type }, m_button{ button }, m_x{ x }, m_y{ y } {}
		~Event() = default;
		Type getType() const noexcept { return m_type; }
		Button getButton() const noexcept { return m_button; }
		int getX() const noexcept { return m_x; }
		int getY() const noexcept { return m_y; }
		std::pair<int, int> getPos() const { return std::make_pair(m_x, m_y); }
	};

	struct PositionDelta {
		long x;
		long y;
	};
private:
	static constexpr unsigned int MAX_QUEUE_SIZE = 16u;
	bool m_leftPressed;
	bool m_middlePressed;
	bool m_rightPressed;
	bool m_inClientRegion;
	bool m_rawInputEnabled;
	int m_x;
	int m_y;
	int m_wheelDeltaAccumulator;
	std::queue<Event> m_eventQueue;
	std::queue<PositionDelta> m_rawQueue;
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
	bool isInClientRegion() const noexcept;

	std::optional<Event> pollEventQueue();
	bool isEventQueueEmpty() const noexcept;
	void clearEventQueue();

	bool enableRawInput();
	bool disableRawInput();
	bool isRawInputEnabled() const noexcept;
	std::optional<PositionDelta> pollRawQueue();
	bool isRawQueueEmpty() const noexcept;
	void clearRawQueue();

	void clearButtonStates() noexcept;

private:
	inline void manageQueueSize();
	inline void manageRawQueueSize();
	void buttonPressed(Event::Button button, int x, int y);
	void buttonReleased(Event::Button button, int x, int y);
	void buttonDoubleClicked(Event::Button button, int x, int y);
	void scrolled(WPARAM packedDelta, int x, int y);
	void moved(int x, int y);
	void entered(int x, int y);
	void left(int x, int y);
	void raw(long dx, long dy);
};

#endif