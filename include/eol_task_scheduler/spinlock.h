#ifndef EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
#define EOL_TASK_SCHEDULER_TASK_SCHEDULER_H

#include <atomic>
#include <xmmintrin.h>

namespace eol {
struct spinlock
{
	std::atomic_bool _lck{false};

	void lock()
	{
		if (!_lck.exchange(true, std::memory_order_acquire))
		{
			return;
		}
		while (_lck.load(std::memory_order_relaxed))
		{
			_mm_pause();
		}
	}
	bool try_lock()
	{
		return !_lck.load(std::memory_order_relaxed) && !_lck.exchange(true, std::memory_order_acquire);
	}
	void unlock()
	{
		_lck.store(false, std::memory_order_acquire);
	}
};
}
#endif // !EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
