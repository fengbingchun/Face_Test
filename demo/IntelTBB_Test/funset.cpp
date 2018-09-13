#include "funset.hpp"
#include <iostream>
#include <tbb/tbb.h>

// Blog: https://blog.csdn.net/fengbingchun/article/details/58281829

// reference: http://www.ibm.com/developerworks/cn/aix/library/au-intelthreadbuilding/
class first_task : public tbb::task {
public:
	tbb::task* execute() {
		fprintf(stderr, "Hello World!\n");
		return nullptr;
	}
};

int test_IntelTBB_1()
{
	tbb::task_scheduler_init init(tbb::task_scheduler_init::automatic);
	first_task& f1 = *new(tbb::task::allocate_root()) first_task();
	tbb::task::spawn_root_and_wait(f1);

	return 0;
}

class first_task_2 : public tbb::task {
public:
	tbb::task* execute() {
		fprintf(stderr, "Hello World!\n");
		tbb::task_list list1;
		list1.push_back(*new(allocate_child()) first_task_2());
		list1.push_back(*new(allocate_child()) first_task_2());
		set_ref_count(3); // 2 (1 per child task) + 1 (for the wait) 
		spawn_and_wait_for_all(list1);
		return nullptr;
	}
};

int test_IntelTBB_2()
{
	first_task& f1 = *new(tbb::task::allocate_root()) first_task();
	tbb::task::spawn_root_and_wait(f1);

	return 0;
}

class say_hello {
public:
	say_hello(const char* str) : message(str) {  }
	void operator( ) () const {
		fprintf(stderr, "%s\n", message);
	}
private:
	const char* message;
};

int test_IntelTBB_3()
{
	tbb::task_group tg;
	tg.run(say_hello("child 1")); // spawn task and return
	tg.run(say_hello("child 2")); // spawn another task and return 
	tg.wait(); // wait for tasks to complete

	return 0;
}
