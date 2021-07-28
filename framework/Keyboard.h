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
		unsigned char m_key;
		Type m_type;
	public:
		Event(unsigned char keyCode, Type t = Type::UNKNOWN)
			: m_key{ keyCode }, m_type{ t } {}
		~Event() = default;

		unsigned char getKey() const noexcept { return m_key; }
		Type getType() const noexcept { return m_type; }
	};
private:
	static constexpr unsigned int VIRTUAL_KEYS = 256u;
	static constexpr unsigned int MAX_QUEUE_SIZE = 16u;
	
	std::bitset<VIRTUAL_KEYS> m_keyStates;
	std::queue<Event> m_keyEvents;
	std::queue<unsigned char> m_characterBuffer;
	bool m_autorepeat;
public:
	Keyboard();
	~Keyboard() = default;

	// no copy init/assign
	Keyboard(const Keyboard& o) = delete;
	Keyboard& operator=(const Keyboard& o) = delete;

	bool isKeyPressed(unsigned char key);
	void clearKeyStates();
	bool isEventQueueEmpty() const noexcept;
	std::optional<Event> pollEventQueue();
	void clearEventQueue();

	bool isCharQueueEmpty() const noexcept;
	std::optional<unsigned char> pollCharQueue();
	void clearCharQueue();

	void enableAutorepeat() noexcept;
	void disableAutorepeat() noexcept;
	bool isAutorepeatEnabled() const noexcept;
	
private:
	inline void manageEventQueueSize();
	inline void manageCharQueueSize();
	void keyPressed(unsigned char key);
	void keyReleased(unsigned char key);
	void characterTyped(unsigned char character);
};


#endif