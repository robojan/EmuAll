#pragma once

#include <emuall/common.h>
#include <cassert>

template <typename T>
class CounterPtr {
public:
	CounterPtr() :
		_ptr(nullptr), _refcount(0) {

	}

	CounterPtr(T *ptr) :
		_ptr(ptr), _refcount(1){

	}

	virtual ~CounterPtr()
	{
		if (_ptr != nullptr) {
			delete _ptr;
			_ptr = nullptr;
			_refcount = 0;
		}
	}
	
	template <typename... Args>
	void IncRef(Args... args)
	{
		assert((_ptr != nullptr && _refcount > 0) || (_ptr == nullptr && _refcount == 0));
		if (_refcount == 0) {
			_ptr = new T(args...);
		}
		_refcount++;
	}
	void DecRef()
	{
		assert(_refcount > 0);
		_refcount--;
		if (_refcount == 0) {
			delete _ptr;
			_ptr = 0;
		}
	}

	T &operator*() const 
	{
		assert(_ptr != nullptr);
		return *_ptr;
	}
	T *operator->() const 
	{
		assert(_ptr != nullptr);
		return _ptr;
	}

	int GetRefCount()
	{
		assert((_ptr != nullptr && _refcount > 0) || (_ptr == nullptr && _refcount == 0));
		return _refcount;
	}

private: 
	T* _ptr;
	int _refcount;
};