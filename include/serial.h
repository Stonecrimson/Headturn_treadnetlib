/* This header file contains a serial port class
 * this class contains nessesary information to 
 * make a serial connection as well as send and
 * receive information over said connection.
 *
 * These files and methods are designed for sending
 * data over a serial connection
 *
 * (C) Kyle Crandall 2009
 */

#ifndef serialH
#define serialH

#include <Windows.h>
#include <string>
#include <stdlib.h>

using namespace std;

#define BUFSIZE 1000

class SerialPort				// this class contains information and functions
{
private:
	string name;				// Name of the port ie. "COM1" or "COM2"
	HANDLE hcom;				// File descriptor for com port
	bool isConnected;			// com port is connected
	_DCB serialInterface;		// Data for serial port
public:
	SerialPort();
	SerialPort(string);				// This constructor will initiate the name of the port ("COM1" COM2" etc.) can also be an existing file
	~SerialPort();					// Destructor disconnects from the port
	bool connect();					// Connects to serial port, must be called before any data is sent
	bool sendStr(string);			// Sends a string
	bool sendStr(unsigned char*, int);		// Sends a cString
	string recieveStr();			// receives string from port
	int recieveCharArray(int s, unsigned char* c);
	void disconnect();				// disconnects from port
	bool IsConnected();				// returns true if connected
};
#endif
