#include "example.h"
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <string.h>
#include <stdio.h>

// CTest implementation

CTest::CTest()
: m_num(76)
{}

void CTest::methodWithParam(const char * pChz){
	printf("CTest(%d)::methodWithParam(%s)\n", m_num, pChz);
}

// WTest implementation

WTest::WTest(mrb_state * mrb)
: m_instance(),
  m_mrb(mrb)
{
	mrb_value vClass;
	vClass.value.p = mrb_class_get(mrb, "CTest");
	m_instance = mrb_instance_new(mrb, vClass);
}

WTest::WTest(mrb_state * mrb, const char * pChzSubclass)
: m_instance(),
  m_mrb(mrb)
{
	mrb_value vClass;
	vClass.value.p = mrb_class_get(mrb, pChzSubclass);
	m_instance = mrb_instance_new(mrb, vClass);
}

void WTest::methodWithParam(const char * arg0)
{
	mrb_value mrb_arg0 = mrb_str_new(m_mrb, arg0, strlen(arg0));
	mrb_funcall(m_mrb, m_instance, "methodWithParam", 1, mrb_arg0);
}


// Bindings implementation

// The c struct that mrb manages, with just a pointer to our c++ class
struct mrb_CTest{
  mrb_CTest()
  : m_p(NULL)
    {}
  CTest * m_p;
};

// the mrb "destructor" which will do the cleanup on our stuff
// feel free to do your memory managment on the pointer however you want
// such as using a "pool"
static void mrb_CTest_free(mrb_state * mrb, void * ptr){
  mrb_CTest * p = (mrb_CTest *)ptr;
  if (p && p->m_p){
    delete p->m_p;
    p->m_p = NULL;
  }
  mrb_free(mrb, ptr);
}

// A struct that configures the cleanup method on our data
static struct mrb_data_type mrb_CTest_binding = { "CTest", mrb_CTest_free };

// The initalize method binding mrb -> c++ ctor
static mrb_value Binding_CTest_initialize(mrb_state * mrb, mrb_value self){
  // search the incomming value for a new binding struct
  mrb_CTest * p = (mrb_CTest *)mrb_get_datatype(mrb, self, &mrb_CTest_binding);
  // if it already has one, get rid of it
  if (p){
    mrb_CTest_free(mrb, p);
  }
  // grab a new struct from the mrb managed memory pool
  p = (mrb_CTest *)mrb_malloc(mrb, sizeof(mrb_CTest));
  // new up our c++ class on this struct
  p->m_p = new CTest;
  // set the outgoing value with the correct data pointers
  DATA_PTR(self) = p;
  DATA_TYPE(self) = &mrb_CTest_binding;
  return self;
}

static mrb_value Binding_CTest_methodWithParam(mrb_state * mrb, mrb_value self){
	// Get the wrapping struct from the data in the mrb_value
	mrb_CTest * p = (mrb_CTest *)mrb_get_datatype(mrb, self, &mrb_CTest_binding);
	// if it's empty, return. May want to do error code here
	if (!p) return self;
	// if we don't have a c++ object on it, return. Error code here
	if (!p->m_p) return self;
	// get the c++ version of the args out of the arguments
	mrb_value arg0;
	mrb_get_args(mrb, "S", &arg0);
	// call the C++ method with the correct arguments
	p->m_p->methodWithParam(mrb_str_ptr(arg0)->ptr);
	// return self like most ruby methods
	return self;
}

static void DefineBoundClass_CTest(mrb_state * mrb) {
	// Build a class definition
    struct RClass * pClass = mrb_define_class(mrb, "CTest", mrb->object_class);
    // The instance type is DATA becaus we are being backed by a struct
    MRB_SET_INSTANCE_TT(pClass, MRB_TT_DATA);
    // declare the method on the class with the c thunk and 1 required argument
    mrb_define_method(mrb, pClass, "methodWithParam", Binding_CTest_methodWithParam, ARGS_REQ(1));
    // define the initialize funcition
    mrb_define_method(mrb, pClass, "initialize", Binding_CTest_initialize, ARGS_NONE());
}

void DefineBoundClasses(mrb_state * mrb)
{
	DefineBoundClass_CTest(mrb);
}

void mrb_cpp_ruby_class_example_gem_init(mrb_state* mrb)
{
	DefineBoundClasses(mrb);
}

void mrb_cpp_ruby_class_example_gem_final(){}
