#ifndef CWF_KEYBOARD_H
#define CWF_KEYBOARD_H

#include <bitset>
#include <optional>
#include <queue>

class Window;

class Keyboard {
public:
	friend class Window;
	class Event {
	public:
		enum class Type {
			PRESSED, RELEASED, UNKNOWN
		};
	private:
		unsigned char key;
		Type type;
	public:
		Event(unsigned char keyCode, Type t = Type::UNKNOWN)
			: key{ keyCode }, type{ t } {}
		~Event() = default;

		unsigned char getKey() const noexcept { return key; }
		Type getType() const noexcept { return type; }
	};
private:
	static constexpr unsigned int VIRTUAL_KEYS = 256u;
	static constexpr unsigned int MAX_QUEUE_SIZE = 16u;
	
	std::bitset<VIRTUAL_KEYS> keyStates;
	std::queue<Event> keyEvents;
	std::queue<char> characterBuffer;
	bool autorepeat;
public:
	Keyboard();
	~Keyboard() = default;

	// no copy init/assign
	Keyboard(const Keyboard& o) = delete;
	Keyboard& operator=(const Keyboard& o) = delete;

	bool isKeyPressed(unsigned char key);
	bool isEventQueueEmpty() const noexcept;
	std::optional<Event> pollEventQueue();
	void clearEventQueue();

	bool isCharQueueEmpty() const noexcept;
	std::optional<char> pollCharQueue();
	void clearCharQueue();

	void enableAutorepeat() noexcept;
	void disableAutorepeat() noexcept;
	bool isAutorepeatEnabled() const noexcept;
	
private:
	inline void manageEventQueueSize();
	inline void manageCharQueueSize();
	void keyPressed(unsigned char key);
	void keyReleased(unsigned char key);
	void characterTyped(char character);
	void clearKeyStates();
};


#endif