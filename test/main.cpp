#include <eol_task_scheduler/task_scheduler.h>
#include <iostream>

using namespace eol;
int main()
{
	task_scheduler<3> scheduler(std::thread::hardware_concurrency());
	scheduler.submit<2>([]
						{ std::cout << "hey\n"; });
	scheduler.submit<1>([]
						{ std::cout << "hey\n"; });

	scheduler.submit<0>([]
						{ std::cout << "hey\n"; });

	scheduler.submit<1>([]
						{ std::cout << "hey\n"; });
	scheduler.submit<2>([]
						{ std::cout << "hey\n"; });

	std::this_thread::sleep_for(std::chrono::seconds(1));
	return 0;
}
