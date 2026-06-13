#pragma once

#include <string>
#include <ostream>
#include <utility>
namespace Aine
{

	#define BIND_EVENT_FUNC(fn) [this](auto&&... args) ->decltype(auto)  {return this->fn(std::forward<decltype(args)>(args)...);}

	enum class EventType
	{
		None = 0,
		WindowClose,WindowResize
	};

	enum EventCategory
	{
		None				= 0,
		EventApplication	= 1 << 0,
		EventInput			= 1 << 1,
		EventKeyboard		= 1 << 2,
		EventMouse			= 1 << 3,
		EventMouseButton	= 1 << 4
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type;}\
								virtual EventType GetEventType() const override { return GetStaticType();}\
								virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const { return GetCategoryFlags() & category; }
	};

	class EventDisPatcher
	{
	public:
		EventDisPatcher(Event& event)
			:m_Event(event) {
		}


		template<typename T, typename F>
		bool Dispatcher(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}