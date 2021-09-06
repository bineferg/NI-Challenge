# Native Instruments - CPP Coding Challenge

A simple function checking if there is a feedback loop in the effects chain


## Requirements 
* clang 
* Docker (for valgrind)

## Build & Run
* Unit testing is done without a framework and just uses `assert` statments.
* These tests are invoked from the main function upon run

```
$ make all run
```

## The way AudioEffect is designed is not ideal. What would you change about the design?

Using `shared_ptr` for the effects chain is not ideal because there is no indication that non-deterministic shared ownership is required. In other words, `shared_ptr` are best used in scenarios where more than one owner might manage the lifetime of an object, with an "effects chain" this is not an apparent requirement, but rather each AudioEffect would own the next.

Additionally, as noted in the code, using shared pointers in a scenario where valid cyclical references occur, like in a feedback loop, will cause memory leaks. Please see the valgrind output below. This could be improved by using `weak_ptr`s for the cyclical referencing.

A better fit could be to use `unique_ptr` to manage the allocation/deallocation of an AudioEffect. This however can also be delicate to use for constructing more simple, lower level data types such as a effects chain (which I am interpreting as a singly linked list). Depending on the context and requirements of the system / effects chain, it might be better still to just use a standard container class such as `std::list` or `std::vector`. It is really a question of context, and who should own the resources.

## Memory Checks
*Note:* A docker container was used in order to run Valgrind on linux

### No cyclical reference constructed

```
$ make leack-check
```
```
==14== Memcheck, a memory error detector
==14== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==14== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==14== Command: ./audioeffect
==14== 
Testing a list with no feedback..
	Expected: false Feedback detected: false
Testing with NULL..
	Expected: false Feedback detected: false
==14== 
==14== HEAP SUMMARY:
==14==     in use at exit: 72,704 bytes in 1 blocks
==14==   total heap usage: 6 allocs, 5 frees, 73,888 bytes allocated
==14== 
==14== LEAK SUMMARY:
==14==    definitely lost: 0 bytes in 0 blocks
==14==    indirectly lost: 0 bytes in 0 blocks
==14==      possibly lost: 0 bytes in 0 blocks
==14==    still reachable: 72,704 bytes in 1 blocks
==14==         suppressed: 0 bytes in 0 blocks
==14== Reachable blocks (those to which a pointer was found) are not shown.
==14== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==14== 
==14== For counts of detected and suppressed errors, rerun with: -v
==14== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### With cyclical reference constructed

```
$ make leack-check
```
```
==14== Memcheck, a memory error detector
==14== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==14== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==14== Command: ./audioeffect
==14== 
Testing a list with no feedback..
	Expected: false Feedback detected: false
Testing a list with feedback..
	Expected: true Feedback detected: true
Testing with NULL..
	Expected: false Feedback detected: false
==14== 
==14== HEAP SUMMARY:
==14==     in use at exit: 72,824 bytes in 4 blocks
==14==   total heap usage: 10 allocs, 6 frees, 74,048 bytes allocated
==14== 
==14== 120 (40 direct, 80 indirect) bytes in 1 blocks are definitely lost in loss record 3 of 4
==14==    at 0x4C2E0EF: operator new(unsigned long) (in /usr/lib/valgrind/vgpreload_memcheck-amd64-linux.so)
==14==    by 0x40267B: __gnu_cxx::new_allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> >::allocate(unsigned long, void const*) (new_allocator.h:104)
==14==    by 0x4024CC: std::allocator_traits<std::allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> > >::allocate(std::allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> >&, unsigned long) (alloc_traits.h:491)
==14==    by 0x4022BF: std::__allocated_ptr<std::allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> > > std::__allocate_guarded<std::allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> > >(std::allocator<std::_Sp_counted_ptr_inplace<Effect, std::allocator<Effect>, (__gnu_cxx::_Lock_policy)2> >&) (allocated_ptr.h:102)
==14==    by 0x4020BE: std::__shared_count<(__gnu_cxx::_Lock_policy)2>::__shared_count<Effect, std::allocator<Effect>>(std::_Sp_make_shared_tag, Effect*, std::allocator<Effect> const&) (shared_ptr_base.h:615)
==14==    by 0x402005: std::__shared_ptr<Effect, (__gnu_cxx::_Lock_policy)2>::__shared_ptr<std::allocator<Effect>>(std::_Sp_make_shared_tag, std::allocator<Effect> const&) (shared_ptr_base.h:1097)
==14==    by 0x401EB3: std::shared_ptr<Effect>::shared_ptr<std::allocator<Effect>>(std::_Sp_make_shared_tag, std::allocator<Effect> const&) (shared_ptr.h:319)
==14==    by 0x401D11: std::shared_ptr<Effect> std::allocate_shared<Effect, std::allocator<Effect>>(std::allocator<Effect> const&) (shared_ptr.h:620)
==14==    by 0x401AC4: _ZSt11make_sharedI6EffectIEESt10shared_ptrIT_EDpOT0_ (shared_ptr.h:636)
==14==    by 0x4011AA: test_with_feedback() (audioeffect.cpp:71)
==14==    by 0x401457: main (audioeffect.cpp:97)
==14== 
==14== LEAK SUMMARY:
==14==    definitely lost: 40 bytes in 1 blocks
==14==    indirectly lost: 80 bytes in 2 blocks
==14==      possibly lost: 0 bytes in 0 blocks
==14==    still reachable: 72,704 bytes in 1 blocks
==14==         suppressed: 0 bytes in 0 blocks
==14== Reachable blocks (those to which a pointer was found) are not shown.
==14== To see them, rerun with: --leak-check=full --show-leak-kinds=all
==14== 
==14== For counts of detected and suppressed errors, rerun with: -v
==14== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```
