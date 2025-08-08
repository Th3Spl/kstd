/* 
	
	The scope of this file is to define the backbone structure and 
	implementation in kernel mode of std::string ( with a lil less functionalities )

*/


//
// inclusions 
//
#pragma once
#include <ntifs.h>
#include <ntstrsafe.h>



/* kstd namespace */
inline namespace kstd {


	//
	// This class will define and contain all the needed functions
	// and attributes related to the kernel std::string implementation
	//
	class string
	{
	public:


		/* empty constructor */
		inline __fastcall string( ) { };


		/* non-empty constructor */
		inline __fastcall string( const char* _content )
		{
			/* gettting the length & allocating the needed memory */
			this->length = static_cast< ULONG >( this->__impl_strlen( _content ) );
			this->content = static_cast< char* >( ExAllocatePool2( POOL_FLAG_NON_PAGED, this->length + 1, ( ULONG )__rdtsc( ) ) );
			if ( !this->content ) return;

			/* filling the buffer */
			for ( unsigned long i = 0; i < this->length; i++ )
			{
				this->content[ i ] = _content[ i ];
			}

			/* ending the string with the right terminator */
			this->content[ this->length ] = '\0';
		}


		/* start ( this returns the start addr of the str alloc ) */
		inline const char* begin( ) { return this->content; }


		/* end ( this returns the ptr to the last char in the str ) */
		inline const char* end( ) { return reinterpret_cast< const char* >( ( PUINT8 )this->content + ( this->length - 1 ) ); }


		/* substr ( returns a string substring ) */
		inline const char* __fastcall nofree_substr( int _start, int _end = -1 )
		{
			/* temporary allocation */
			int idx = 0;
			this->temp_content = this->__temp_alloc__( );

			/* if the _end is not specified we continue to get the string till the end */
			if ( _end == -1 ) _end = this->length;

			/* getting only the charaters that we're interested in */
			for ( int i = _start; i < _end; i++ )
			{
				temp_content[ idx++ ] = this->content[ i ];
			}

			/* clenaup and return */
			const char* res = this->temp_content;
			this->temp_content = 0;
			return res;
		}


		/* substr ( returns a string substring ) */
		inline const char* __fastcall substr( char* _buffer, unsigned long _length, int _start, int _end = -1 )
		{
			/* temporary allocation */
			unsigned int idx = 0;

			/* if the _end is not specified we continue to get the string till the end */
			if ( _end == -1 || _end > static_cast< int >( this->length ) ) _end = this->length;
			if ( _start < 0 || _start >= _end ) return nullptr;

			/* getting only the charaters that we're interested in */
			for ( int i = _start; i < _end && idx < _length - 1; i++ )
			{
				/* saving everything into the user provided buffer */
				_buffer[ idx++ ] = this->content[ i ];
			}

			/* ending the string correctly */
			_buffer[ idx ] = '\0';

			/* clenaup and return */
			return _buffer;
		}


		/* data */
		inline PVOID __fastcall data( ) { return static_cast< PVOID >( this->content ); }


		/* c-str */
		inline const char* __fastcall c_str( ) { return this->content; };


		/* length */
		inline ULONG __fastcall size( ) { return this->length; }


		/* empty */
		inline bool __fastcall empty( ) { return this->length <= 0; }


		/* operator ( char ) */
		inline char& __fastcall operator[]( unsigned long idx ) {
			return this->content[ idx ];
		}


		/* operator ( const char ) */
		inline const char& __fastcall operator[]( unsigned long idx ) const
		{
			return this->content[ idx ];
		}


		/* class destructor */
		inline ~string( )
		{
			/* freeing the content */
			if ( this->content ) ExFreePoolWithTag( this->content, 0 );

			/* freeing the temporary content */
			if ( this->temp_content ) ExFreePoolWithTag( this->temp_content, 0 );

			return;
		}


	private:


		/* attributes */
		ULONG length = 0;
		char* content = nullptr;
		char* temp_content = nullptr;


		/* strlen implmentation */
		size_t __impl_strlen( const char* str )
		{
			const char* s = str;
			while ( *s )
				s++;
			return ( size_t )( s - str );
		}


		/* temporary allocation ( can be done manually ) */
		char* __temp_alloc__( )
		{
			char* res = static_cast< char* >( ExAllocatePool2( POOL_FLAG_NON_PAGED, this->length + 1, ( ULONG )__rdtsc( ) ) );
			if ( !res ) return nullptr;

			res[ this->length ] = '\0';
			return res;
		}


		/* temporary allocation clenaup ( can be done manually ) */
		void __temp_cleanup__( )
		{
			/* freeing */
			if ( this->temp_content )
				ExFreePoolWithTag( this->temp_content, 0 );

			return;
		}

	};

};