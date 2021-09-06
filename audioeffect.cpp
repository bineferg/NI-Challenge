/**
 *
 * Native Instruments CPP Code Challenge
 * audioeffect.cpp
 * @author EF
 *
 * Please find further answers to Question 2 in README.md
 *
 **/

#include<iostream>
#include<memory>
#include<cassert>

// AudioEffect is the base class for effects that can process 
// audio and have a subsequent effect (next).
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
	auto ae1 = std::make_shared<Effect>();
	auto ae2 = std::make_shared<Effect>();
	auto ae3 = std::make_shared<Effect>();
	
	head->next = ae1;
	ae1->next = ae2;
	ae2->next = ae3;

	bool feedback = detect_feedback(head);
	assert(feedback == false);
	std::cout << std::boolalpha << "\t" << "Expected: false Feedback detected: " << feedback << std::endl; 
}


// When ownership of ae1 is stored with another shared pointer (ae3 which causes the loop)
// a cyclical reference causes a memory leak (ie.e the Effect is only deleted when all
// the ->next references are deleted but these are only deleted once the Effect is deleted.)
//
// When two shared_ptrs reference eachother, the reference count will never reach zero 
// and so will never be deleted / memory never be freed.
void test_with_feedback()
{
	std::cout << "Testing a list with feedback.." << std::endl;
	auto head = std::make_shared<Effect>();
	auto ae1 = std::make_shared<Effect>();
	auto ae2 = std::make_shared<Effect>();
	auto ae3 = std::make_shared<Effect>();
	
	head->next = ae1;
	ae1->next = ae2;
	ae2->next = ae3;
	ae3->next = ae1; // Causes the cyclical reference / memory leak
	
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
// have a feedback loop.
int main(void) {
	test_no_feedback();
	test_with_feedback();
	test_null();
	return 0;
}

