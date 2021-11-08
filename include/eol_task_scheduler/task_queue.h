#ifndef EOL_TASK_SCHEDULER_TASK_QUEUE_H
#define EOL_TASK_SCHEDULER_TASK_QUEUE_H

#include "spinlock.h"
#include <array>
#include <chrono>
#include <eol_task/task.h>
#include <mutex>

namespace eol {
template <std::size_t Size, std::size_t task_size>
class task_queue
{
  public:
	using self_type		 = task_queue<Size, task_size>;
	using size_type		 = std::size_t;
	using task_type		 = task<task_size>;
	using lock_type		 = spinlock;
	using container_type = std::array<task_type, Size>;
	using q_iterator	 = typename container_type::iterator;
	using clock_type	 = std::chrono::high_resolution_clock;
	using duration_type	 = std::chrono::microseconds;
	using time_point	 = std::chrono::time_point<clock_type, duration_type>;

  public:
	task_queue() = default;
	void push(const task_type& task);
	void push(task_type&& task);
	template <class... Args>
	void emplace(Args&&... args);

	bool try_push(const task_type& task);
	bool try_push(task_type&& task);
	template <class... Args>
	bool try_emplace(Args&&... args);

	void pop(task_type& task);
	bool try_pop(task_type& task);

	size_type size() const noexcept;
	bool empty() const noexcept;
	bool full() const noexcept;
	time_point get_last_changed() const;

  private:
	container_type _container;
	mutable lock_type _lock;
	q_iterator _head{_container.begin()},
		_tail{_container.begin()};
	size_type _size{};
	time_point _lastChanged;

  private:
	void increment_head();
	void increment_tail();
};
template <std::size_t Size, std::size_t task_size>
void task_queue<Size, task_size>::push(const task_type& task)
{
	std::lock_guard<lock_type> lck(_lock);
	*_tail = task;
	increment_tail();
}
template <std::size_t Size, std::size_t task_size>
void task_queue<Size, task_size>::push(task_type&& task)
{
	std::lock_guard<lock_type> lck(_lock);
	*_tail = std::move(task);
	increment_tail();
}
template <std::size_t Size, std::size_t task_size>
template <class... Args>
void task_queue<Size, task_size>::emplace(Args&&... args)
{
	std::lock_guard<lock_type> lck(_lock);
	*_tail = task_type(std::forward<Args>(args)...);
	increment_tail();
}
template <std::size_t Size, std::size_t task_size>
bool task_queue<Size, task_size>::try_push(const task_type& task)
{
	std::lock_guard<lock_type> lck(_lock);
	if (_size == size)
		return false;
	*_tail = task;
	increment_tail();
	return true;
}
template <std::size_t Size, std::size_t task_size>
bool task_queue<Size, task_size>::try_push(task_type&& task)
{
	std::lock_guard<lock_type> lck(_lock);
	if (_size == size)
		return false;
	*_tail = std::move(task);
	increment_tail();
	return true;
}
template <std::size_t Size, std::size_t task_size>
template <class... Args>
bool task_queue<Size, task_size>::try_emplace(Args&&... args)
{
	std::lock_guard<lock_type> lck(_lock);
	if (_size == size)
		return false;
	*_tail = task_type(std::forward<Args>(args)...);
	increment_tail();
	return true;
}
template <std::size_t Size, std::size_t task_size>
void task_queue<Size, task_size>::pop(task_type& task)
{
	std::lock_guard<lock_type> lck(_lock);
	task = std::move(*_head);
	_head->~task_type();
	increment_head();
}
template <std::size_t Size, std::size_t task_size>
bool task_queue<Size, task_size>::try_pop(task_type& task)
{
	std::lock_guard<lock_type> lck(_lock);
	if (_size == 0)
		return false;
	task = std::move(*_head);
	_head->~task_type();
	increment_head();
	return true;
}
template <std::size_t Size, std::size_t task_size>
typename task_queue<Size, task_size>::size_type
task_queue<Size, task_size>::size() const noexcept
{
	std::lock_guard<lock_type> lck(_lock);
	return _size;
}
template <std::size_t Size, std::size_t task_size>
bool task_queue<Size, task_size>::empty() const noexcept
{
	std::lock_guard<lock_type> lck(_lock);
	return _size == 0;
}
template <std::size_t Size, std::size_t task_size>
bool task_queue<Size, task_size>::full() const noexcept
{
	std::lock_guard<lock_type> lck(_lock);
	return _size == Size;
}
template <std::size_t Size, std::size_t task_size>
void task_queue<Size, task_size>::increment_head()
{
	if (++_head == _container.end())
		_head = _container.begin();
	--_size;
	_lastChanged = clock_type::now();
}
template <std::size_t Size, std::size_t task_size>
void task_queue<Size, task_size>::increment_tail()
{
	if (++_tail == _container.end())
		_tail = _container.begin();
	++_size;
	_lastChanged = clock_type::now();
}
template <std::size_t Size, std::size_t task_size>
typename task_queue<Size, task_size>::time_point
task_queue<Size, task_size>::get_last_changed() const
{
	return _lastChanged;
}
} // namespace eol
#endif // !EOL_TASK_SCHEDULER_TASK_QUEUE_H
