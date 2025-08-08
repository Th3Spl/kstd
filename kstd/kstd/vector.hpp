/*

	The scope of this file is to define the backbone structure
	implementation in kernel mode of std::vector<> ( with a lil less functionalities )

*/


//
// inclusions
//
#pragma once
#include "ntifs.h"
#include "ntstrsafe.h"
#include ".\initializer_list.hpp"



/* kstd namespace */
inline namespace kstd {


	//
	// This class will define and contain all the needed functions 
	// and logic to implement the class std::vector in kernel mode 
	// allowing the user to utilize dynamic arrays in kernel mode
	// 
	// Capacity = 2 * each realloc to avoid overhead usage of ExAllocatePool2
	//
	template <typename T>
	class vector
	{
	public:

		/* empty constructor */
		inline __fastcall vector()
		{
			this->buffer = nullptr;
			this->size = 0;
			this->length = 0;
			this->capacity = 0;
			this->initialized = false;
		}

		/* variadic constructor for { 1, 2 } syntax */
		template<typename... Args>
		inline __fastcall vector(Args... args)
		{
			this->buffer = nullptr;
			this->size = 0;
			this->length = 0;
			this->capacity = 0;
			this->initialized = false;

			if (!__ensure_initialized__()) return;

			this->length = sizeof...(args);
			this->size = this->length * sizeof(T);
			this->capacity = this->size * 2;

			if (this->capacity < 64) this->capacity = 64;

			this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
			if (!this->buffer)
			{
				this->length = 0;
				this->size = 0;
				this->capacity = 0;
				return;
			}

			__init_values__(0, args...);
			this->initialized = true;
		}

		/* size constructor */
		inline __fastcall vector(ULONG count)
		{
			this->length = count;
			this->size = this->length * sizeof(T);
			this->capacity = this->size * 2;

			this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
			if (!this->buffer) return;

			memset(this->buffer, 0, this->size);
			this->initialized = true;
		}

		/* copy constructor */
		inline __fastcall vector(const vector<T>& other)
		{
			this->length = other.length;
			this->size = other.size;
			this->capacity = other.capacity;

			this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
			if (!this->buffer) return;

			memcpy(this->buffer, other.buffer, this->size);
			this->initialized = true;
		}

		/* assignment operator */
		inline vector<T>& operator=(const vector<T>& other)
		{
			if (this != &other)
			{
				__try
				{
					if (this->buffer)
						ExFreePoolWithTag(this->buffer, 0);

					this->length = other.length;
					this->size = other.size;
					this->capacity = other.capacity;

					if (other.buffer && other.capacity > 0)
					{
						this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
						if (this->buffer)
						{
							memcpy(this->buffer, other.buffer, this->size);
							this->initialized = true;
						}
						else
						{
							this->length = 0;
							this->size = 0;
							this->capacity = 0;
							this->initialized = false;
						}
					}
					else
					{
						this->buffer = nullptr;
						this->initialized = other.initialized;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					this->buffer = nullptr;
					this->length = 0;
					this->size = 0;
					this->capacity = 0;
					this->initialized = false;
				}
			}
			return *this;
		}

		/* assignment from multiple values */
		template<typename... Args>
		inline vector<T>& assign(Args... args)
		{
			__try
			{
				if (this->buffer)
					ExFreePoolWithTag(this->buffer, 0);

				this->length = sizeof...(args);
				this->size = this->length * sizeof(T);
				this->capacity = this->size * 2;

				if (this->capacity < 64) this->capacity = 64;

				this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
				if (!this->buffer)
				{
					this->length = 0;
					this->size = 0;
					this->capacity = 0;
					this->initialized = false;
					return *this;
				}

				__init_values__(0, args...);
				this->initialized = true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				this->buffer = nullptr;
				this->length = 0;
				this->size = 0;
				this->capacity = 0;
				this->initialized = false;
			}

			return *this;
		}

		/* push back ( adding an item ) */
		inline bool push_back(T item)
		{
			if (!__ensure_initialized__()) return false;

			if (this->size + sizeof(T) > this->capacity)
			{
				if (!this->__realloc__()) return false;
			}

			this->buffer[this->length] = item;
			this->size += sizeof(T);
			this->length++;

			return true;
		}

		/* pop back ( removing an item ) */
		inline T pop_back()
		{
			if (!this->initialized || this->length == 0) return T{};

			T ret = this->buffer[this->length - 1];
			this->length--;
			this->size -= sizeof(T);

			return ret;
		}

		/* insert at position */
		inline bool insert(ULONG pos, const T& value)
		{
			if (!__ensure_initialized__()) return false;
			if (pos > this->length) return false;

			if (this->size + sizeof(T) > this->capacity)
			{
				if (!this->__realloc__()) return false;
			}

			for (ULONG i = this->length; i > pos; i--)
				this->buffer[i] = this->buffer[i - 1];

			this->buffer[pos] = value;
			this->length++;
			this->size += sizeof(T);

			return true;
		}

		/* erase at position */
		inline bool erase(ULONG pos)
		{
			if (!this->initialized || pos >= this->length) return false;

			for (ULONG i = pos; i < this->length - 1; i++)
				this->buffer[i] = this->buffer[i + 1];

			this->length--;
			this->size -= sizeof(T);

			return true;
		}

		/* resize vector */
		inline bool resize(ULONG new_size)
		{
			if (!__ensure_initialized__()) return false;

			if (new_size * sizeof(T) > this->capacity)
			{
				ULONG new_capacity = new_size * sizeof(T) * 2;
				if (new_capacity < 64) new_capacity = 64;

				T* new_buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, new_capacity, (ULONG)__rdtsc());
				if (!new_buffer) return false;

				if (this->buffer)
				{
					memcpy(new_buffer, this->buffer, this->size);
					ExFreePoolWithTag(this->buffer, 0);
				}
				this->buffer = new_buffer;
				this->capacity = new_capacity;
			}

			if (new_size > this->length)
				memset(&this->buffer[this->length], 0, (new_size - this->length) * sizeof(T));

			this->length = new_size;
			this->size = new_size * sizeof(T);

			return true;
		}

		/* reserve capacity */
		inline bool reserve(ULONG new_capacity)
		{
			if (!__ensure_initialized__()) return false;
			if (new_capacity <= this->capacity) return true;

			T* new_buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, new_capacity, (ULONG)__rdtsc());
			if (!new_buffer) return false;

			if (this->buffer)
			{
				memcpy(new_buffer, this->buffer, this->size);
				ExFreePoolWithTag(this->buffer, 0);
			}
			this->buffer = new_buffer;
			this->capacity = new_capacity;

			return true;
		}

		/* get front element */
		inline T& front()
		{
			if (!this->initialized || this->length == 0)
			{
				static T dummy = T{};
				return dummy;
			}
			return this->buffer[0];
		}

		/* get back element */
		inline T& back()
		{
			if (!this->initialized || this->length == 0)
			{
				static T dummy = T{};
				return dummy;
			}
			return this->buffer[this->length - 1];
		}

		/* at with bounds checking */
		inline T& at(ULONG pos)
		{
			if (!this->initialized || pos >= this->length)
			{
				static T dummy = T{};
				return dummy;
			}
			return this->buffer[pos];
		}

		/* find element */
		inline ULONG find(const T& value)
		{
			if (!this->initialized) return 0;

			for (ULONG i = 0; i < this->length; i++)
			{
				if (this->buffer[i] == value)
					return i;
			}
			return this->length;
		}

		/* contains element */
		inline bool contains(const T& value)
		{
			return find(value) != this->length;
		}

		/* clear */
		inline void clear(bool dealloc = false)
		{
			if (!this->initialized) return;

			this->size = 0;
			this->length = 0;

			if (dealloc)
			{
				if (this->buffer)
				{
					ExFreePoolWithTag(this->buffer, 0);
					this->buffer = nullptr;
					this->capacity = 0;
					this->initialized = false;
				}
			}
			else if (this->buffer)
			{
				memset(this->buffer, 0, this->capacity);
			}
		}

		/* shrink to fit */
		inline bool shrink_to_fit()
		{
			if (!this->initialized || this->size == this->capacity || this->size == 0) return true;

			T* new_buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->size, (ULONG)__rdtsc());
			if (!new_buffer) return false;

			memcpy(new_buffer, this->buffer, this->size);
			ExFreePoolWithTag(this->buffer, 0);
			this->buffer = new_buffer;
			this->capacity = this->size;

			return true;
		}

		/* get data */
		inline T* data() { return this->buffer; }

		/* get the size ( in items ) */
		inline ULONG len() { return this->length; }

		/* get the size in bytes */
		inline ULONG raw_size() { return this->size; }

		/* get the current capacity */
		inline ULONG raw_capacity() { return this->capacity; }

		/* check if the vector is empty */
		inline bool empty() { return this->length == 0; }

		/* check if vector is properly initialized */
		inline bool is_valid() { return this->initialized && this->buffer != nullptr; }

		/* operator [] with index */
		inline T& __fastcall operator[](unsigned long idx)
		{
			if (!this->initialized || idx >= this->length)
			{
				static T dummy = T{};
				return dummy;
			}
			return this->buffer[idx];
		}

		/* operator [] const with index */
		inline const T& __fastcall operator[](unsigned long idx) const
		{
			if (!this->initialized || idx >= this->length)
			{
				static T dummy = T{};
				return dummy;
			}
			return this->buffer[idx];
		}

		/* begin iterator (pointer) */
		inline T* begin() { return this->buffer; }

		/* end iterator (pointer) */
		inline T* end() { return this->buffer + this->length; }

		/* manual cleanup - call this instead of relying on destructor */
		inline void cleanup()
		{
			if (this->initialized && this->buffer)
			{
				ExFreePoolWithTag(this->buffer, 0);
				this->buffer = nullptr;
				this->initialized = false;
				this->size = 0;
				this->length = 0;
				this->capacity = 0;
			}
		}

		/* destructor - empty to avoid dynamic initialization */
		~vector() = default;

	private:

		/* value initializer helper */
		void __init_values__(size_t index) { index; }
		template<typename U, typename... Params>
		void __init_values__(size_t index, U&& first, Params&&... rest)
		{
			this->buffer[index] = first;
			__init_values__(index + 1, rest...);
		}

		/* auto-initialize check - handles uninitialized global vectors */
		inline bool __ensure_initialized__()
		{
			if (this->initialized != true && this->initialized != false)
			{
				__try
				{
					this->buffer = nullptr;
					this->size = 0;
					this->length = 0;
					this->capacity = 0;
					this->initialized = false;
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					return false;
				}
			}

			if (!this->initialized)
			{
				this->capacity = 64;
				this->buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, this->capacity, (ULONG)__rdtsc());
				if (!this->buffer)
				{
					this->capacity = 0;
					return false;
				}
				this->initialized = true;
			}

			return true;
		}

		/* realloc --> copy */
		inline bool __realloc__()
		{
			ULONG new_capacity = this->capacity == 0 ? 64 : 2 * this->capacity;

			T* new_buffer = (T*)ExAllocatePool2(POOL_FLAG_NON_PAGED, new_capacity, (ULONG)__rdtsc());
			if (!new_buffer) return false;

			if (this->buffer && this->size > 0)
			{
				memcpy(new_buffer, this->buffer, this->size);
				ExFreePoolWithTag(this->buffer, 0);
			}

			this->buffer = new_buffer;
			this->capacity = new_capacity;

			return true;
		}

		/* attributes */
		T* buffer;
		ULONG size;
		ULONG length;
		ULONG capacity;
		bool initialized;

	};
}