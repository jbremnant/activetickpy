#include "StdAfx.h"
#include "Helper.h"
#include "Session.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>


Session& Session::instance()
{
  static Session inst; // instantiated when this function is called
  return inst;
}

// private constructor
Session::Session(void)
  : m_hSession(0), m_hLastRequest(0), m_didLogin(false), m_loginCB(NULL)
{
  m_hSession = ATCreateSession();
}


Session::~Session(void)
{
  shutdown();
}


/*
  TODO:
  - setup backup server address
*/
bool Session::init(const ATGUID& apiUserid, const std::string& serverIpAddress,
                   uint32_t serverPort, const wchar16_t* userid, const wchar16_t* password)
{
	//if(Helper::StringLength(userid) >= _countof(m_userid) ||
  //   Helper::StringLength(password) >= _countof(m_password))
	//	return false;
  if(m_hSession == 0)
    m_hSession = ATCreateSession();

	Helper::CopyString(m_userid, userid);
	Helper::CopyString(m_password, password);

  char buf[50];
  wchar16_t *p;
  int i;
  for(i=0, p = m_userid; *p != 0; p++, i++)
  {
    buf[i] = (char) *p; // cast and truncate
    // printf("%04d: %c\n", i, (char)*p);
  }
  buf[i] = 0;

  // printf("session num: %u\n", m_hSession);
	bool rc = ATSetAPIUserId(m_hSession, (LPATGUID)&apiUserid);

	if(rc == true)
		rc = ATInitSession(
      m_hSession,
      serverIpAddress.c_str(),
      serverIpAddress.c_str(),
      serverPort,
      ATSessionStatusChangeCallback, // static callback function from this class
      true);

  // printf("initialized session for user: %s\n", buf);
	return rc;
}

bool Session::init(const std::string& apikey, const std::string& host, const std::string& port,
                   const std::string& user, const std::string& passwd)
{
  // first do conversions
  ATGUID guidApiUserid = Helper::StringToATGuid(apikey);
  std::vector<wchar16_t> userv = Helper::ConvertString(user);
  std::vector<wchar16_t> passv = Helper::ConvertString(passwd);
  uint32_t serverPort = atoi(port.c_str());
  return init(guidApiUserid, host, serverPort, &userv.front(), &passv.front());
}


bool Session::shutdown()
{
  if(m_hSession > 0)
  {
    // m_shutdown_mutex.lock();

    printf("INFO: shutting down session\n");
	  ATShutdownSession(m_hSession);

    printf("INFO: destroying session\n");
	  ATDestroySession(m_hSession);
    m_hSession = 0;

    // m_shutdown_mutex.unlock();
  }

  m_didLogin = false;
	return true;
}


/*static*/ void Session::ATSessionStatusChangeCallback(uint64_t hSession, ATSessionStatusType statusType)
{
	std::string strStatusType;
	switch(statusType)
	{
	  case SessionStatusConnected: strStatusType = "SessionStatusConnected"; break;
	  case SessionStatusDisconnected: strStatusType = "SessionStatusDisconnected"; break;
	  case SessionStatusDisconnectedDuplicateLogin: strStatusType = "SessionStatusDisconnectedDuplicateLogin"; break;
	  default: break;
	}

	printf("RECV: Status change [%s]\n", strStatusType.c_str());

	if(statusType == SessionStatusConnected)
	{
    // if connected, initiate login request, and save it
		Session::instance().m_hLastRequest = ATCreateLoginRequest(
      hSession,
      Session::instance().m_userid,
      Session::instance().m_password,
      Session::ATLoginResponseCallback);

    // send the request
		bool rc = ATSendRequest(
      hSession,
      Session::instance().m_hLastRequest,
      DEFAULT_REQUEST_TIMEOUT,
      Session::ATRequestTimeoutCallback);

    std::string user = Helper::ConvertString(Session::instance().m_userid, Helper::StringLength(Session::instance().m_userid));
		printf("SEND: (%d): Login request [%s] (rc=%d)\n", Session::instance().m_hLastRequest, user.c_str(), rc);
	}
  else
  {
    Session::instance().m_didLogin = false;
  }
}


/*static*/ void	Session::ATLoginResponseCallback(uint64_t hSession, uint64_t hRequest, LPATLOGIN_RESPONSE pResponse)
{
	std::string strLoginResponseType;
	switch(pResponse->loginResponse)
	{
	  case LoginResponseSuccess: strLoginResponseType = "LoginResponseSuccess"; break;
	  case LoginResponseInvalidUserid: strLoginResponseType = "LoginResponseInvalidUserid"; break;
	  case LoginResponseInvalidPassword: strLoginResponseType = "LoginResponseInvalidPassword"; break;
	  case LoginResponseInvalidRequest: strLoginResponseType = "LoginResponseInvalidRequest"; break;
	  case LoginResponseLoginDenied: strLoginResponseType = "LoginResponseLoginDenied"; break;
	  case LoginResponseServerError: strLoginResponseType = "LoginResponseServerError"; break;
	  default: strLoginResponseType = "unknown"; break;
	}

	printf("RECV: (%llu): Login response [%s]\n", hRequest, strLoginResponseType.c_str());

  if(Session::instance().m_loginCB != NULL)
  {
    Session::instance().m_loginCB(pResponse->loginResponse, strLoginResponseType);
  }

  // update the flag once other statements are done
  if(pResponse->loginResponse == LoginResponseSuccess)
  { 
    Session::instance().m_didLogin = true;
  }
}


/*static*/ void	Session::ATServerTimeUpdateCallback(LPATTIME pServerTime)
{
	printf("RECV: Server time update [%0.2d:%0.2d:%0.2d %0.2d/%0.2d/%0.4d\n",
		pServerTime->hour, pServerTime->minute, pServerTime->second,
		pServerTime->month, pServerTime->day, pServerTime->year);
}


/*static*/ void	Session::ATRequestTimeoutCallback(uint64_t hOrigRequest)
{
	printf("(%I64u): Request timed-out\n", hOrigRequest);
}
