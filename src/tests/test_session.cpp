#include <string>
#include <stdio.h>
#include "ActiveTickServerAPI.h"
// #include "ActiveTickServerRequestor.h"
#include "Session.h"
#include "Requestor.h"

void mycb(ATLoginResponseType status, std::string msg)
{
  if(status == LoginResponseSuccess)
  {
    printf("CB success! %s\n", msg.c_str());
  } else {
    printf("CB fail! %s\n", msg.c_str());
  }
}
void myticks(ATTickHistoryResponseType status, std::string symbol, const Requestor::DataVec& ticks)
{
  printf("symbol %s, received %d records\n", symbol.c_str(), ticks.size());
}

int main(int argc, char* argv[])
{
  ATInitAPI(); // done by the session

  if(argc < 6)
  {
    printf("usage: %s <apikey> <server> <port> <user> <passwd>\n", argv[0]);
    return 1;
  }

  {  // context sensitive destroy
    Session& s = Session::instance();
    Session& s2 = Session::instance();

    Requestor requestor(s, 100000);
    requestor.SetTickHistCB(myticks);
    // what happens when you create more than 1?
    // Session s2;

    printf("registering callback\n");
    s.registerLoginCallback(mycb);

    // apikey, server, port, user, passwd
    printf("running with: %s, %s, %s, %s, %s\n", argv[1], argv[2], argv[3], argv[4], argv[5]);
    s.init(argv[1], argv[2], argv[3], argv[4], argv[5]);
    printf("session1: %d, session2: %d\n", s.getSessionHandle(), s2.getSessionHandle());

    while(! s.didLogin())
    {
    }

    printf("returning from loop, session handle: %d\n", s.getSessionHandle());
    
    // ActiveTickServerRequestor requestor(s.getSessionHandle());
    // start the requestor

    requestor.MakeTickRequest("IBM", "20131014", "20131014");
    requestor.MakeTickRequest("GOOG", "20131014", "20131014");
    // requestor.MakeTickRequest("AAPL", "20131014", "20131014");
    // requestor.MakeTickRequest("BAC", "20131014", "20131014");

    while(true) {
      if(requestor.NumPendingRequests() == 0)
        break;
    }

    requestor.CloseAllATRequests();
  }

  ATShutdownAPI();

  return 0;
}
