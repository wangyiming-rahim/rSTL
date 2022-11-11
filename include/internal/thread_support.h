#ifndef RSTL_THREAD_SUPPORT_H
#define RSTL_THREAD_SUPPORT_H

#pragma once

#include <cstdint>
#include <mutex>

#include "config.h"

RSTL_NAMESPACE_BEGIN

namespace Thread_Support_Internal {

	inline int32_t atomic_increment(int32_t* p32) noexcept
	{
		return __sync_add_and_fetch(p32, 1);
	}

	inline int32_t atomic_decrement(int32_t* p32) noexcept
	{
		return __sync_add_and_fetch(p32, -1);
	}

	inline bool atomic_compare_and_swap(int32_t* p32, int32_t newValue, int32_t condition)
	{
		return __sync_bool_compare_and_swap(p32, condition, newValue);
	}

	class auto_mutex
	{
	public:
		auto_mutex(std::mutex& mutex) : pMutex(&mutex)
		{
			pMutex->lock();
		}
		~auto_mutex()
		{
			pMutex->unlock();
		}

	protected:
		std::mutex* pMutex;

		auto_mutex(const auto_mutex&) = delete;
		void operator=(const auto_mutex&) = delete;
	};

	class shared_ptr_auto_mutex : public auto_mutex
	{
	public:
		shared_ptr_auto_mutex(const void* pSharedPtr);
		shared_ptr_auto_mutex(const shared_ptr_auto_mutex&) = delete;
		void operator=(shared_ptr_auto_mutex&&) = delete;
	};



}

RSTL_NAMESPACE_END

#endif //RSTL_THREAD_SUPPORT_H
