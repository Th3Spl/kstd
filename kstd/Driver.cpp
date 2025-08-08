
//
// inclusions
//
#include <ntifs.h>
#include "kstd/kiostream.hpp"


//
// entry
//
NTSTATUS DriverEntry( PDRIVER_OBJECT drv_obj, PUNICODE_STRING reg_path )
{
	/* unref params */
	UNREFERENCED_PARAMETER( drv_obj );
	UNREFERENCED_PARAMETER( reg_path );


	/* vars */
	NTSTATUS status = STATUS_SUCCESS;
	kstd::string my_string = "abcdefghijklmnopqrstuvwxyz";
	kstd::wstring my_wstring = L"abcdefghijklmnopqrstuvwxyz";
	kstd::vector<int> my_vector = { 1, 2 };
	UNICODE_STRING my_unicode;


	/* string */
	DbgPrintEx( 0, 0, "(~) IsEmpty: %s", my_string.empty( ) ? "IsEmpty" : "NotEmpty" );
	DbgPrintEx( 0, 0, "(~) Test  : %s", my_string.c_str( ) );
	DbgPrintEx( 0, 0, "(~) Test2 : %c", my_string[2] );
	DbgPrintEx( 0, 0, "(~) Test3 : %c", *( char* )my_string.end( ) );
	DbgPrintEx( 0, 0, "(~) Test4 : %s", my_string.nofree_substr( 1, my_string.size( ) - 2 ) );
	DbgPrintEx( 0, 0, "\n" );


	/* wstring */
	my_wstring.to_unicode( &my_unicode );
	DbgPrintEx( 0, 0, "(~) wstring now:" );
	DbgPrintEx( 0, 0, "(~) IsEmpty : %s", my_wstring.empty( ) ? "IsEmpty" : "NotEmpty" );
	DbgPrintEx( 0, 0, "(~) Test    : %ws", my_wstring.c_str( ) );
	DbgPrintEx( 0, 0, "(~) Test2   : %wc", my_wstring[2] );
	DbgPrintEx( 0, 0, "(~) Test3   : %wc", *( char* )my_wstring.end( ) );
	DbgPrintEx( 0, 0, "(~) Test4   : %ws", my_wstring.nofree_substr( 1, my_wstring.size( ) - 2 ) );
	DbgPrintEx( 0, 0, "(~) Test5   : %wZ", my_unicode );


	/* vector */
	for ( int i = 0; i < 10; i++ )
	{
		my_vector.push_back( i );
	}

	return status;
}