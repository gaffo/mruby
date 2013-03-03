#pragma once

#include <mruby.h>
/*
 * We are going to define 2 layers of classes here.
 *
 * The first is a class entirely in c++ that is called from ruby. This will be a "C" class, eg starting with a C.
 *
 * The second is an "W" class, which is a wrapper for mruby and calls through the interpreter
 * into the ruby or c++ classes that implement the methods
 *
 * Note that the "C" and "R" are not magical, I'm just using them to make things clear.
 */

// Define the C Class

class CTest {
public:
	CTest();

	void methodWithParam(const char * pChz);

private:
	int m_num; 	// checksum field to prove we're not accessing garbage memory
};

// Define the wrapper class

class WTest {
public:
	WTest(mrb_state * mrb);	// create a wrapped instance of the pure c++ class
	WTest(mrb_state * mrb, const char * pChzSubclass); // create a wrapped instanceof a ruby subclass

	void methodWithParam(const char * pChz);
private:
	mrb_value m_instance;
	mrb_state * m_mrb;
};

// This is the binding function that sets up all of our wrapping code

void DefineBoundClasses(mrb_state * mrb);
