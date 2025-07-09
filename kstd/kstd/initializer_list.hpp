/*

	The scope of this file is to actually create a class
	which in a second time will allow us to initialize 
	other classes in the following manner:
	kstd::vector<int> v = {1, 2, 3, 4};
		
*/


//
// inclusions
//
#pragma once
#include <ntifs.h>


/* kstd namespace */
inline namespace kstd {


	//
	// This class will define the logic that we need
	// in order to recreate the class std::initializer_list 
	// which is used in usermode c++ STL but in kernel mode
	//
	template <typename T>
	class initializer_list {
	public:

		/* empty constructor */
		constexpr initializer_list( ) noexcept : _first( nullptr ), _last( nullptr ) { };

		/* parametized constructor */
		constexpr initializer_list( const T* _first_arg, const T* _last_arg ) noexcept
			: _first( _first_arg ), _last( _last_arg ) {
		};

		/* begin ( returning the first element ) */
		[[nodiscard]] constexpr const T* begin( ) const noexcept { return this->_first; }

		/* end ( return the last element ) */
		[[nodiscard]] constexpr const T* end( ) const noexcept { return this->_last; }

		/* size ( returns the size of the given array ) */
		[[nodiscard]] constexpr ULONG size( ) const noexcept {
			return static_cast< ULONG >( this->_last - this->_first );
		}

	private:

		/* attributes */
		const T* _first;
		const T* _last;

	};

};