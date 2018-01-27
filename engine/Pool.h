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
		bool IsOut();
	private:
		friend class ok::Pool<T>;

		T* item;
		typename std::list<int>::iterator _out_location;
		bool _is_out;
		int _index;
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
			std::function<bool(T&, float)> on_update = nullptr
		);
		~Pool();

		std::shared_ptr<PoolContainer<T>> Eject();
		void Inject(std::shared_ptr<PoolContainer<T>> container);
		std::vector<std::shared_ptr<PoolContainer<T>>>& Items();
		void SetLimitGrabFrom(ok::PoolLimitGrabFrom limit_grab_from);

		void Update(float dt);
	private:
		std::vector<std::shared_ptr<PoolContainer<T>>> _items;
		std::stack<int> _items_in;
		std::list<int> _items_out;

		std::function<void(T&)> _event_on_inject;
		std::function<void(T&)> _event_on_eject;
		std::function<bool(T&, float)> _event_on_update;
		std::function<T*()> _custom_allocator;

		int _limit;
		ok::PoolLimitGrabFrom _limit_grab_from;
	};

	template<class T>
	inline Pool<T>::Pool(int limit, std::function<T*()> custom_allocator, std::function<void(T&)> on_inject, std::function<void(T&)> on_eject, std::function<bool(T&, float)> on_update)
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
		for (std::shared_ptr<ok::PoolContainer<T>>& container : _items)
		{
			delete container->item;
		}
	}

	template<class T>
	inline std::shared_ptr<PoolContainer<T>> Pool<T>::Eject()
	{
		std::shared_ptr<PoolContainer<T>> container;

		if (_items_in.size() == 0)
		{
			if (_limit != -1 && static_cast<int>(_items.size()) == _limit)
			{
				if (_limit_grab_from == ok::PoolLimitGrabFrom::End)
				{
					container = _items[_items_out.back()];
				}
				else if (_limit_grab_from == ok::PoolLimitGrabFrom::Begin)
				{
					int index = _items_out.front();
					container = _items[index];
					_items_out.pop_front();
					_items_out.push_back(index);
					container->_out_location = _items_out.end();
					container->_out_location--;
				}

				if (_event_on_inject) _event_on_inject(**container);
				if (_event_on_eject) _event_on_eject(**container);

				container->_is_out = true;

				return container;
			}
			else
			{
				_items.push_back(std::make_shared<PoolContainer<T>>());
				container = _items[_items.size() - 1];
				container->_index = _items.size() - 1;

				if (_custom_allocator)
					container->item = _custom_allocator();
				else
					container->item = new T();
			}
		}
		else
		{
			container = _items[_items_in.top()];
			_items_in.pop();
		}

		_items_out.push_back(container->_index);

		container->_out_location = _items_out.end();
		container->_out_location--;

		if (_event_on_eject) _event_on_eject(**container);

		container->_is_out = true;

		return container;
	}

	template<class T>
	inline void Pool<T>::Inject(std::shared_ptr<PoolContainer<T>> container)
	{
		if (_event_on_inject) _event_on_inject(**container);

		_items_out.erase(container->_out_location);
		_items_in.push(container->_index);

		container->_is_out = false;
	}

	template<class T>
	inline std::vector<std::shared_ptr<PoolContainer<T>>>& Pool<T>::Items()
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

		if (_event_on_update) for (std::shared_ptr<ok::PoolContainer<T>>& container : _items)
		{
			if (container->_is_out && _event_on_update(**container, dt) == false)
			{
				Inject(container);
			}
		}
	}

	template<class T>
	inline T & PoolContainer<T>::operator*()
	{
		return *item;
	}

	template<class T>
	inline bool PoolContainer<T>::IsOut()
	{
		return _is_out;
	}
}