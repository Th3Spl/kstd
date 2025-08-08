
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

    kstd::vector<int> my_vector ; // must innitialize before usage.

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



    /* vector testing */
    DbgPrintEx(0, 0, "(~) vector now:\n");
    DbgPrintEx(0, 0, "(~) IsEmpty : %s\n", my_vector.empty() ? "IsEmpty" : "NotEmpty");
    DbgPrintEx(0, 0, "(~) Length  : %d\n", my_vector.len());
    DbgPrintEx(0, 0, "(~) Size    : %d\n", my_vector.raw_size());
    DbgPrintEx(0, 0, "(~) Capacity: %d\n", my_vector.raw_capacity());
    DbgPrintEx(0, 0, "(~) Front   : %d\n", my_vector.front());
    DbgPrintEx(0, 0, "(~) Back    : %d\n", my_vector.back());
    DbgPrintEx(0, 0, "(~) At(0)   : %d\n", my_vector.at(0));
    DbgPrintEx(0, 0, "(~) [1]     : %d\n", my_vector[1]);
    DbgPrintEx(0, 0, "\n");

    /* push_back test */
    my_vector.push_back(3);
    my_vector.push_back(4);
    my_vector.push_back(5);
    DbgPrintEx(0, 0, "(~) After push_back(3,4,5):\n");
    for (ULONG i = 0; i < my_vector.len(); i++)
    {
        DbgPrintEx(0, 0, "(~) [%d]     : %d\n", i, my_vector[i]);
    }
    DbgPrintEx(0, 0, "\n");

    /* pop_back test */
    int popped = my_vector.pop_back();
    DbgPrintEx(0, 0, "(~) Popped  : %d\n", popped);
    DbgPrintEx(0, 0, "(~) NewLen  : %d\n", my_vector.len());
    DbgPrintEx(0, 0, "\n");

    /* insert test */
    my_vector.insert(2, 99);
    DbgPrintEx(0, 0, "(~) After insert(2, 99):\n");
    for (ULONG i = 0; i < my_vector.len(); i++)
    {
        DbgPrintEx(0, 0, "(~) [%d]     : %d\n", i, my_vector[i]);
    }
    DbgPrintEx(0, 0, "\n");

    /* erase test */
    my_vector.erase(2);
    DbgPrintEx(0, 0, "(~) After erase(2):\n");
    for (ULONG i = 0; i < my_vector.len(); i++)
    {
        DbgPrintEx(0, 0, "(~) [%d]     : %d\n", i, my_vector[i]);
    }
    DbgPrintEx(0, 0, "\n");

    /* find test */
    ULONG pos = my_vector.find(3);
    DbgPrintEx(0, 0, "(~) Find(3) : %d\n", pos);
    DbgPrintEx(0, 0, "(~) Contains(3): %s\n", my_vector.contains(3) ? "True" : "False");
    DbgPrintEx(0, 0, "(~) Contains(99): %s\n", my_vector.contains(99) ? "True" : "False");
    DbgPrintEx(0, 0, "\n");

    /* resize test */
    my_vector.resize(10);
    DbgPrintEx(0, 0, "(~) After resize(10):\n");
    DbgPrintEx(0, 0, "(~) Length  : %d\n", my_vector.len());
    DbgPrintEx(0, 0, "\n");

    /* clear test */
    my_vector.clear();
    DbgPrintEx(0, 0, "(~) After clear:\n");
    DbgPrintEx(0, 0, "(~) IsEmpty : %s\n", my_vector.empty() ? "IsEmpty" : "NotEmpty");
    DbgPrintEx(0, 0, "(~) Length  : %d\n", my_vector.len());

    my_vector.cleanup();

	return status;
}