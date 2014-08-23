#pragma once
#include <ActiveTickServerAPI/ActiveTickServerAPI.h>
#include <string>
// #include <mutex> // only supported on C++11

// simple singleton class
class Session
{
public:
  static Session& instance(); // singleton class since we want just 1 session per process

	uint64_t getSessionHandle() const { return m_hSession; }
  operator uint64_t() { return m_hSession; } // convenience type cast

	bool		init(const ATGUID& apiUserid, const std::string& serverIpAddress, uint32_t serverPort,
               const wchar16_t* userid, const wchar16_t* password);
  // for convenience
  bool    init(const std::string& apikey, const std::string& serverAddress, const std::string& serverPort,
               const std::string& userid, const std::string& passwd);
	bool		shutdown();
  bool    didLogin() { return m_didLogin; }

  typedef void (*LoginCB)(ATLoginResponseType status, std::string msg);
  void    registerLoginCallback(LoginCB cb_) { m_loginCB = cb_; };

private:
  // c++ Singleton pattern using static variable
	Session();
  Session(const Session& other);
	virtual ~Session(void);

	static void ATSessionStatusChangeCallback(uint64_t hSession, ATSessionStatusType statusType);
	static void ATLoginResponseCallback(uint64_t hSession, uint64_t hRequest, LPATLOGIN_RESPONSE pResponse);
	static void ATServerTimeUpdateCallback(LPATTIME pServerTime);
	static void ATRequestTimeoutCallback(uint64_t hOrigRequest);	

	uint64_t		m_hSession;
	uint64_t		m_hLastRequest;
  bool        m_didLogin;

  ATGUID      m_atuserkey;
	wchar16_t		m_userid[50];
	wchar16_t		m_password[50];
  LoginCB     m_loginCB;
  // std::mutex m_shutdown_mutex;
};
