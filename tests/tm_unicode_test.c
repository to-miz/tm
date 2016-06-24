#define TM_UNICODE_IMPLEMENTATION
#include "../tm_unicode.h"

#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define DATA_PATH "../tests/data/"

static size_t readWholeFile( const char* filename, void* out, size_t size )
{
	FILE* file = fopen( filename, "rb" );
	if( file ) {
		fseek( file, 0, SEEK_END );
		size_t fsize = ftell( file );
		fseek( file, 0, SEEK_SET );

		size_t len = ( fsize < size ) ? ( fsize ) : ( size );
		fread( out, len, 1, file );
		fclose( file );
		return len;
	}
	return 0;
}
static void writeBufferToFile( const char* filename, void* src, size_t size )
{
	FILE* file = fopen( filename, "wb" );
	if( file ) {
		fwrite( src, size, 1, file );
		fclose( file );
	}
}
// helper
static void print_char8( char** dest, size_t* remaining, const char* src, size_t size )
{
	size_t sz = ( *remaining < size ) ? ( *remaining ) : ( size );
	memcpy( *dest, src, sz );
	*dest += sz;
	*remaining -= sz;
}
static void print_char16( uint16_t** dest, size_t* remaining, const void* src, size_t size )
{
	size_t sz = ( *remaining < size ) ? ( *remaining ) : ( size );
	memcpy( *dest, src, sz * sizeof( uint16_t ) );
	*dest += sz;
	*remaining -= sz;
}

static void unicodeTest1()
{
	size_t readBufferSize = 100000;
	size_t writeBufferSize = 100000;

	char* readBuffer = (char*)malloc( readBufferSize );
	char* writeBuffer = (char*)malloc( writeBufferSize );

	size_t len = readWholeFile( DATA_PATH "UTF-8-demo.txt", readBuffer, readBufferSize );
	if( len ) {
		// convert utf8 to utf32 to utf8 and write to file

		// skip bom
		if( utf8HasByteOrderMark( readBuffer, len ) ) {
			readBuffer += utf8ByteOrderMarkSizeInBytes;
			len -= utf8ByteOrderMarkSizeInBytes;
		}

		const char* str = readBuffer;
		size_t remaining = len;
		char* out = writeBuffer;
		size_t outRemaining = writeBufferSize;

		print_char8( &out, &outRemaining, utf8ByteOrderMark, utf8ByteOrderMarkSizeInBytes );
		while( remaining ) {
			uint32_t cp = utf8NextCodepoint( &str, &remaining );
			Utf8Sequence sequence = toUtf8( cp );
			print_char8( &out, &outRemaining, sequence.elements, sequence.length );
		}
		size_t outLen = writeBufferSize - outRemaining;
		writeBufferToFile( "utf8_to_utf8_test.txt", writeBuffer, outLen );

		if( outLen - utf8ByteOrderMarkSizeInBytes != len ) {
			printf( "Error: Written utf8 file has differing size\n" );
		}
	} else {
		printf( "Error: Failed to load file UTF-8-demo.txt\n" );
	}

	free( readBuffer );
	free( writeBuffer );
}

static void unicodeTest2()
{
	size_t readBufferSize = 100000;
	size_t writeBufferCount = 100000;
	size_t writeBufferSize = writeBufferCount * sizeof( uint16_t );

	char* readBuffer = (char*)malloc( readBufferSize );
	uint16_t* writeBuffer = (uint16_t*)malloc( writeBufferSize );

	size_t len = readWholeFile( DATA_PATH "UTF-8-demo.txt", readBuffer, readBufferSize );
	if( len ) {
		// convert utf8 to utf16 and write to file

		// skip bom
		if( utf8HasByteOrderMark( readBuffer, len ) ) {
			readBuffer += utf8ByteOrderMarkSizeInBytes;
			len -= utf8ByteOrderMarkSizeInBytes;
		}

		const char* str = readBuffer;
		size_t remaining = len;
		uint16_t* out = writeBuffer;
		size_t outRemaining = writeBufferCount;

		print_char16( &out, &outRemaining, utf16LittleEndianByteOrderMark, utf16ByteOrderMarkSize );

		outRemaining -= convertUtf8ToUtf16( readBuffer, len, out, outRemaining );
		size_t outLen = writeBufferCount - outRemaining;
		writeBufferToFile( "utf8_to_utf16_test.txt", writeBuffer, outLen * sizeof( uint16_t ) );
	} else {
		printf( "Error: Failed to load file UTF-8-demo.txt\n" );
	}

	free( readBuffer );
	free( writeBuffer );
}

static void unicodeTest3()
{
	size_t readBufferCount = 100000;
	size_t readBufferSize = readBufferCount * sizeof( uint16_t );
	size_t writeBufferSize = 100000;

	uint16_t* readBuffer = (uint16_t*)malloc( readBufferSize );
	char* writeBuffer = (char*)malloc( writeBufferSize );

	size_t len = readWholeFile( "utf8_to_utf16_test.txt", readBuffer, readBufferSize );
	// convert size in bytes to uint16_t count
	len /= sizeof( uint16_t );
	if( len ) {
		// convert utf16 to utf8 and write to file

		// skip bom
		if( utf16HasLittleEndianByteOrderMark16( readBuffer, len ) ) {
			readBuffer += utf16ByteOrderMarkSize;
			len -= utf16ByteOrderMarkSize;
		}

		const uint16_t* str = readBuffer;
		size_t remaining = len;
		char* out = writeBuffer;
		size_t outRemaining = writeBufferSize;

		print_char8( &out, &outRemaining, utf8ByteOrderMark, utf8ByteOrderMarkSizeInBytes );

		outRemaining -= convertUtf16ToUtf8( readBuffer, len, out, outRemaining );
		size_t outLen = writeBufferSize - outRemaining;
		writeBufferToFile( "utf16_to_utf8_test.txt", writeBuffer, outLen );
	} else {
		printf( "Error: Failed to load file utf8_to_utf16_test.txt\n" );
	}

	free( readBuffer );
	free( writeBuffer );
}

int main( int argc, char const* argv[] )
{
	unicodeTest1();
	unicodeTest2();
	unicodeTest3();
	return 0;
}
