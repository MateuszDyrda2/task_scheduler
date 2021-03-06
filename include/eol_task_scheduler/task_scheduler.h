#ifndef EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
#define EOL_TASK_SCHEDULER_TASK_SCHEDULER_H

#include "task_queue.h"
#include <condition_variable>
#include <thread>
#include <vector>

#ifndef EOL_TASK_SIZE
#	define EOL_TASK_SIZE 32
#endif // !EOL_TASK_SIZE

#ifndef EOL_QUEUE_SIZE
#	define EOL_QUEUE_SIZE 128
#endif // !EOL_QUEUE_SIZE

namespace eol {
/** Simple multithreaded task scheduler implementation
 * that after creation doesn't allocate any memory
*/
template <std::size_t PriorityLevels>
class task_scheduler
{
  public:
	using self_type		   = task_scheduler<PriorityLevels>;
	using size_type		   = std::size_t;
	using task_queue_type  = task_queue<EOL_QUEUE_SIZE, EOL_TASK_SIZE>;
	using task_qarray_type = std::array<task_queue_type, PriorityLevels>;
	using task_type		   = typename task_queue_type::task_type;

  public:
	/** @brief Create a task scheduler with a specified number
	 * of workers 
	 * @param threadCount number of workers
	*/
	task_scheduler(size_type threadCount);
	task_scheduler(const self_type&) = delete;

	/** @brief Submit a task to be executed on a free thread
	 * with a specified priority
	 * @param callable a callable object without arguments and with 
	 * void return type
	 */
	template <std::size_t Priority, class F>
	requires(Priority < PriorityLevels) void submit(F&& callable);
	~task_scheduler();

  private:
	std::vector<std::thread> workers;
	task_qarray_type jobQueues;

	mutable std::mutex _waitMtx;
	mutable std::condition_variable _waitCvar;
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
							if (q.try_pop(next_task))
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
template <std::size_t Priority, class F>
requires(Priority < PriorityLevels) void task_scheduler<PriorityLevels>::
	submit(F&& callable)
{
	jobQueues[Priority].emplace(std::forward<F>(callable));
	_waitCvar.notify_one();
}
template <std::size_t PriorityLevels>
task_scheduler<PriorityLevels>::~task_scheduler()
{
	is_running = false;
	_waitCvar.notify_all();
	for (auto& w : workers)
		w.join();
}
} // namespace eol
#endif // !EOL_TASK_SCHEDULER_TASK_SCHEDULER_H
