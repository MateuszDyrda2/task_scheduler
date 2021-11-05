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
	task_scheduler(const self_type&) = delete;

	template <class F, std::size_t Priority>
	void submit(F&& callable);
	void wait_for_all();
	~task_scheduler();

  private:
	std::vector<std::thread> workers;
	task_qarray_type jobQueues;

	std::mutex _waitMtx;
	std::condition_variable _waitCvar;
	bool is_running;
};
template <std::size_t PriorityLevels>
task_scheduler<PriorityLevels>::task_scheduler(size_type threadCount)
	: workers(threadCount), jobQueues{}, is_running{true}
{
	for (auto& worker : workers)
	{
		worker = std::thread(
			[this]
			{
				while (is_running)
				{
					task_type next_task;
					while (is_running)
					{
						for (auto& q : jobQueues)
						{
							if (q.try_pop(task))
								goto end_loop;
						}
						std::unique_lock<std::mutex> lock(_waitMtx);
						_waitCvar.wait(lock);
					}
				end_loop:
					if (!is_running) break;
					next_task();
				}
			});
	}
}
template <std::size_t PriorityLevels>
template <class F, std::size_t Priority>
void task_scheduler<PriorityLevels>::submit(F&& callable)
{
}
template <std::size_t PriorityLevels>
void task_scheduler<PriorityLevels>::wait_for_all()
{
}
template <std::size_t PriorityLevels>
task_scheduler<PriorityLevels>::~task_scheduler()
{
}

} // namespace eol
#endif // !EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
