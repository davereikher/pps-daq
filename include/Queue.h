/*
Copyright (c) 2013, Juan Palacios <juan.palacios.puyana@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#define MAX_QUEUE_SIZE 100

template <typename T>
class Queue
{
 public:
	Queue():
	m_bShouldLock(true)
	{}

  T pop() 
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
	bool bShouldLock;
	bool_mutex_.lock();
	bShouldLock = m_bShouldLock;
	bool_mutex_.unlock();
	if(!bShouldLock)
	{
	//	printf("Returning\n");
		return T();
	}
	if(bShouldLock)
	{
	      cond_pop_.wait(mlock);
	}
	//printf("released\n");
    }
    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
	bool bShouldLock;
	bool_mutex_.lock();
	bShouldLock = m_bShouldLock;
	bool_mutex_.unlock();
	if(!bShouldLock)
	{
		return T();
	}
/*	if(bShouldLock)
	{*/
	      cond_pop_.wait(mlock);
//	}
    }
    item = queue_.front();
    queue_.pop();
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_pop_.notify_one();
  }

  int size()
  {
    int size = -1;
    std::unique_lock<std::mutex> mlock(mutex_);
    size = queue_.size();
    mlock.unlock();
    return size;
  }

  int unlock()
  {
     //printf("releasing\n");
	bool_mutex_.lock();
	m_bShouldLock = false;
	bool_mutex_.unlock();
	cond_pop_.notify_one();
  }
//  Queue()=default;
  Queue(const Queue&) = delete;            // disable copying
  Queue& operator=(const Queue&) = delete; // disable assignment
  
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_pop_;
 std::mutex bool_mutex_;
  bool m_bShouldLock;
};

