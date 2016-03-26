#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

#include <list>
#include <vector>

template <class Type, int blockSize = 1024> 
class EventQueue {
public:
	EventQueue();
	~EventQueue();

	// Iterators
	// TODO: to implement

	// Capacity
	size_t size() const;
	bool empty() const;

	// Modifiers
	void push_back(const Type &);
	template <typename... Args>
	void emplace_back(Args&&... args);
	void clear();
	void pop_front();

	// Element access
	Type &front();
	const Type &front() const;

private:
	std::list<std::vector<Type>> _data;
	size_t _size;
	size_t _emptyFront;

	size_t _maxSize, _maxBlocks;
};

template <class Type, int blockSize /*= 1024*/>
EventQueue<Type, blockSize>::EventQueue() : 
	_data(1), _size(0), _emptyFront(0), _maxSize(0), _maxBlocks(0)
{
	_data.front().resize(blockSize);
}

template <class Type, int blockSize /*= 1024*/>
EventQueue<Type, blockSize>::~EventQueue()
{

}
template <class Type, int blockSize /*= 1024*/>
size_t EventQueue<Type, blockSize>::size() const
{
	return _size;
}

template <class Type, int blockSize /*= 1024*/>
bool EventQueue<Type, blockSize>::empty() const
{
	return _size == 0;
}

template <class Type, int blockSize /*= 1024*/>
template <typename... Args>
void EventQueue<Type, blockSize>::emplace_back(Args&&... args)
{
	size_t pos = _emptyFront + _size;
	size_t block = pos / blockSize;
	size_t blockIdx = pos % blockSize;
	if (block >= _data.size()) {
		_data.emplace_back(blockSize);
	}
	_data[block][blockIdx] = Type(args...);
	_size++;
}

template <class Type, int blockSize /*= 1024*/>
const Type & EventQueue<Type, blockSize>::front() const
{
	return _data.front()[_emptyFront];
}

template <class Type, int blockSize /*= 1024*/>
Type & EventQueue<Type, blockSize>::front()
{
	return _data.front()[_emptyFront];
}

template <class Type, int blockSize /*= 1024*/>
void EventQueue<Type, blockSize>::pop_front()
{
	if (_size == 0)
		return;
	_emptyFront++;
	_size--;
	if (_emptyFront >= blockSize) {
		_emptyFront -= blockSize;
		size_t blocksUsed = _size / blockSize + 1;
		if (blocksUsed + 1 < _data.size()) {
			// Delete block when there is an empty block waiting, 
			_data.erase(_data.begin());
		}
		else {
			// Move first block to the end of the list
			_data.splice(_data.end(), _data, _data.begin());
		}
	}
}

template <class Type, int blockSize /*= 1024*/>
void EventQueue<Type, blockSize>::clear()
{
	_size = 0;
	_emptyFront = 0;
	_data.clear();
	_data.emplace_back(blockSize);
}

template <class Type, int blockSize /*= 1024*/>
void EventQueue<Type, blockSize>::push_back(const Type &element)
{
	size_t pos = _emptyFront + _size;
	size_t block = pos / blockSize;
	size_t blockIdx = pos % blockSize;
	if (block >= _data.size()) {
		_data.emplace_back(blockSize);
	}
	auto blockIt = _data.end();
	size_t i = _data.size();
	do {
		blockIt--;
		i--;
	} while (i > block);
	(*blockIt)[blockIdx] = element;
	_size++;
	if (_size > _maxSize)
		_maxSize = _size;
	if (_data.size() > _maxBlocks)
		_maxBlocks = _data.size();
}

#endif
