#pragma once
#include <memory>
#include <mutex>

// notes from Circular Buffer
// https://en.wikipedia.org/wiki/Circular_buffer

template <class T>

class CircularBuffer
{
public:
	explicit CircularBuffer(size_t size);

public:
	void	Put(T item);
	T		Get();
	void	Reset();
	bool	IsEmpty() const;
	bool	IsFull() const;
	size_t	Capacity() const;
	size_t	Size() const;

private:
	std::mutex m_mutex;
	std::unique_ptr<T[]> m_buf;
	size_t m_head = 0;
	size_t m_tail = 0;
	const size_t m_maxSize;
	bool m_full = 0;
};

template <class T>
CircularBuffer<T>::CircularBuffer(size_t size):
	m_buf(std::unique_ptr<T[]>(new T[size])),
	m_maxSize(size) { }

template <class T>
void CircularBuffer<T>::Put(T item)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_buf[m_head] = item;

	if(m_full)
	{
		//possible loss of data. will need to check this later
		m_tail = (m_tail + 1) % m_maxSize;
	}

	m_head = (m_head + 1) % m_maxSize;

	m_full = m_head == m_tail;
}

template <class T>
T CircularBuffer<T>::Get()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if(IsEmpty())
	{
		return T();
	}

	//Read data and advance the tail (we now have a free space)
	auto val = m_buf[m_tail];
	m_full = false;
	m_tail = (m_tail + 1) % m_maxSize;

	return val;
}

template <class T>
void CircularBuffer<T>::Reset()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_head = m_tail;
	m_full = false;
}

template <class T>
bool CircularBuffer<T>::IsEmpty() const
{
	//using the paradigm when the head and tail are equal, we are empty
	return (!m_full && (m_head == m_tail));
}

template <class T>
bool CircularBuffer<T>::IsFull() const
{
	return m_full;
}

template <class T>
size_t CircularBuffer<T>::Capacity() const
{
	return m_maxSize;
}

template <class T>
size_t CircularBuffer<T>::Size() const
{
	size_t size = m_maxSize;

	if(!m_full)
	{
		if(m_head >= m_tail)
		{
			size = m_head - m_tail;
		}
		else
		{
			size = m_maxSize + m_head - m_tail;
		}
	}

	return size;
}

