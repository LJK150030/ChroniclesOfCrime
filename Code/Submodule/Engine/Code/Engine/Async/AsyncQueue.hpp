#pragma once
#include <queue>
#include <mutex>

template <typename T>
class AsyncQueue
{
public:
	void Enqueue( T const &v ); 

	bool Dequeue( T* out ); 

	std::queue<T> m_queue; 
	std::mutex m_mutex;
};

//------------------------------------------------------------------------
template <typename T>
void AsyncQueue<T>::Enqueue( T const& v )
{
	std::lock_guard<std::mutex> locky(m_mutex); 
	m_queue.push( v );
}

//------------------------------------------------------------------------
template <typename T>
bool AsyncQueue<T>::Dequeue( T *out )
{
	std::lock_guard<std::mutex> locky(m_mutex); 
	if (m_queue.empty()) {
		return false; 
	} else {
		*out = m_queue.front(); 
		m_queue.pop();
		return true;
	} 
}
