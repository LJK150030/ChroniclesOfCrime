#include "Engine/Architecture/AsyncRingBuffer.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

AsyncRingBuffer::AsyncRingBuffer()
{
}

AsyncRingBuffer::~AsyncRingBuffer()
{
	Deinit();
}

bool AsyncRingBuffer::Init(size_t size_in_bytes)
{
	// todo: make sure deinit'd handle it being init'd

	m_buffer = (byte*) malloc( size_in_bytes ); 
	m_byteSize = size_in_bytes;
	return m_buffer == NULL;
}

void AsyncRingBuffer::Deinit()
{
	// frees memory
	if (m_buffer != nullptr) {
		free( m_buffer ); 
		m_buffer = nullptr; 
		m_byteSize = 0U; 
	}
}

bool AsyncRingBuffer::Write(void const* address_of_data, size_t byte_size)
{
	// Given a pointer to the address of data (address_of_data)
	// as well as the total size of the object, (byte_size)
	// I want to place this data into the container
		// ex, we create a local Vec3, give the address to the ringbuffer
		// and mem copy that data into the buffer (duplicating the data).
		// Once out of scope, and the local Vec3 is gone, the ring
		// buffer should still have

	void* available_address = LockWrite(byte_size);
	void* saved_address = memcpy(available_address, address_of_data, byte_size);
	UnlockWrite(available_address);

	const bool find_space = available_address != nullptr;
	const bool copied_properly = available_address == saved_address;
	
	return find_space && copied_properly;
}

bool AsyncRingBuffer::TryWrite(void const* address_of_data, size_t byte_size)
{
	void* available_address = TryLockWrite(byte_size);

	if(available_address == nullptr)
	{
		return false;
	}
	
	void* saved_address = memcpy(available_address, address_of_data, byte_size);
	UnlockWrite(available_address);

	const bool find_space = available_address != nullptr;
	const bool copied_properly = available_address == saved_address;

	return find_space && copied_properly;
}

// returns the pointer where the memory can be saved
void* AsyncRingBuffer::TryLockWrite(size_t size)
{
	ASSERT_OR_DIE( size < (1 << 31), "size is larger than a 31 bit representation" );
	ASSERT_RECOVERABLE(size != 0, "size is zero")

	const size_t header_size = sizeof(ringbuffer_header_t);
	const size_t total_size = header_size + size; 
	const size_t needed_size = total_size + header_size; 
	
	std::scoped_lock lock( m_lock ); 
	size_t free_realestate = GetWritableSpace();
	if (free_realestate < needed_size) {
		return nullptr; 
	}
	ASSERT_RECOVERABLE( free_realestate <= m_byteSize, "GetWritableSpace is too large" );

	// we want to know if we can store a header, data size,
	// and a foreseeable second head
	const size_t next_data_slot_idx = m_writeHeadIdx + total_size + header_size;

	//If we are at the end of the ring buffer and our data is too large
	if (next_data_slot_idx > m_byteSize) {

		// need to wrap, warn the read to go back to the start
		byte* skip_buf = m_buffer + m_writeHeadIdx;
		ringbuffer_header_t* skip_header = (ringbuffer_header_t*) skip_buf; 
		skip_header->size = 0;  // 0 means skip; 
		skip_header->ready = 1; 

		m_writeHeadIdx = 0;
		
	}

	// Check needed, because we are now behind the read head
	if (GetWritableSpace() < needed_size) {
		return nullptr; 
	}

	// my usable buffer where I can start to write
	byte* cur_buf = m_buffer + m_writeHeadIdx;
	ringbuffer_header_t* head = (ringbuffer_header_t*) cur_buf; 
	head->size = (unsigned int) size;
	head->ready = 0; 

	// move the write head up the total size
	m_writeHeadIdx += (unsigned int) total_size; 

	return head + 1; 
}

void* AsyncRingBuffer::LockWrite(size_t size)
{
	ASSERT_OR_DIE( size < (1 << 31), "size is larger than a 31 bit representation" );

	void* ptr = TryLockWrite(size); 
	while (ptr == nullptr) {
		std::this_thread::yield(); 
		ptr = TryLockWrite(size); 
	}

	return ptr; 
}

void AsyncRingBuffer::UnlockWrite(void* ptr)
{
	ringbuffer_header_t* write_head = (ringbuffer_header_t*) ptr; 
	--write_head; 

	write_head->ready = 1; 
}

bool AsyncRingBuffer::Read(void* out_data)
{
	size_t buf_size = 0;
	void* data_from_buf = LockRead(&buf_size);
	UnlockRead(data_from_buf);
	
	return memcpy(out_data, data_from_buf, buf_size);
}

bool AsyncRingBuffer::TryRead(void* out_data)
{
	size_t buf_size = 0;
	void* data_from_buf = LockRead(&buf_size);

	if(data_from_buf == nullptr)
	{
		return false;
	}
	
	UnlockRead(data_from_buf);

	return memcpy(out_data, data_from_buf, buf_size);
}

void* AsyncRingBuffer::TryLockRead(size_t* out_size)
{
	// MULTIPLE PRODUCER CASE

	//if (m_readHeadIdx == m_writeHeadIdx) {
	//	return nullptr;
	//}

	
	std::scoped_lock lock( m_lock );

	
	while (true) 
	{
		if (m_readHeadIdx == m_writeHeadIdx) {
			return nullptr; 
		}

		ringbuffer_header_t* read_header = (ringbuffer_header_t*)(m_buffer + m_readHeadIdx); 
		ASSERT_RECOVERABLE(read_header->size <= 1000, "trying to read an abnormaly large message")

		if (read_header->ready) {
			if (read_header->size == 0) {
				// skip case
				m_readHeadIdx = 0; 
			} else {
				// valid case
				if(out_size != nullptr)
				{
					*out_size = read_header->size;
				}
				ASSERT_OR_DIE(!m_doubelLockingTest, "Double locking test");
				m_doubelLockingTest = true;
				
				// SINGLE CONSUMER CASE - nothing else happens
				void* ret_buf = read_header + 1; 
				return ret_buf;
			} 
		} else {

			return nullptr;
		}
	}
}

void* AsyncRingBuffer::LockRead(size_t* out_size)
{
	void* ptr = TryLockRead(out_size); 
	while (ptr == nullptr) {
		std::this_thread::yield(); 
		ptr = TryLockRead(out_size); 
	}

	return ptr; 
}

void AsyncRingBuffer::UnlockRead(void* ptr)
{
	std::scoped_lock lock( m_lock ); 
	m_doubelLockingTest = false;
	
	ringbuffer_header_t* read_head = (ringbuffer_header_t*) ptr;
	read_head--; 

	ASSERT_OR_DIE ((m_buffer + m_readHeadIdx) == (void*) read_head, 
		"Attempting to read outside of buffer");

	size_t block_size = sizeof(ringbuffer_header_t) + read_head->size; 
	memset( read_head, 0xffffffff, block_size );

	m_readHeadIdx += static_cast<uint>(block_size); 

	ASSERT_RECOVERABLE( m_readHeadIdx <= m_byteSize, "attempting to read outside of ring buffer")
}

size_t AsyncRingBuffer::GetWritableSpace() const
{
	size_t remaining = 0; 
	if (m_writeHeadIdx >= m_readHeadIdx) {
		remaining = m_byteSize - m_writeHeadIdx; 
		remaining += m_readHeadIdx; 
	} else {
		remaining = m_readHeadIdx - m_writeHeadIdx; 
	}

	return remaining; 
}