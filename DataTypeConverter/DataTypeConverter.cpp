// DataTypeConverter.cpp : Defines the entry point for the console application.
// Author: Hao Cheng Cheam
// Last Revised: 25 Oct 2017
// This program converts a 64 binary input in hex into a 64 bit double precision floating point, with 4 different byte and word orders (Modbus)
// NOT OPTIMISED. USE AT YOUR OWN RISK.
/////////////////
/////////////////
/////////////////
/////////////////

#include "stdafx.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void swapBits64(string, int, string*);

// converts hex character to a binary character
const char* hex_char_to_bin(char c)
{
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	default: cout << "Error in hex_char_to_bin function." << endl;
	}
}

// convert hex string to binary string
// example input: "FF00FF00FF00FF00"
// example output: "1111111100000000111111110000000011111111000000001111111100000000"
string hex_str_to_bin_str(string hexString)
{
	string binStr;
	for (int i = 0; i != hexString.length(); ++i)
	{
		binStr += hex_char_to_bin(hexString[i]);
    }
	return binStr;
}

// converts a string to 64 bit double fp
double bitstring_to_double(const string& s)
{
	unsigned long long x = 0;
	for (string::const_iterator it = s.begin(); it != s.end(); ++it)
	{
		x = (x << 1) + (*it - '0');
	}
	double d;
	memcpy(&d, &x, 8);
	return d;
}

// converts a string to 64 bit long long int
long long int bitstring_to_longlong(string s)
{
    long long int x = 0;
	for (string::const_iterator it = s.begin(); it != s.end(); ++it)
	{
		x = (x << 1) + (*it - '0');
	}
	return x;
}

// convert 64 bit long long int to string
string longlong_to_bitstring(long long int d)
{
	string outputStr = "";
	for (int i = 63;i >=0; --i)
	{
		unsigned long long x = ((unsigned long long) d >> i) % 2;
		char c = x + '0';
		outputStr+=c;
	}
	return outputStr;
}


int main()
{
	int error = 0; // error flag
	string hexString = ""; // input string in hex
	string binaryString = ""; // input string in binary
	string byteOrderString = "";
	string finalString = "";
	double finalFloat = 0;
	unsigned int bits = 0; // number of bits in input string
	// cout << "Please type in length of the binary input (16,32,64): " << endl;
	
	// header
	cout << "***********************************************************************" << endl;
	cout << "***********************************************************************" << endl;
	cout << "64 bit hex to float data converter." << endl;
	cout << "For Modbus word/byte orderings." << endl;
	cout << "For bug fixes and suggestions, please email me at hao.cheam@gmail.com" << endl;
	cout << "***********************************************************************" << endl;
	cout << "***********************************************************************\n" << endl;

	while (1)
	{
		cout << "Please type in your 64 bit in hex: (eg: ff00ff00ff00ff00)" << endl;
		cin >> hexString;

		// error check
		if (hexString.length() != 16)
		{
			cout << "Your input is invalid. Please input a 64 bit hex number.\n" << endl;
			cout << "//////////////////////////////////////" << endl; // delimiter
			cout << "//////////////////////////////////////\n" << endl; // delimiter
		}
		else
		{
			binaryString = hex_str_to_bin_str(hexString);
			cout << "\nYour input is: " << binaryString << endl;
			bits = binaryString.length();
			cout << "Number of bits in your input = " << bits << endl;

			// specify word and byte ordering
			// HWHB = high word high byte first, LWLB = low word low byte first
			cout << "\nPlease choose word and byte ordering (HWHB,LWHB,HWLB,LWLB): " << endl;
			cin >> byteOrderString;
			if (byteOrderString == "HWHB") // high word high byte first, or big endian
			{
				swapBits64(binaryString, 1, &finalString);
			}
			else if (byteOrderString == "LWHB") // low word high byte first
			{
				swapBits64(binaryString, 2, &finalString);
			}
			else if (byteOrderString == "HWLB") // high word low byte first
			{
				swapBits64(binaryString, 3, &finalString);
			}
			else if (byteOrderString == "LWLB") // low word low byte first
			{
				swapBits64(binaryString, 4, &finalString);
			}
			else // incorrect input
			{
				cout << "Invalid input!\n" << endl;
				error = 1; // set error flag
				cout << "//////////////////////////////////////" << endl; // delimiter
				cout << "//////////////////////////////////////\n" << endl; // delimiter
			}
			if (error == 0)
			{
				// print out true binary ordering
				cout << "\nThe true bit ordering is: " << finalString << endl;

				// convert from binary to 64 bit double precision floating point
				finalFloat = bitstring_to_double(finalString);
				cout << "The 64 bit floating point value is: " << finalFloat << "\n\n\n" << endl;
				cout << "//////////////////////////////////////" << endl; // delimiter
				cout << "//////////////////////////////////////\n" << endl; // delimiter
		    }
			error = 0; // reset error flag
		}
	}
    return 0;
}

// for 64 bit inputs only
// swap the bytes and words in binary string according to the ordering specified
// ordering: 1 for HWHB, 2 for LWHB, 3 for HWLB, 4 for LWLB
void swapBits64(string binaryString, int ordering, string* outputStr)
{
	long long int HB1; // high byte 1
	long long int HB2, HB3, HB4, HB5, HB6, HB7, HB8; 
	long long int inputbinary = bitstring_to_longlong(binaryString);

	long long outputbinary;

	// extract bytes
	HB1 = (inputbinary & 0xFF00000000000000) >> 56;
	HB2 = (inputbinary & 0x00FF000000000000) >> 48;
	HB3 = (inputbinary & 0x0000FF0000000000) >> 40;
	HB4 = (inputbinary & 0x000000FF00000000) >> 32;
	HB5 = (inputbinary & 0x00000000FF000000) >> 24;
	HB6 = (inputbinary & 0x0000000000FF0000) >> 16;
	HB7 = (inputbinary & 0x000000000000FF00) >> 8;
	HB8 = (inputbinary & 0x00000000000000FF);

	switch (ordering)
	{
	case 1: // HWHB ordering, HB1,2,3,4,5,6,7,8
		*outputStr = binaryString;
		break;
	case 2: // LWHB ordering, HB7,8,5,6,3,4,1,2
		outputbinary = (HB7 << 56) | (HB8 << 48) | (HB5 << 40) | (HB6 << 32) | (HB3 << 24) | (HB4 << 16) | (HB1 << 8) | (HB2); 
		*outputStr = longlong_to_bitstring(outputbinary);
		break;
	case 3: // HWLB ordering, HB2,1,4,3,6,5,8,7
		outputbinary = (HB2 << 56) | (HB1 << 48) | (HB4 << 40) | (HB3 << 32) | (HB6 << 24) | (HB5 << 16) | (HB8 << 8) | (HB7);
		*outputStr = longlong_to_bitstring(outputbinary);
		break;
	case 4: // LWLB ordering, HB8,7,6,5,4,3,2,1
		outputbinary = (HB8 << 56) | (HB7 << 48) | (HB6 << 40) | (HB5 << 32) | (HB4 << 24) | (HB3 << 16) | (HB2 << 8) | (HB1);
		*outputStr = longlong_to_bitstring(outputbinary);
		break;
	default:
		cout << "Invalid byte ordering!" << endl;
		break;
	}

}

