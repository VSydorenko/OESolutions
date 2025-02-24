#ifndef _VALUE_PTR_H__
#define _VALUE_PTR_H__

#include "backend_core.h"

template<typename retType>
class value_ptr
{
	retType* m_pointer;
public:

	value_ptr(retType* _ptr) : m_pointer(_ptr) { m_pointer->IncrRef(); }
	~value_ptr() { m_pointer->DecrRef(); }

	inline retType* pointer() const { return m_pointer; }

	// overload operator->
	inline retType* operator->() const { return pointer(); }
};

#endif 