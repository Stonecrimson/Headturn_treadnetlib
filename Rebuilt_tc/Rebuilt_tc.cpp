// Rebuilt_tc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#define _BIND_TO_CURRENT_MFC_VERSION 1
#define _BIND_TO_CURRENT_CRT_VERSION 1





#include "Client_header.h"
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <sstream>
#include <iostream>


#using <System.dll>

using namespace System;
using namespace System::Text;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Collections;

/* function declarations */
double UpdateVW(const UserState_Data &u, VirtualState_Data &v);
double mycurs_getdouble(void);

/* Variables to use from client_common */
extern char* statstr[64];
extern char* terrstr[4];
extern Client_Controller *client;
extern UserState_Packet * UsrState_pkt;

VirtualState_Data UDPvs_data;
TRVector UDPsurf_normal;
TRTerrain UDPterrain;
TRVector UDPvirtual_force;
float UDPmovement_scale = 0;
float UDPuser_scale = 0;
std::string receivedDataString;

void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

unsigned __stdcall UDPserver(void* pArguments)
{

	UdpClient^ receivingUdpClient = gcnew UdpClient(11115);

	TRVector surf_normal(3, 3, 3);
	TRTerrain terrain = NORMAL_TERRAIN;
	TRVector virtual_force(0, 1, 1);
	float movement_scale = 7;
	float user_scale = 1;
	UDPsurf_normal = surf_normal;
	UDPterrain = terrain;
	UDPvirtual_force = virtual_force;
	UDPmovement_scale = 9;
	UDPuser_scale = user_scale;

	//Creates an IPEndPoint to record the IP Address and port number of the sender.  
	// The IPEndPoint will allow you to read datagrams sent from any source.
	IPEndPoint^ RemoteIpEndPoint = gcnew IPEndPoint(IPAddress::Any, 0);

	try
	{
		while (true)
		{
			// Blocks until a message returns on this socket from a remote host. 

			array<Byte>^receiveBytes = receivingUdpClient->Receive(RemoteIpEndPoint);
			String^ returnData = Encoding::ASCII->GetString(receiveBytes);
			std::stringstream ss;
			std::string x;// = returnData;
			MarshalString(returnData, x);
			ss << x;
			//ss << "2.65444 3.32544444 4.45 5.56 6.67 7.78 8.89 9.9 NORMAL_TERRAIN";
			receivedDataString = ss.str();
			float temp;
			ss >> temp;
			UDPsurf_normal.X(temp);
			ss >> temp;
			UDPsurf_normal.Y(temp);
			ss >> temp;
			UDPsurf_normal.Z(temp);
			ss >> temp;
			UDPvirtual_force.X(temp);
			ss >> temp;
			UDPvirtual_force.Y(temp);
			ss >> temp;
			UDPvirtual_force.Z(temp);
			ss >> temp;
			UDPmovement_scale = temp;
			ss >> temp;
			UDPuser_scale = temp;

			std::string tempStr;
			ss >> tempStr;
			if (tempStr.compare("NORMAL_TERRAIN") == 0)
			{
				UDPterrain = NORMAL_TERRAIN;
			}
			else if (tempStr.compare("STAIRS_TERRAIN") == 0)
			{
				UDPterrain = NORMAL_TERRAIN;
			}
			else if (tempStr.compare("ROUGH_TERRAIN") == 0)
			{
				UDPterrain = NORMAL_TERRAIN;
			}
			else if (tempStr.compare("ICY_TERRAIN") == 0)
			{
				UDPterrain = NORMAL_TERRAIN;
			}
			//std::cout << "temptts" << temp << std::endl;
			//UDPsurf_normal.X(
			//receivedDataString = ss.str();
			//std::cout << receivedDataString <<std::endl;
			//Console::WriteLine( "This is the message you received {0}", ss.str() );

			//Console::WriteLine( "This message was sent from {0} on their port number {1}",
			//RemoteIpEndPoint->Address, RemoteIpEndPoint->Port );

			//data
			// (surf_normal)      (virtualforce)      (ms)    (us)     terrain
			// float float float  float float float   float   float  terrainstring
			//  terrain string can be 
			// NORMAL_TERRAIN, STAIRS_TERRAIN, ROUGH_TERRAIN, ICY_TERRAIN

		}
	}
	catch (Exception^ e)
	{
		Console::WriteLine(e->ToString());
	}
	return 0;
}

#ifndef RTI_VXWORKS
int main(int argc, char *argv[]) {
#else
int vxmain(int arg1, int arg2, int arg3) {
#endif
	char hostname[128];
	//char 
	short port;
	double hz_request = 64;

	// process command line
#if defined (RTI_VXWORKS)
	// vxworks allows only 9 int args, so take first arg to define which machine, and
	//  the second arg as the port number - I know it's messy
	if (arg1 > 3 || arg2 == 0) {
		printf(" \n usage: sp <vxmain address>, server num, port, <hertz>\n\n");
		printf(" Current servers available are \n");
		printf(" 0 - roboworks\n");
		printf(" 1 - roboshell\n");
		printf(" 2 - buzzworm\n");
		printf(" 3 - hovenweep\n");
		exit(1);
	}

	switch (arg1) {
	case 0:
		strcpy(hostname, "roboworks");
		break;
	case 1:
		strcpy(hostname, "roboshell");
		break;
	case 2:
		strcpy(hostname, "buzzworm");
		break;
	case 3:
		strcpy(hostname, "hovenweep");
		break;
	}

	port = arg2;
	if (arg3 != 0) {
		hz_request = (double)pow(2.0, (int)(log(arg3) / log(2) + 0.5));
	}
#else  
	if (argc < 3) {
		fprintf(stderr, " \n usage:  tc server port <hertz>\n\n");
		printf(" \n\n and the best way to run this in unix is: \n");
		printf("  (tc host portnum > /dev/tty) >& /dev/console \n");
		printf("  This way, all stderr messages go to your console window.\n");
		printf("\n  In Windows, simply run from a console:\n");
		printf("  tc host portnum <hertz> \n");
		printf(" Note: hertz should be a power of 2 from 1 - 8192\n");
		exit(1);
	}
	strcpy(hostname, argv[1]);
	port = atoi(argv[2]);
	if (argc > 3) {
		hz_request = atof(argv[3]);
		// convert hertz to power of 2
		hz_request = (double)pow(2.0, (int)(log((float)hz_request) / log(2.0f) + 0.5f));
	}
#endif
	//127.0.0.1 shuold be local computer
	UdpClient^ udpSendClient = gcnew UdpClient("127.0.0.1", 11116);
	rtcSync my_sync((int)hz_request);

	// Create client controller and connect to server
	client = new Client_Controller();
	//  client->SetVerbosity(1);

	switch (client->ConnectToServer(hostname, port)) {
	case SOCKET_ERR:
		// socket problem
		client->PrintLastError();
		exit(1);
		break;
	case NO_SERVER:
		// server does not have a socket open
		client->PrintLastError();
		exit(1);
		break;
	default:
		// connection successful
		fprintf(stderr, "Connection successful to %s:%d\n", hostname, port);
		break;
	}

	// Register all packets (it creates the packets now)
	RegisterPackets();
	printf("Registered all Packets\n");

	// Set some initial values - use data classes
	UserState_Data us_data;
	VirtualState_Data vs_data;

	// User state first, Only can change the position and facing
	//  as all other values are determined by server
	us_data.SetPosEasting(0.0);
	us_data.SetPosNorthing(0.0);

	// Set facing North
	us_data.SetFacingEasting(0.0);
	us_data.SetFacingNorthing(1.0);
	//  TRSetUserElevation(&user_state,1);
	//  TRSetEyeElevation(&user_state,1.8);

	// Virtual state
	TRVector surf_normal(1, 1, 1);
	TRTerrain terrain = NORMAL_TERRAIN;
	TRVector virtual_force(1, 2, 1);
	float movement_scale = 1;
	float user_scale = 1;

	vs_data.SetSurfaceNormal(surf_normal);
	vs_data.SetTerrain(terrain);
	vs_data.SetVirtualForce(virtual_force);
	vs_data.SetMovementScale(movement_scale);
	vs_data.SetUserScale(user_scale);

	// run a udp server in a new thread here
	// the udp server will listen on a port for vs_data

	HANDLE hThread;
	unsigned threadID;

	printf("Creating second thread...\n");

	// Create the second thread.
	int testInt = 0;
	std::string testStr = "t";
	hThread = (HANDLE)_beginthreadex(NULL, 0, &UDPserver, &testStr, 0, &threadID);
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &UDPserver, NULL, 0, &threadID );



	// Treadport must be disabled for New Position to work
	TRDisable();
	sleep(1);

	// Need to check that status is disabled
	TRGetUserState();
	while (UsrState_pkt->GetStatus() & ENABLED_BIT) {
		printf(" awaiting disabled mode on the treadport...\n");
		sleep(2);
		TRGetUserState();
	}

	/* start the user out on the top of the hill */
	TRSetPosition(us_data, vs_data);

	double elevation;
	elevation = UpdateVW(us_data, vs_data);

	// Initialize window
	InitWindow();

	int quit = 0;
	int counter = 0;

	/* ok, lets do something interesting */
	TRVector tmpfacing;
	UserState_Data tmp_usd = us_data;
	VirtualState_Data tmp_vsd = vs_data;


	while (!quit) {

		/* request the treadport position and the status */
		TRGetUserState();

		// check if valid position returned
		if (false) {// JACK EDIT FOR TESTING (!UsrState_pkt->PosValid()) {
					// invalid position returned, which means we lost the server and
					//  when it rebooted it no longer has a valid position, set it
			TRDisable();
			TRSetPosition(us_data, vs_data);
		}
		else {
			// new data received, get copy of it
			UsrState_pkt->GetCopyOfData(&us_data);
		}

		/* check to see if the user wants to
		command start/stop or change the terrain. */
		switch (GetCharacter()) {

		case 'D':
			/* Disable */
			TRDisable();
			break;

		case 'E':
			/*  */
			TREnable();
			break;

		case 'M':
			movement_scale *= 1.1f;
			//      fprintf(stderr," increased to %f \n", movement_scale);
			vs_data.SetMovementScale(movement_scale);
			break;

		case 'm':
			movement_scale *= .9f;
			vs_data.SetMovementScale(movement_scale);
			break;

		case 'U':
			user_scale *= 1.1f;
			vs_data.SetUserScale(user_scale);
			break;

		case 'u':
			user_scale *= .9f;
			vs_data.SetUserScale(user_scale);
			break;

		case 'V':
			// increase virtual force in direction of facing by 10 N
			virtual_force.X() += 10 * us_data.GetFacingEasting();
			virtual_force.Y() += 10 * us_data.GetFacingNorthing();
			virtual_force.Z() += 1;
			vs_data.SetVirtualForce(virtual_force);
			break;

		case 'v':
			// decrease virtual force in direction of facing by 10 N
			virtual_force.X() -= 10 * us_data.GetFacingEasting();
			virtual_force.Y() -= 10 * us_data.GetFacingNorthing();
			virtual_force.Z() -= 1;
			vs_data.SetVirtualForce(virtual_force);
			break;

		case 't':
			/* change the terrain */
			terrain++;
			if (terrain>ICY_TERRAIN)
				terrain = NORMAL_TERRAIN;
			vs_data.SetTerrain(terrain);
			break;

		case 'j':
			/* jump to new location */
			if (us_data.GetStatus() & ENABLED_BIT)
				break;

			double easting;
			double northing;
			double tmp_elev;
			double invnorm;

			printwvis1(" \n INPUT: \n");
			printwvis1("   new location: \n");
			printwvis1("    Easting:  ");
			easting = mycurs_getdouble();
			printwvis1("\n    Northing: ");
			northing = mycurs_getdouble();
			printwvis1("\n    Facing X: ");
			tmpfacing.X(mycurs_getdouble());
			printwvis1("\n    Facing Y: ");
			tmpfacing.Y(mycurs_getdouble());
			tmpfacing.Z(0);

			invnorm = tmpfacing.Norm();

			if (invnorm < EPS) {
				tmpfacing.X(1.0);
				tmpfacing.Y(0.0);
			}
			else {
				tmpfacing /= invnorm;
			}
			tmp_usd.SetPosEasting(easting);
			tmp_usd.SetPosNorthing(northing);
			tmp_usd.SetFacing(tmpfacing);
			tmp_vsd = vs_data;
			tmp_elev = UpdateVW(tmp_usd, tmp_vsd);

			if (TRSetPosition(tmp_usd, tmp_vsd)) {
				/* we were successful, so we should
				update ourselves the same way. */
				us_data = tmp_usd;
				//vs_data = tmp_vsd;
			}
			// otherwise an error occurred, so don't save new user state
			//  or virtual state data

			break;

		case 'q':
			client->Disconnect();
			sleep(1);
			quit = 1;
			break;

		}

		//Update the virtual data with regards to the data
		//received from Unity through the UDP server
		vs_data.SetSurfaceNormal(UDPsurf_normal);
		vs_data.SetTerrain(UDPterrain);

		vs_data.SetVirtualForce(UDPvirtual_force);
		//virtual_force=UDPvirtual_force;
		vs_data.SetMovementScale(UDPmovement_scale);
		vs_data.SetUserScale(UDPuser_scale);


		// Get new elevation, virtual state data
		//elevation = UpdateVW(us_data,vs_data);
		// Send virtual state to server
		TRSetVirtualState(vs_data);

		TRVector facing = us_data.GetFacing();
		TRVector velocity = us_data.GetVelocity();

		/* display this stuff on the text screen*/
		EraseWindow();

#ifdef RTI_VXWORKS
		// only printing at 1 hz since lack curses, or windows
		if (counter % 100 == 0) {
#endif   
			//TRVector UDPsurf_normal;
			//TRTerrain UDPterrain;
			//TRVector UDPvirtual_force;
			//float UDPmovement_scale;
			//float UDPuser_scale;
			//std::cout<<receivedDataString<<std::endl;
			printw1(receivedDataString.c_str());
			printw4(" UDPsurf_normal: %7.2lf x %7.2lf y %7.2lf z \n",
				UDPsurf_normal.X(), UDPsurf_normal.Y(), UDPsurf_normal.Z());
			printw2(" UDPterrain: %s \n", terrstr[UDPterrain]);
			printw3(" UDPms: %7.2lf   UDPus: %7.2lf \n",
				UDPmovement_scale, UDPuser_scale);
			printw4(" UDPvf: %7.2lf x %7.2lf y %7.2lf z \n",
				UDPvirtual_force.X(), UDPvirtual_force.Y(), UDPvirtual_force.Z());


			printw4(" vf: %7.2lf x %7.2lf y %7.2lf z \n",
				vs_data.GetVirtualForce().X(), vs_data.GetVirtualForce().Y(), vs_data.GetVirtualForce().Z());

			printw2(" Counter: %d\n", counter);
			printw3(" Treadport Client connected to %s, port %d        \n\n",
				hostname, port);
			printw1(" -------------------------------------      \n");
			printw1(" Keys: \n");
			printw1("    E:  Enable Treadport      \n");
			printw1("    D:  Disable  Treadport      \n");
			printw1("    t:  change the terrain      \n");
			printw1("    j:  jump user to new position (when stopped)      \n");
			printw1("    q:  quit      \n");
			printw1(" -------------------------------------      \n");
			printw2(" Treadport Status: %s\n", statstr[us_data.GetStatus()]);
			printw1(" -------------------------------------      \n");
			printw3(" Treadport Position:  %7.2lf East %7.2lf North     \n",
				us_data.GetPosEasting(),
				us_data.GetPosNorthing());
			printw4(" User Position:       %7.2lf E  %7.2lf N %7.2lf E        \n",
				us_data.GetPosEasting() + us_data.GetUserEasting(),
				us_data.GetPosNorthing() + us_data.GetUserNorthing(),
				elevation + us_data.GetUserElevation());

			printw4(" Eye Position:       %7.2lf E  %7.2lf N %7.2lf E        \n",
				us_data.GetPosEasting() + us_data.GetEyeEasting(),
				us_data.GetPosNorthing() + us_data.GetEyeNorthing(),
				elevation + us_data.GetEyeElevation());

			printw4(" Treadport Facing:    %7.2lf X  %7.2lf Y  %7.2lf Z        \n",
				facing.X(), facing.Y(), facing.Z());
			printw4(" Treadport Velocity:  %7.2lf X  %7.2lf Y  %7.2lf Z        \n",
				velocity.X(), velocity.Y(), velocity.Z());
			printw1(" -------------------------------------        \n ");
			printw2(" Current Terrain Setting: %s          \n",
				terrstr[terrain]);

			// ONLY VELOCY AND FACING WILL BE SENT.... NOT ENOUGH TIME
			// send facing and velocy to 127.0.0.1 on port 11116
			std::stringstream sb;
			sb << velocity.X() << " " << velocity.Y() << " " << velocity.Z() <<
				" " << facing.X() << " " << facing.Y() << " " << facing.Z();
			const std::string tmp = sb.str();
			char send[1024];
			strcpy(send, tmp.c_str());
			String^ strT = gcnew String(send);
			array<Byte>^ sendBytes = Encoding::ASCII->GetBytes(strT);

			try
			{
				std::cout << "SENT" << std::endl;
				udpSendClient->Send(sendBytes, sendBytes->Length);
			}
			catch (Exception^ e)
			{
				Console::WriteLine(e->ToString());
			}


#ifdef RTI_VXWORKS
		}
#endif

		/* sleep a small while */

		//#if   defined(sparc_solaris)
		//      struct timespec r;
		//      r.tv_sec = 0;
		//      r.tv_nsec = 2000000000; /* ??? 30 hz? */
		//      usleep(r.tv_nsec/1000);
		//#else

		//my_sync.block();
		//#endif

		counter++;
	}/* end while */

	StopWindow();
	// Destroy the thread object.
	CloseHandle(hThread);
	return 1;
}

/******************************* Virtual World Info ***********************************/

#define MAXX +10.0
#define MINX -10.0
#define MAXY +10.0
#define MINY -10.0

/* this defines sqrt(2)/2 */
#define R2O2 0.70710678

double UpdateVW(const UserState_Data &u, VirtualState_Data &v) {

	/* here we assume the world is a gausian
	hill defined on x = [MINX,MAXX],
	y = [MINY,MAXY], with z = 5*exp(-(x^2+y^2)/100).

	The square world is surrounded by walls that
	produce collisions.

	There is no virtual force yet. */

	double x, y, z;
	TRVector normal, collisionn, collisionp;

	x = u.GetPosEasting();
	y = u.GetPosNorthing();

	double elevation;
	elevation = z = 5.0*exp(-(x*x + y*y) / 100);

	normal.X() = 2 * z*x / 100;
	normal.Y() = 2 * z*y / 100;
	normal.Z() = 1;
	normal /= normal.Norm();

	//v.SetSurfaceNormal(normal);

	/* worry about collisions with the walls
	This is really ugly code, but it tries
	to do the tom-style basic blending of normals
	at the contact between two surfaces. */


	/*  if (x>MAXX) {
	if (y>MAXY) {
	collisionn.X = -R2O2;
	collisionn.Y = -R2O2;
	collisionn.Z = 0;
	collisionp.X = MAXX;
	collisionp.Y = MAXY;
	}
	else if ( y<MINY) {
	collisionn.X = -R2O2;
	collisionn.Y = R2O2;
	collisionn.Z = 0;
	collisionp.X = MAXX;
	collisionp.Y = MINY;
	}
	else {
	collisionn.X = -1;
	collisionn.Y = 0;
	collisionn.Z = 0;
	collisionp.X = MAXX;
	collisionp.Y = y;

	}
	//    TRSetCollision(v,collisionn,collisionp);
	}
	else if (x<MINX) {
	if (y>MAXY) {
	collisionn.X = +R2O2;
	collisionn.Y = -R2O2;
	collisionn.Z = 0;
	collisionp.X = MINX;
	collisionp.Y = MAXY;
	}
	else if ( y<MINY) {
	collisionn.X = +R2O2;
	collisionn.Y = +R2O2;
	collisionn.Z = 0;
	collisionp.X = MINX;
	collisionp.Y = MINY;
	}
	else {
	collisionn.X = 1;
	collisionn.Y = 0;
	collisionn.Z = 0;
	collisionp.X = MINX;
	collisionp.Y = y;
	}
	//    TRSetCollision(v,collisionn,collisionp);
	}
	else {
	if (y>MAXY) {
	collisionn.X = 0;
	collisionn.Y = -1;
	collisionn.Z = 0;
	collisionp.X = x;
	collisionp.Y = MAXY;
	//      TRSetCollision(v,collisionn,collisionp);
	}
	else if (y<MINY) {
	collisionn.X = 0;
	collisionn.Y = 1;
	collisionn.Z = 0;
	collisionp.X = x;
	collisionp.Y = MINY;
	//      TRSetCollision(v,collisionn,collisionp);
	}
	else {
	//      TRClearCollision(v);
	}
	}
	*/
	return elevation;
}

