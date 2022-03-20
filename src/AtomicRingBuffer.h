#ifndef _ATOMIC_RING_BUFFER_H_
#define _ATOMIC_RING_BUFFER_H_

#include <atomic>
#include <cassert>


template <typename T>
class AtomicRingBuffer
{
public:
    AtomicRingBuffer(int capacity) : 
        mBufferLength(capacity)
        {
            
            assert(capacity > 0);
            mBuffer = new T[capacity];
        }
    ~AtomicRingBuffer()
    {
        delete[] mBuffer;
    }
    
    bool push(const T& element)
    {
        size_t currentWritePointer = mWritePointer.load();
        size_t nextWritePointer = increment(currentWritePointer);
        if (nextWritePointer != mReadPointer.load())
        {
            mBuffer[currentWritePointer] = element;
            mWritePointer.store(nextWritePointer);
            return true;
        }
        return false;
    }
    
    bool pop(T& element)
    {
        const size_t readPointer = mReadPointer.load();
        if (readPointer == mWritePointer.load())
        {
            return false;
        }
            
        element = mBuffer[readPointer];
        mReadPointer.store(increment(readPointer));
        return true;
    }
    
    bool wasEmpty() const
    {
        return mWritePointer.load() == mReadPointer.load();
    }
    
    bool wasFull() const
    {
        const size_t nextWritePointer = increment(mWritePointer.load());
        return (nextWritePointer == mReadPointer.load());
    }
    
private:
    
    int mBufferLength = 0;
    std::atomic<size_t> mWritePointer = 0;
    std::atomic<size_t> mReadPointer = 0;
    T* mBuffer = 0;
    
    size_t increment(size_t index) const
    {
        index++;
        while (index < 0)
            index += mBufferLength;
        return index %= mBufferLength;
    }

};

#endif // _ATOMIC_RING_BUFFER_H_
