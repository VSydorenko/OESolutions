#ifndef _VALUE_PTR_H__
#define _VALUE_PTR_H__

#include "backend_core.h"

template<typename retType>
class value_ptr {
	retType* m_pointer;
public:

	value_ptr(retType* _ptr) : m_pointer(_ptr) { if (m_pointer != nullptr) m_pointer->IncrRef(); }
	~value_ptr() { if (m_pointer != nullptr) m_pointer->DecrRef(); }

	inline retType* pointer() const { return m_pointer; }

	// copy assignment
	value_ptr& value_ptr::operator=(value_ptr other) noexcept { // call copy or move constructor to construct other
		std::swap(m_pointer, other.m_pointer); // exchange resources between *this and other
		return *this;
	} // destructor of other is called to release the resources formerly managed by *this

	// overload operator->
	inline retType* operator->() const { return pointer(); }
};

#endif 