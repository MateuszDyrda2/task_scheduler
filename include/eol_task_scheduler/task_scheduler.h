#ifndef EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
#define EOL_TASK_SCHEDULER_TASK_SCHEDULER_H

#include "task_queue.h"
#include <array>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace eol {
template <std::size_t PriorityLevels>
class task_scheduler
{
  public:
	using self_type		   = task_scheduler<PriorityLevels>;
	using size_type		   = std::size_t;
	using task_queue_type  = task_queue<128, 32>;
	using task_qarray_type = std::array<task_queue_type, PriorityLevels>;

  public:
	task_scheduler(size_type threadCount);
	template <class F, std::size_t Priority>
	void submit(F&& callable);

  private:
	std::vector<std::thread> workers;
	task_qarray_type jobQueues;

	std::mutex _waitMtx;
	std::condition_variable _watCvar;
};
} // namespace eol
#endif // !EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
