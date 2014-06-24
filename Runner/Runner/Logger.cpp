#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>

#include "Logger.h"
#include "LoginWrapper.h"
#define DEFAULT_PORT "8080"
#define DEFAULT_HOST "creamstout.cs.usfca.edu"

/* A ServerCom struct contains all the necessary pieces for a socket connection */
struct ServerCom
{
  WSADATA wsaData;
  SOCKET ConnectSocket;// = INVALID_SOCKET;
  int iResult;
  struct addrinfo *result;// = NULL,
  struct addrinfo *ptr;// = NULL,
  struct addrinfo hints;

  ServerCom()
  {
    ConnectSocket = INVALID_SOCKET;
    result = NULL;
    ptr = NULL;
  }
};

Logger::Logger(LoginWrapper *login) : mPlyrData(), mSkelLock(), mPlyrLock(), mDbLock()
{
  mSeconds = 0;
  mMinutes = 0;
  mSeconds2 = 0;
  mMinutes2 = 0;
  mSock = new ServerCom();
  mScoreSock = new ServerCom();
  mLRSock = new ServerCom();
  mFBSock = new ServerCom();
  mHost = DEFAULT_HOST;
  mPort = DEFAULT_PORT;
  mLogin = login;
}

Logger::~Logger()
{
}


void
Logger::StartSession()
{
  /* Timestamp based on example from stack overflow on constructing stamp in
     ISO 8601 format. Initial timestamp used for file names to differentiate 
     between sessions. */

	if (!mLogin->loggedIn())
	{
		// Throw here?
		    printf("DB_Send failed.\n");

		    printf("Not logged in!\n");
		return;
	}
	mCurrentID = mLogin->getUsername();
  time(&mTimeStampBeg);
  
  strftime(mBegBuf, sizeof mBegBuf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&mTimeStampBeg));
  mSessionStarted = true;
  mSessionEnded = false;

  Connect();
  
  memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
  sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "db.patients.insert({\"patient_id\":%s, \"timestamp\":\"%s\"})", 
               mCurrentID.c_str(), mBegBuf);

  if(DB_Send(mPlyrBuf, DEFAULT_BUFSIZE, mSock) == 1)
  {
    printf("DB_Send failed.\n");
  }
  memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
  sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "In StartSession, starting daemon.\n");
  WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

  mDaemon = std::thread(&Logger::daemonFunc, this);
}

void
Logger::EndSession()
{
  mSessionEnded = true;
  time(&mTimeStampEnd);
  mDaemon.join();
  memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
  sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "In EndSession, daemon has joined.\n");
  WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);

  Disconnect();
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

void
Logger::daemonSendPlyrData(PlyrData *pdata)
{
  mSeconds++;
  mMinutes = mSeconds / 60;

  memset(mPlyrBuf, 0, DEFAULT_BUFSIZE);
  sprintf_s(mPlyrBuf, 
            DEFAULT_BUFSIZE, 
            "db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"scores.%d.%d\":%d}})", 
           mCurrentID.c_str(), 
            mBegBuf, 
            mMinutes, 
            mSeconds % 60, 
            pdata->totalScore);
  WriteToLog(mPlyrBuf, DEFAULT_BUFSIZE);
  if(DB_Send(mPlyrBuf, strlen(mPlyrBuf), mScoreSock) == 1)
    WriteToLog("DB_Send failed.\n", 18);
}

void
Logger::daemonSendSkelData(SkelData *sdata)
{
  mSeconds2++;
  mMinutes2 = mSeconds2 / 60;

  memset(mKinBuf1, 0, DEFAULT_BUFSIZE);
  sprintf_s(mKinBuf1, 
            DEFAULT_BUFSIZE, 
            "db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"skel_lr.%d.%d\":%f}})", 
           mCurrentID.c_str(), 
            mBegBuf, 
            mMinutes2, 
            mSeconds2 % 60, 
            sdata->lrAngle);
  WriteToLog(mKinBuf1, DEFAULT_BUFSIZE);
  if(DB_Send(mKinBuf1, strlen(mKinBuf1), mLRSock) == 1)
    WriteToLog("DB_Send failed.\n", 18);

    memset(mKinBuf1, 0, DEFAULT_BUFSIZE);
  sprintf_s(mKinBuf1, 
            DEFAULT_BUFSIZE, 
            "db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"skel_lr_true.%d.%d\":%f}})", 
           mCurrentID.c_str(), 
            mBegBuf, 
            mMinutes2, 
            mSeconds2 % 60, 
            sdata->lrAngleTrue);
  WriteToLog(mKinBuf1, DEFAULT_BUFSIZE);
  if(DB_Send(mKinBuf1, strlen(mKinBuf1), mLRSock) == 1)
    WriteToLog("DB_Send failed.\n", 18);

  memset(mKinBuf2, 0, DEFAULT_BUFSIZE);
  sprintf_s(mKinBuf2, 
            DEFAULT_BUFSIZE, 
            "db.patients.update({\"patient_id\":%s, \"timestamp\": \"%s\"}, {$set: {\"skel_fb.%d.%d\":%f}})", 
           mCurrentID.c_str(), 
            mBegBuf, 
            mMinutes2, 
            mSeconds2 % 60, 
            sdata->fbAngle);
  WriteToLog(mKinBuf2, DEFAULT_BUFSIZE);
  if(DB_Send(mKinBuf2, strlen(mKinBuf2), mFBSock) == 1)
    WriteToLog("DB_Send failed.\n", 18);
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

int
Logger::Connect(void)
{
  int res1, res2, res3, res4;
  res1 = DB_Connect(mHost, mPort, mScoreSock);
  res2 = DB_Connect(mHost, mPort, mLRSock);
  res3 = DB_Connect(mHost, mPort, mFBSock);
  res4 = DB_Connect(mHost, mPort, mSock);
  return (res1 && res2 && res3);
}

void
Logger::Disconnect(void)
{
  DB_Disconnect(mScoreSock);
  DB_Disconnect(mLRSock);
  DB_Disconnect(mFBSock);
  DB_Disconnect(mSock);
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


  // Initialize Winsock
  com->iResult = WSAStartup(MAKEWORD(2,2), &com->wsaData);
  if (com->iResult != 0) {
    printf("WSAStartup failed with error: %d\n", com->iResult);
    return 1;
  }

  ZeroMemory( &com->hints, sizeof(com->hints) );
  com->hints.ai_family = AF_UNSPEC;
  com->hints.ai_socktype = SOCK_STREAM;
  com->hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  com->iResult = getaddrinfo(hostname, port, &com->hints, &com->result);

  if ( com->iResult != 0 ) {
    printf("getaddrinfo failed with error: %d\n", com->iResult);
    WSACleanup();
    return 2;
  }

  // Attempt to connect to an address until one succeeds
  for(com->ptr=com->result; com->ptr != NULL ;com->ptr=com->ptr->ai_next) {

    // Create a SOCKET for connecting to server
    com->ConnectSocket = socket(com->ptr->ai_family, com->ptr->ai_socktype, 
      com->ptr->ai_protocol);
    if (com->ConnectSocket == INVALID_SOCKET) 
    {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      WSACleanup();
      return 3;
    }

    // Connect to server.
    com->iResult = connect( com->ConnectSocket, com->ptr->ai_addr, (int)com->ptr->ai_addrlen);
    if (com->iResult == SOCKET_ERROR) {
      closesocket(com->ConnectSocket);
      com->ConnectSocket = INVALID_SOCKET;
      continue;
    }
    break;
  }

  freeaddrinfo(com->result);

  if (com->ConnectSocket == INVALID_SOCKET) {
    printf("Unable to connect to server!\n");
    WSACleanup();
    return 4;
  }
  mConnected = true;
  return 0;
}

int
Logger::DB_Disconnect(ServerCom *com)
{
  // shutdown the connection since no more data will be sent
  com->iResult = shutdown(com->ConnectSocket, SD_SEND);

  if (com->iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(com->ConnectSocket);
    WSACleanup();
    return 1;
  }
  return 0;
}

int
Logger::DB_Send(char *buf, size_t buflen, ServerCom *com)
{
  if(!mConnected || com->ConnectSocket == INVALID_SOCKET)
    return 1;

  com->iResult = send(com->ConnectSocket, buf, buflen, 0);

  if (com->iResult == SOCKET_ERROR) {
    printf("send failed with error: %d\n", WSAGetLastError());
    closesocket(com->ConnectSocket);
    WSACleanup();
    return 2;
  }
  return 0;
}

int 
Logger::DB_CreateEntry()
{
  sprintf_s(mPlyrBuf, DEFAULT_BUFSIZE, "db.patients.insert({\"patient_id\":%s})",mCurrentID.c_str());

  if(DB_Send(mPlyrBuf, DEFAULT_BUFSIZE, mSock) == 1)
  {
    printf("DB_Send failed.\n");
    return 1;
  }
  return 0;
}
