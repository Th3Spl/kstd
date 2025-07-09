/*

	The scope of this file is to define the backbone structure and
	implementation in kernel mode of std::wstring ( with a lil less functionalities )

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
	// and attributes related to the kernel std::wstring implementation
	//
	class wstring
	{
	public:


		/* empty constructor */
		inline __fastcall wstring( ) { };


		/* non-empty constructor */
		inline __fastcall wstring( const wchar_t* _content )
		{
			/* gettting the length & allocating the needed memory */
			this->length = static_cast< ULONG >( this->__impl_wcslen( _content ) );
			this->content = static_cast< wchar_t* >( ExAllocatePool2( POOL_FLAG_NON_PAGED, ( this->length + 1 ) * sizeof( wchar_t ), ( ULONG )__rdtsc( ) ) );
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
		inline const wchar_t* begin( ) { return this->content; }


		/* end ( this returns the ptr to the last char in the str ) */
		inline const wchar_t* end( ) { return reinterpret_cast< const wchar_t* >( ( PUINT8 )this->content + ( ( this->length - 1 ) * sizeof( wchar_t ) ) ); }


		/* substr ( returns a string substring ) */
		inline const wchar_t* __fastcall nofree_substr( int _start, int _end = -1 )
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
			const wchar_t* res = this->temp_content;
			this->temp_content = 0;
			return res;
		}


		/* substr ( returns a string substring ) */
		inline const wchar_t* __fastcall substr( wchar_t* _buffer, unsigned long _length, int _start, int _end = -1 )
		{
			/* temporary allocation */
			int idx = 0;
			int length_in_wchars = _length / sizeof( wchar_t );

			/* if the _end is not specified we continue to get the string till the end */
			if ( _end == -1 || _end > length_in_wchars ) _end = this->length;
			if ( _start < 0 || _start >= _end ) return nullptr;

			/* getting only the charaters that we're interested in */
			for ( int i = _start; i < _end && idx < length_in_wchars - 1; i++ )
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
		inline const wchar_t* __fastcall c_str( ) { return this->content; };


		/* length */
		inline ULONG __fastcall size( ) { return this->length; }


		/* empty */
		inline bool __fastcall empty( ) { return this->length <= 0; }


		/* to unicode string */
		inline void to_unicode( PUNICODE_STRING ustr )
		{
			if ( this->length <= 0 ) return;

			ustr->Buffer = this->content;
			if ( this->content )
			{
				USHORT v3 = 2 * (USHORT)wcslen( this->content );
				if ( v3 >= 0xFFFE )
					v3 = 2;

				ustr->Length = v3;
				ustr->MaximumLength = v3 + 2;
			}
		}

		
		/* operator ( char ) */
		inline wchar_t& __fastcall operator[]( unsigned long idx ) {
			return this->content[ idx ];
		}


		/* operator ( const char ) */
		inline const wchar_t& __fastcall operator[]( unsigned long idx ) const
		{
			return this->content[ idx ];
		}


		/* class destructor */
		inline ~wstring( )
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
		wchar_t* content = nullptr;
		wchar_t* temp_content = nullptr;


		/* strlen implmentation */
		size_t __impl_wcslen( const wchar_t* str )
		{
			const wchar_t* s = str;
			while ( *s )
				s++;
			return ( size_t )( s - str );
		}


		/* temporary allocation ( can be done manually ) */
		wchar_t* __temp_alloc__( )
		{
			wchar_t* res = static_cast< wchar_t* >( ExAllocatePool2( POOL_FLAG_NON_PAGED, ( this->length + 1 ) * sizeof( wchar_t ), ( ULONG )__rdtsc( ) ) );
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


		/* to unicode_string [ TODO: fix this ] */
		inline UNICODE_STRING __fastcall to_unicode( )
		{
			if ( this->length <= 0 ) return { 0, 0, nullptr };

			return {
				sizeof( this->content ) - sizeof( this->content[ 0 ] ),
				sizeof( this->content ) / sizeof( wchar_t ),
				( wchar_t* )this->content
			};
		}

	};

}