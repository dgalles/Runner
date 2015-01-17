#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <winsock2.h>

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "wsock32.lib")

#include "Logger.h"
#define DEFAULT_PORT "8080"
#define DEFAULT_HOST "guinness.cs.usfca.edu"

#define PORT 8080

SOCKET tcpConnect() 
{
	int error;
	SOCKET handle;
	struct hostent *host;
	struct sockaddr_in server;

	    std::string foo(DEFAULT_HOST);

	host = gethostbyname(DEFAULT_HOST);
	handle = socket(AF_INET, SOCK_STREAM, 0);
	struct timeval timeout;      
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	if (setsockopt (handle, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
		sizeof(timeout)) < 0) { perror("setsockopt failed\n"); }
	if (handle == INVALID_SOCKET)
	{
		perror("Socket");
		handle = 0;
	} 
	else 
	{
		server.sin_family = AF_INET;
		server.sin_port = htons(PORT);
		server.sin_addr = *((struct in_addr *) host->h_addr);
		memset(&(server.sin_zero), 0, 8);

		error = connect(handle, (struct sockaddr *) &server,
			sizeof(struct sockaddr));

		if (error == -1)
		{
			perror("Connect");
			handle = 0;
		}
	}
	return handle;
}

connection * sslConnect(void) 
{
	connection *c;

	c = (connection *) malloc(sizeof(connection));
	c->sslHandle = NULL;
	c->sslContext = NULL;

	c->socket = tcpConnect();
	if (c->socket) {
		SSL_load_error_strings();
		SSL_library_init();

		c->sslContext = SSL_CTX_new(TLSv1_method());
		if (c->sslContext == NULL) {
			ERR_print_errors_fp (stderr);
		}


		c->sslHandle = SSL_new(c->sslContext);
		if (c->sslHandle == NULL) {
			ERR_print_errors_fp(stderr);
		}
		if (!SSL_set_fd(c->sslHandle, c->socket)) {
			ERR_print_errors_fp (stderr);
		}
		int err= SSL_connect (c->sslHandle);
		if (err != 1) {
			int err2  = SSL_get_error(c->sslHandle,err);
			ERR_print_errors_fp (stdout);
		}
	} else {
		perror("Connect failed");
	}
	return c;
}

void sslDisconnect (connection * c)
{
	if (c->socket) {
		closesocket(c->socket);
	}
	if (c->sslHandle) {
		SSL_shutdown (c->sslHandle);
		SSL_free (c->sslHandle);
	}
	if (c->sslContext) {
		SSL_CTX_free (c->sslContext);
	}
	free(c);
}

int sslRead (connection * c, char *buffer, int minReadSize, int maxReadSize)
{
	int totalReceived = 0;
	int iter = 0;

	memset(buffer, 0, maxReadSize * sizeof(char));
	bool read = false;
	while (totalReceived < minReadSize && iter < 100)
	{
		int rev =  SSL_read(c->sslHandle, buffer, maxReadSize);
		if (rev > 0)
		{
			totalReceived += rev;
			maxReadSize -= rev;
			buffer += rev;
		}
		iter++;
	}

    return totalReceived;
}




int sslWrite(connection * c, const char * text)
{
	if (c == NULL)
	{
		return -1;
	}

	int totalWritten = 0;
	if (c) 
	{
		int lengthToWrite = strlen(text);
		while (lengthToWrite > 0)
		{
			int written_bytes = SSL_write (c->sslHandle, text, lengthToWrite);
			if (written_bytes > 0)
			{
				lengthToWrite -= written_bytes;
				totalWritten += written_bytes;
			}
			else
			{
				return written_bytes;  // Returning error code from SSL_write
			}
		}
	}
	return totalWritten;
}


int sendDataAndGetAwk(connection *c, const char *text)
{
	int bytesSent = sslWrite(c, text);
	if (bytesSent <= 0)
	{
		return bytesSent;
	}
	char buffer[10];

	int bytes_read = sslRead(c, buffer, 2, 10);
	if (bytes_read <= 0 || strcmp(buffer, "OK") != 0)
	{
		return -1;
	}
	return bytesSent;
}

Ogre::String MakeOKForFilename(Ogre::String str)
{
	str = Ogre::StringUtil::replaceAll(str,":","_");
	str = Ogre::StringUtil::replaceAll(str,"|", "_");
	return str;


}


connection * init(std::string username, std::string password)
{
	connection * c;
	char * cert;
	char buffer[10];

	c = sslConnect();

	if (!c || !c->socket)
	{
		goto error;
	}
	int bytes_read = sslRead(c, buffer, 2, 10);
	printf("response is: %s\n", buffer);
	cert = (char *)malloc(sizeof(char) * 4097);
	if (strcmp(buffer, "OK") != 0) {
		printf("Initial Connnection failed\n");
		goto error;
	}

	sslWrite(c, (username + "\n").c_str());

	bytes_read = sslRead(c, buffer, 2, 10);
	if (strcmp(buffer, "OK") != 0) {
		printf("Bad Username!");
		goto error;
	}

	sslWrite(c, (password + "\n").c_str());

	bytes_read = sslRead(c, buffer,2, 10);
	if (strcmp(buffer, "OK") != 0) {
		printf("Bad Password!");
		goto error;
	}
	return c;
error:
	sslDisconnect(c);
    return NULL;
}

/* A ServerCom struct contains all the necessary pieces for a socket connection */
//struct ServerCom
//{
//	WSADATA wsaData;
//	SOCKET ConnectSocket;// = INVALID_SOCKET;
//	int iResult;
//	struct addrinfo *result;// = NULL,
//	struct addrinfo *ptr;// = NULL,
//	struct addrinfo hints;
//
//	ServerCom()
//	{
//		ConnectSocket = INVALID_SOCKET;
//		result = NULL;
//		ptr = NULL;
//	}
//};


void Logger::changeUsername(Ogre::String username)
{
	if (mCurrentUsername != username)
	{
		mCurrentUsername = username;
		Logout();
	}

}


bool Logger::Login()
{
	connection *secureConnection = init(mCurrentUsername, mCurrentPassword);

	if (secureConnection == NULL) {
        printf("could not connect\n");
		return false;
	}
	sslWrite(secureConnection, "LoginCheck\n");
	mLoggedIn = true;
	sslDisconnect(secureConnection);
	return true;

}
void Logger::Logout()
{
	//mFailureOverlay->show();
	//mSuccessOverlay->hide();
	mLoggedIn = false;
	if (mSecureConnection)
	{
		sslDisconnect(mSecureConnection);
		mSecureConnection = NULL;
	}
}

Logger::Logger() : mPlyrData(), mSkelLock(), mPlyrLock(), mDbLock()
{
	mSeconds = 0;
	mMinutes = 0;
	mSeconds2 = 0;
	mMinutes2 = 0;
	//mSock = new ServerCom();
	//mScoreSock = new ServerCom();
	//mLRSock = new ServerCom();
	//mLRTrueSock = new ServerCom();
	//mFBSock = new ServerCom();
	//mCoinsLeftSock = new ServerCom();
	//mCoinsLeftMissedSock = new ServerCom();
	//mCoinsMiddleMissedSock = new ServerCom();
	//mCoinsMiddleSock = new ServerCom();
	//mCoinsRightSock = new ServerCom();
	//mCoinsRightMissedSock = new ServerCom();
	//mSpeedSock = new ServerCom();
	mHost = DEFAULT_HOST;
	mPort = DEFAULT_PORT;
	mSessionStarted = false;
	mSessionEnded = false;
	mSecureConnection = NULL;
}

Logger::~Logger()
{
}



void Logger::sendProfileData(std::string data)
{
	char buffer[10000];
	connection *secureConnection = init(mCurrentUsername, mCurrentPassword);
	if (!secureConnection)
	{
		return;
	}
	sprintf_s(buffer, 10000, "SetProfile;runner;%s\n",data.c_str());
	sslWrite(secureConnection, buffer);
	 
}
std::string Logger::getProfileData()
{
	char buffer[10000];
	connection *secureConnection = init(mCurrentUsername, mCurrentPassword);
	if (!secureConnection)
	{
		return std::string("{}");
	}
	sslWrite(secureConnection,"GetProfile;runner\n");
	int bytes_read = sslRead(secureConnection, buffer, 2, 10000);
	return std::string(buffer);
}

void
	Logger::StartSession()
{
	/* Timestamp based on example from stack overflow on constructing stamp in
	ISO 8601 format. Initial timestamp used for file names to differentiate 
	between sessions. */

	if (!mLoggedIn)
	{
		// Throw here?
		printf("DB_Send failed.\n");

		printf("Not logged in!\n");
		return;
	}



	time(&mTimeStampBeg);

	strftime(mBegBuf, sizeof mBegBuf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&mTimeStampBeg));
	mSessionStarted = true;
	mSessionEnded = false;

	mSecureConnection = init(mCurrentUsername, mCurrentPassword);

	if (mSecureConnection == NULL)
	{
		mSessionStarted = false;
		return;

	}

	memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
	sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "StartSession;%s\n",mBegBuf);
	sslWrite(mSecureConnection, mPlyrBuf);


	memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
	//sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "db.patients.insert({\"patient_id\":%s, \"timestamp\":\"%s\"})\n", 
	//	mCurrentUsername.c_str(), mBegBuf);


	sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "insert;{\"game\":\"runner\", \"patient_id\":%s, \"timestamp\":\"%s\"})", 
		mCurrentUsername.c_str(), mBegBuf);

	int sent = sslWrite(mSecureConnection, mPlyrBuf);

//	int sent = sendDataAndGetAwk(mSecureConnection, mPlyrBuf);

	if (sent <= 0)
	{
		WriteToLog("Failed to insert patient_it / timestamp.\n", DEFAULT_BUFSIZE);
	}


	memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
	sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "In StartSession, starting daemon.\n");
	WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

	mConnected = true;

	mDaemon = std::thread(&Logger::daemonFunc, this);
}

std::string Logger::changePassword(Ogre::String password)
{
	mCurrentPassword = password;

	if (Login())
	{
		return getProfileData();
	}
	else
	{
		return "";
	}



}



void
	Logger::EndSession()
{
	if (mSessionStarted && !mSessionEnded)
	{
		mSessionEnded = true;
		time(&mTimeStampEnd);
		mDaemon.join();
		memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
		sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "In EndSession, daemon has joined.\n");
		WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

	}
	if (mSecureConnection)
	{
		sslDisconnect(mSecureConnection);
		mSecureConnection = NULL;
	}
	mSessionEnded = true;
	mConnected = false;
}

void
	Logger::daemonFunc()
{
	PlyrData *pdata;
	SkelData *sdata;
	bool ptest = false;
	bool stest = false;

	// loop until session ends and all data queues are empty
	while(true)
	{
		// send player data
		mPlyrLock.lock();
		if(!mPlyrData.empty())
		{
			pdata = mPlyrData.front();
			mPlyrData.pop();
			mPlyrLock.unlock();

			daemonSendPlyrData(pdata);
		}
		else
		{
			mPlyrLock.unlock();
			ptest = true;
		}

		// send skeleton data
		mSkelLock.lock();
		if(!mSkelData.empty())
		{
			sdata = mSkelData.front();
			mSkelData.pop();
			mSkelLock.unlock();

			daemonSendSkelData(sdata);
		}
		else
		{
			mSkelLock.unlock();
			stest = true;
		}
		// if the session is ended and both queues are empty, break
		if(mSessionEnded && ptest && stest) 
			break;
	}


}




int Logger::LogAndSend(const char *attribName, float value)
{
	if (!mSecureConnection)
	{
		return -1;
	}
	int sentBytes;
	memset(mPlyrBuf,0,DEFAULT_BUFSIZE);
	//sprintf_s(mPlyrBuf, 
	//	DEFAULT_BUFSIZE,
	//	"db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"%s.%d.%d\":%f}})\n",
	//	mCurrentUsername.c_str(), mBegBuf, attribName, mMinutes, mSeconds % 60, value);


	sprintf_s(mPlyrBuf, 
		DEFAULT_BUFSIZE,
		"set;{\"game\":\"runner\", \"patient_id\":\"%s\", \"timestamp\": \"%s\"};{\"%s.%d.%d.0\":%f}\n",
		mCurrentUsername.c_str(), mBegBuf, attribName, mMinutes, mSeconds % 60, value);

	WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

	 sentBytes = sslWrite(mSecureConnection, mPlyrBuf);
//	sentBytes = sendDataAndGetAwk(mSecureConnection, mPlyrBuf);

	if (sentBytes <= 0)
	{
		WriteToLog("sslWrite failed.\n", 18);
	}

	return sentBytes;
}


int Logger::LogAndSend(const char *attribName, int value)
{
	if (!mSecureConnection)
	{
		return -1;
	}
	int sentBytes;
	memset(mPlyrBuf,0,DEFAULT_BUFSIZE);
	sprintf_s(mPlyrBuf, 
		DEFAULT_BUFSIZE,
		"set;{\"game\":\"runner\", \"patient_id\":\"%s\", \"timestamp\": \"%s\"};{\"%s.%d.%d.0\":%f}\n",
		mCurrentUsername.c_str(), mBegBuf, attribName, mMinutes, mSeconds % 60, value);


	WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

	sentBytes = sslWrite(mSecureConnection, mPlyrBuf);
//	sentBytes = sendDataAndGetAwk(mSecureConnection, mPlyrBuf);

	if (sentBytes <= 0)
	{
		WriteToLog("sslWrite failed.\n", 18);
	}

	return sentBytes;
}

void
	Logger::daemonSendPlyrData(PlyrData *pdata)
{
	mSeconds++;
	mMinutes = mSeconds / 60;
	int sentBytes = 0;

	LogAndSend("CoinsL", pdata->leftCoinsCollected);
	LogAndSend("CoinsR", pdata->rightCoinsCollected);
	LogAndSend("CoinsM", pdata->middleCoinsCollected);
	LogAndSend("CoinsMissL", pdata->leftCoinsMissed);
	LogAndSend("CoinsMissR", pdata->rightCoinsMissed);
	LogAndSend("CoinsMissM", pdata->middleCoinsCollected);
}

void
	Logger::daemonSendSkelData(SkelData *sdata)
{
	mSeconds2++;
	mMinutes2 = mSeconds2 / 60;

	LogAndSend("skel_lr", sdata->lrAngle);
	LogAndSend("skel_lr_true", sdata->lrAngleTrue);
	LogAndSend("skel_fb", sdata->fbAngle);

	//memset(mKinBuf1, 0, DEFAULT_BUFSIZE);
	//sprintf_s(mKinBuf1, 
	//	DEFAULT_BUFSIZE, 
	//	"db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"skel_lr.%d.%d\":%f}})", 
	//	mCurrentID.c_str(), 
	//	mBegBuf, 
	//	mMinutes2, 
	//	mSeconds2 % 60, 
	//	sdata->lrAngle);
	//WriteToLog(mKinBuf1, DEFAULT_BUFSIZE);
	//if(DB_Send(mKinBuf1, strlen(mKinBuf1), mLRSock) == 1)
	//	WriteToLog("DB_Send failed.\n", 18);


}

/*------------------------------------------------------------------------------
* Receives player data (score only for now from a Player the logger is 
* registered to listen to
*/
void
	Logger::ReceivePlyrData(PlyrData *data)
{
	if(mSessionEnded || !mSessionStarted)
		return;

	PlyrData *localcpy = new PlyrData(data);
	mPlyrLock.lock();
	mPlyrData.push(localcpy);
	mPlyrLock.unlock();
}
/*------------------------------------------------------------------------------
* Receives skeleton data from a Kinect sensor the logger is registered to 
* listen to
*/
void
	Logger::ReceiveSkelData(SkelData *data) 
{
	if(mSessionEnded || !mSessionStarted)
		return;

	SkelData *localcpy = data;
	mSkelLock.lock();
	mSkelData.push(localcpy);
	mSkelLock.unlock();

}
/*------------------------------------------------------------------------------
* Writes all relevant data to output file.
* File name convention: 
<playerId>_<timestamp_session_beg>_<timestamp_session_end>
<data_type>
<per second entries (1 minute per line max)>
...
<data_type>
<per second entries (1 minute per line max)>
...
...
* 
*/
void
	Logger::WriteToLog(char *buf, size_t buflen)
{
	std::ofstream outfile;
	outfile.open("db_log.txt", std::ofstream::out | std::ofstream::app);
	outfile << buf << "\n";
	outfile.close();
}



/*
* Database connection done with WinSock 2.2. We use code detailed on Microsoft's Dev Center:
* http://msdn.microsoft.com/en-us/library/windows/desktop/ms737591(v=vs.85).aspx
* Our connection data is stored in a separate struct referenced below.
*
*/
int
	Logger::DB_Connect(char *hostname, char *port, ServerCom *com)
{


	//// Initialize Winsock
	//com->iResult = WSAStartup(MAKEWORD(2,2), &com->wsaData);
	//if (com->iResult != 0) {
	//	printf("WSAStartup failed with error: %d\n", com->iResult);
	//	return 1;
	//}

	//ZeroMemory( &com->hints, sizeof(com->hints) );
	//com->hints.ai_family = AF_UNSPEC;
	//com->hints.ai_socktype = SOCK_STREAM;
	//com->hints.ai_protocol = IPPROTO_TCP;

	//// Resolve the server address and port
	//com->iResult = getaddrinfo(hostname, port, &com->hints, &com->result);

	//if ( com->iResult != 0 ) {
	//	printf("getaddrinfo failed with error: %d\n", com->iResult);
	//	WSACleanup();
	//	return 2;
	//}

	//// Attempt to connect to an address until one succeeds
	//for(com->ptr=com->result; com->ptr != NULL ;com->ptr=com->ptr->ai_next) {

	//	// Create a SOCKET for connecting to server
	//	com->ConnectSocket = socket(com->ptr->ai_family, com->ptr->ai_socktype, 
	//		com->ptr->ai_protocol);
	//	if (com->ConnectSocket == INVALID_SOCKET) 
	//	{
	//		printf("socket failed with error: %ld\n", WSAGetLastError());
	//		WSACleanup();
	//		return 3;
	//	}

	//	// Connect to server.
	//	com->iResult = connect( com->ConnectSocket, com->ptr->ai_addr, (int)com->ptr->ai_addrlen);
	//	if (com->iResult == SOCKET_ERROR) {
	//		closesocket(com->ConnectSocket);
	//		com->ConnectSocket = INVALID_SOCKET;
	//		continue;
	//	}
	//	break;
	//}

	//freeaddrinfo(com->result);

	//if (com->ConnectSocket == INVALID_SOCKET) {
	//	printf("Unable to connect to server!\n");
	//	WSACleanup();
	//	return 4;
	//}
	//mConnected = true;
	return 0;
}

int
	Logger::DB_Disconnect(ServerCom *com)
{
	// shutdown the connection since no more data will be sent
	//	com->iResult = shutdown(com->ConnectSocket, SD_SEND);
	//
	//	if (com->iResult == SOCKET_ERROR) {
	//		printf("shutdown failed with error: %d\n", WSAGetLastError());
	//		closesocket(com->ConnectSocket);
	//		WSACleanup();
	//		return 1;
	//	}
	return 0;

}

int
	Logger::DB_Send(char *buf, size_t buflen, ServerCom *com)
{
	//if(!mConnected || com->ConnectSocket == INVALID_SOCKET)
	//	return 1;

	//com->iResult = send(com->ConnectSocket, buf, buflen, 0);

	//if (com->iResult == SOCKET_ERROR) {
	//	printf("send failed with error: %d\n", WSAGetLastError());
	//	closesocket(com->ConnectSocket);
	//	WSACleanup();
	//	return 2;
	//}
	return 0;
}

int 
	Logger::DB_CreateEntry()
{
	//sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "db.patients.insert({\"patient_id\":%s})",mCurrentID.c_str());

	//if(DB_Send(mPlyrBuf, DEFAULT_BUFSIZE, mSock) == 1)
	//{
	//	printf("DB_Send failed.\n");
	//	return 1;
	//}
	return 0;
}
