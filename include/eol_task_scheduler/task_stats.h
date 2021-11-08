#ifndef EOL_TASK_SCHEDULER_TASK_STATS_H
#define EOL_TASK_SCHEDULER_TASK_STATS_H

#include <chrono>

namespace eol {
struct task_stats
{
	using time_t = std::chrono::microseconds;

	void record(const time_t& time)
	{
		_average = (time + _count * _average) / (++_count);
	}

	time_t _average;
	std::size_t _count;
};
} // namespace eol

#endif // !EOL_TASK_SCHEDULER_TASK_STATS_H
