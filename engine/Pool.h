#pragma once

namespace ok
{
	template <class T>
	class Pool;

	template <class T>
	class PoolContainer
	{
	public:
		T& operator*();
	private:
		friend class ok::Pool<T>;
		T* item;
		typename std::deque<PoolContainer<T>*>::iterator _out_location;
	};

	enum class PoolLimitGrabFrom
	{
		Begin,
		End
	};

	template <class T>
	class Pool
	{
	public:
		Pool(
			int limit = -1,
			std::function<T*()> custom_allocator = nullptr,
			std::function<void(T&)> on_inject = nullptr,
			std::function<void(T&)> on_eject = nullptr,
			std::function<void(T&, float)> on_update = nullptr
			);
		~Pool();

		PoolContainer<T>& Eject();
		void Inject(PoolContainer<T>& container);
		std::vector<PoolContainer<T>>& Items();
		void SetLimitGrabFrom(ok::PoolLimitGrabFrom limit_grab_from);

		void Update(float dt);
	private:
		std::vector<PoolContainer<T>> _items;
		std::stack<PoolContainer<T>*> _items_in;
		std::deque<PoolContainer<T>*> _items_out;

		std::function<void(T&)> _event_on_inject;
		std::function<void(T&)> _event_on_eject;
		std::function<void(T&, float)> _event_on_update;
		std::function<T*()> _custom_allocator;

		int _limit;
		ok::PoolLimitGrabFrom _limit_grab_from;
	};

	template<class T>
	inline Pool<T>::Pool(int limit, std::function<T*()> custom_allocator, std::function<void(T&)> on_inject, std::function<void(T&)> on_eject, std::function<void(T&, float)> on_update)
	{
		_event_on_inject = on_inject;
		_event_on_eject = on_eject;
		_event_on_update = on_update;
		_custom_allocator = custom_allocator;
		_limit = limit;
		_limit_grab_from = ok::PoolLimitGrabFrom::Begin;
	}

	template<class T>
	inline Pool<T>::~Pool()
	{
		for (ok::PoolContainer<T>& container : _items)
		{
			delete container.item;
		}
	}

	template<class T>
	inline PoolContainer<T>& Pool<T>::Eject()
	{
		PoolContainer<T>* container;

		if (_items_in.size() == 0)
		{
			if (_limit != -1 && static_cast<int>(_items.size()) == _limit)
			{
				if (_limit_grab_from == ok::PoolLimitGrabFrom::End)
				{
					container = _items_out.back();
				}
				else if (_limit_grab_from == ok::PoolLimitGrabFrom::Begin)
				{
					container = _items_out.front();
					_items_out.pop_front();
					_items_out.push_back(container);
					container->_out_location = _items_out.end();
				}

				if (_event_on_inject) _event_on_inject(**container);
				if (_event_on_eject) _event_on_eject(**container);

				return *container;
			}
			else
			{
				_items.push_back(PoolContainer<T>());
				container = &_items[_items.size() - 1];

				if (_custom_allocator)
					container->item = _custom_allocator();
				else
					container->item = new T();
			}
		}
		else
		{
			container = _items_in.top();
			_items_in.pop();
		}

		_items_out.push_back(container);

		container->_out_location = _items_out.end();

		if (_event_on_eject) _event_on_eject(**container);

		return *container;
	}

	template<class T>
	inline void Pool<T>::Inject(PoolContainer<T>& container)
	{
		if (_event_on_inject) _event_on_inject(*container);

		_items_out.erase(container._out_location);
		_items_in.push(&container);
	}

	template<class T>
	inline std::vector<PoolContainer<T>>& Pool<T>::Items()
	{
		return _items;
	}

	template<class T>
	inline void Pool<T>::SetLimitGrabFrom(ok::PoolLimitGrabFrom limit_grab_from)
	{
		_limit_grab_from = limit_grab_from;
	}

	template<class T>
	inline void Pool<T>::Update(float dt)
	{
		
		if (_event_on_update) for (ok::PoolContainer<T>& container : _items)
		{
			_event_on_update(*container, dt);
		}
	}

	template<class T>
	inline T & PoolContainer<T>::operator*()
	{
		return *item;
	}
}