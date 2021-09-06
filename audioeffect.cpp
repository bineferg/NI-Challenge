/**
 *
 * Native Instruments CPP Code Challenge
 * audioeffect.cpp
 * @author EF
 *
 * Further answers to Question 2 can be found in README.md
 *
 **/

#include<iostream>
#include<memory>
#include<cassert>

struct AudioEffect 
{
	virtual ~AudioEffect() = default;
	virtual void process(float* buf, size_t num) = 0;
	std::shared_ptr<AudioEffect> next;
};

// Generall "Effect" struct derived from AudioEffect
struct Effect : AudioEffect 
{
public:
	Effect(){next = nullptr;} 
	~Effect(){}
	void process(float* buf, size_t num){}
};

// Uses Floyd’s Cycle-Finding Algorithm
// https://tinyurl.com/vxc67ua3
//
// detect_feedback returns true if a loop exists
// in the effects chain and false otherwise
bool detect_feedback(std::shared_ptr<AudioEffect> ptr)
{
    std::shared_ptr<AudioEffect> slow_p = ptr, fast_p = ptr;
    while (slow_p && fast_p && fast_p->next) {
        slow_p = slow_p->next;
        fast_p = fast_p->next->next;
        if (slow_p == fast_p) {
            return 1;
        }
    }
    return 0;
	
}

void test_no_feedback()
{
	std::cout << "Testing a list with no feedback.." << std::endl;
	auto head = std::make_shared<Effect>();
	auto n1 = std::make_shared<Effect>();
	auto n2 = std::make_shared<Effect>();
	auto n3 = std::make_shared<Effect>();
	
	head->next = n1;
	n1->next = n2;
	n2->next = n3;

	bool feedback = detect_feedback(head);
	assert(feedback == false);
	std::cout << std::boolalpha << "\t" << "Expected: false Feedback detected: " << feedback << std::endl; 
}


// When ownership of n1 is stored with another shared pointer (n3 which causes the loop)
// a cyclical reference causes a memory leak (ie.e the Effect is only deleted when all
// the ->next references are deleted but these are only deleted once the Effect is deleted.)
//
// When two shared_ptrs reference eachother, the reference count will never reach zero 
// and so will never be deleted / memory never be freed.
void test_with_feedback()
{
	std::cout << "Testing a list with feedback.." << std::endl;
	auto head = std::make_shared<Effect>();
	auto n1 = std::make_shared<Effect>();
	auto n2 = std::make_shared<Effect>();
	auto n3 = std::make_shared<Effect>();
	
	head->next = n1;
	n1->next = n2;
	n2->next = n3;
	n3->next = n1; // Causes the cyclical reference / memory leak
	
	bool feedback = detect_feedback(head);
	assert(feedback == true);
	std::cout << std::boolalpha << "\t" << "Expected: true Feedback detected: " << feedback << std::endl; 
}

void test_null()
{
	std::cout << "Testing with NULL.." << std::endl;
	bool feedback = detect_feedback(NULL);
	assert(feedback == false);
	std::cout << std::boolalpha << "\t" << "Expected: false Feedback detected: " << feedback << std::endl; 
}

// Creates and tests if effects chains
// have a feedback loop exists.
int main(void) {
	test_no_feedback();
	test_with_feedback();
	test_null();
	return 0;
}

