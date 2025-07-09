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
	inline __fastcall vector( ) { this->initialized = true; };

	/* value initializer for { 1, 2 } constructor*/
	void __init_values__( size_t index ) { index; }
	template<typename T, typename... Params>
	void __init_values__( size_t index, T&& first, Params&&... rest )
	{
		this->buffer[ index ] = first;
		__init_values__( index + 1, rest... );
	}

	/* { 1, 2 } constructor */
	template<typename... params>
	inline __fastcall vector( params... init_params )
	{
		/* getting the current size */
		this->length = sizeof...( init_params );
		this->size = this->length * sizeof( T );
		this->capacity = this->size * 2;

		/* allocating the needed memory */
		this->buffer = ( T* )ExAllocatePool2( POOL_FLAG_NON_PAGED, this->capacity, ( ULONG )__rdtsc( ) );
		if ( !this->buffer ) return;

		/* initializing the vector with the provided values */
		__init_values__( 0, init_params... );

		this->initialized = true;
		return;
	}

	/* push back ( adding an item ) */
	inline void push_back( T item )
	{
		/* rellocating the buffer if needed */
		if ( this->size + sizeof( T ) > this->capacity )
			this->__realloc__( );

		/* adding the item */
		if ( this->size + sizeof( T ) <= this->capacity )
		{
			this->buffer[ this->length ] = item;
			this->size += sizeof( T );
			this->length++;
		}
		
		return;
	}

	/* pop back ( removing an item ) */
	inline T pop_back( )
	{
		T ret = this->buffer[ this->length - 1 ];
		memset( this->buffer[ this->length - 1 ], 0, sizeof( T ) );

		return ret;
	}

	/* clear */
	inline void clear( bool dealloc = false )
	{
		/* fixing */
		this->size = 0;
		this->length = 0;
		
		/* action */
		if ( dealloc ) ExFreePoolWithTag( this->buffer, 0 );
		else memset( this->buffer, 0, this->capacity );

		return;
	}

	/* get data */
	inline T* data( ) { return this->buffer; }

	/* get the size ( in items ) */
	inline ULONG len( ) { return this->length; }

	/* get the size in bytes */
	inline ULONG raw_size( ) { return this->size; }

	/* get the current capacity */
	inline ULONG raw_capacity() { return this->capacity; }

	/* check if the vector is empty */
	inline bool empty( ) { return this->length > 0; }

	/* operator [] with index */
	inline T __fastcall operator[]( unsigned long idx )
	{
		return this->buffer[ idx ];
	}

	/* operator [] const with index */
	inline const T __fastcall operator[]( unsigned long idx ) const
	{
		return this->buffer[ idx ];
	}

	/* destructor */
	~vector( ) { ExFreePoolWithTag( this->buffer, 0 ); }

private:

	/* attributes */
	T* buffer = nullptr;
	ULONG size = 0;
	ULONG length = 0;
	ULONG capacity = 0;
	bool initialized = false;

	/* realloc --> copy */
	inline void __realloc__( )
	{
		/* calculating the new capacity */
		ULONG new_capacity = 2 * ( this->capacity + sizeof( T ) );
		
		/* allocating the new buffer */
		T* new_buffer = (T*)ExAllocatePool2( POOL_FLAG_NON_PAGED, new_capacity, ( ULONG )__rdtsc( ) );
		if ( !new_buffer ) return;

		/* copying the mem */
		memcpy( new_buffer, this->buffer, this->size );
		
		/* swapping the buffer */
		ExFreePoolWithTag( this->buffer, 0 );
		this->buffer = new_buffer;
		this->capacity = new_capacity;

		return;
	}

};