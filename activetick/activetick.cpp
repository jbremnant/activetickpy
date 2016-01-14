#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <Python.h>
#include <ActiveTickServerAPI/ActiveTickServerAPI.h>
#include "Helper.h"
#include "Session.h"
#include "Requestor.h"

#define VER "0.1"

#ifdef __cplusplus
extern "C" {
#endif

// Global variables
static Session   *psession = NULL; // global pointer to session
static Requestor *prequestor = NULL; // global pointer to session

static PyObject *ATPyError;
static PyObject *_py_login_callback = NULL;
static PyObject *_py_tickhist_callback = NULL;

/**********************************************************************************************
 * _activetick.version()
 **********************************************************************************************/
static char doc_version[] = "Gives back version of the underlying activetick library and this lib";
static PyObject* version(PyObject *self, PyObject *args)
{
  uint32_t apiver = ATGetAPIVersion();
  char buf[50];
  snprintf(buf, 50, "AT.ver:%d, ATpy.ver:%s", apiver, VER);
  // printf("buf: %s\n", buf);
  return Py_BuildValue("s", buf);
}

/**********************************************************************************************
 * _activetick.connect()
 *    Initializes the AT session. Returns the session number.
 **********************************************************************************************/
static char doc_connect[] = "Connects to activetick server.\n\
  Usage: at.connect(apikey, host, port, user, pass)\nArguments are all strings";
static PyObject* connect(PyObject *self, PyObject *args) // or third param PyObject *kwargs with METH_KEYWORDS def
{
  const char *apikey;
  const char *host;
  const char *port;
  const char *user;
  const char *pass;

  if(!PyArg_ParseTuple(args, "sssss", &apikey, &host, &port, &user, &pass))  // or PyArg_ParseTupleAndKeywords()
    return NULL;

  std::string str_apikey(apikey);
  std::string str_host(host);
  std::string str_port(port);
  std::string str_user(user);
  std::string str_pass(pass);

  psession->init(str_apikey, str_host, str_port, str_user, str_pass);

    // PyErr_SetString(AtPyError, "System command failed");
    // return NULL;

  printf("session num: %d", (uint64_t) (*psession));
  return Py_BuildValue("l", psession->getSessionHandle());
}


/**********************************************************************************************
 * _activetick.did_login()
 *    Initializes the AT session. Returns the session number.
 **********************************************************************************************/
static char doc_did_login[] = "Checks the login status, returns true on successful login.\n\
Polling is required because connect is setup by a different threa";
static PyObject* did_login(PyObject *self, PyObject *args)
{
  bool loggedin = psession->didLogin();
  return Py_BuildValue("i", (int) loggedin);
}

/**********************************************************************************************
 * _activetick.set_login_callback()
 *    Initializes the AT session. Returns the session number.
 **********************************************************************************************/
static char doc_set_login_callback[] = "Sets up the callback function for login. Callback signature:\n\
  def mylogincb(int, str)";
static PyObject* set_login_callback(PyObject *self, PyObject *args)
{
  PyObject *result = NULL;
  PyObject *temp = NULL;

  // if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
  if (PyArg_ParseTuple(args, "O", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    printf("setting login callback function\n");
    Py_XINCREF(temp);                // Add a reference to new callback
    Py_XDECREF(_py_login_callback);  // Dispose of previous callback
    _py_login_callback = temp;       // Remember new callback
    // Py_RETRUN_NONE; is equivalent to Py_INCREF(Py_None); return Py_None;
    Py_INCREF(Py_None);              // Boilerplate to return "None" 
    result = Py_None;
 }
 return result;
}


/*
  Caveats: http://stackoverflow.com/questions/16606872/calling-python-method-from-c-or-c-callback

  Takes care of calling the python callback function from within Session callback mechanism.
  This function seems be called from a different thread from AT library. Therefore, GIL needs
  to be acquired in order to manipulate the python objects.
 */
static void login_callback_wrapper(ATLoginResponseType status, std::string msg)
{

  printf("msg: %s\n", msg.c_str());
  if(_py_login_callback == NULL)
    return;

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure(); // acquire lock

  printf("attempt to call python callback function\n");
  PyObject *arglist;
  PyObject *result;

  arglist = Py_BuildValue("(is)", (int) status, msg.c_str());
  result = PyObject_CallObject(_py_login_callback, arglist);
  Py_DECREF(arglist);

  // Release the thread. No Python API allowed beyond this point.
  PyGILState_Release(gstate); 
}



/**********************************************************************************************
 * _activetick.set_login_callback()
 *    Initializes the AT session. Returns the session number.
 **********************************************************************************************/
static char doc_set_tickhist_callback[] = "Sets up callback for tick history data";
static PyObject* set_tickhist_callback(PyObject *self, PyObject *args)
{
  PyObject *result = NULL;
  PyObject *temp = NULL;

  // if (PyArg_ParseTuple(args, "O:set_callback", &temp)) {
  if (PyArg_ParseTuple(args, "O", &temp)) {
    if (!PyCallable_Check(temp)) {
      PyErr_SetString(PyExc_TypeError, "parameter must be callable");
      return NULL;
    }
    printf("setting tickhist callback function\n");
    Py_XINCREF(temp);                // Add a reference to new callback
    Py_XDECREF(_py_tickhist_callback);  // Dispose of previous callback
    _py_tickhist_callback = temp;       // Remember new callback
    // Py_RETRUN_NONE; is equivalent to Py_INCREF(Py_None); return Py_None;
    Py_INCREF(Py_None);              // Boilerplate to return "None" 
    result = Py_None;
 }
 return result;
}


/*
From Requestor.h
  struct DataTick {
    ATTickHistoryRecordType type;  // parser.GetRecordType())
    ATTIME datetime;               // parser.GetRecordDateTime();
    ATPRICE tp;                    // parser.GetTradeLastPrice().precision, parser.GetTradeLastPrice().price,
    uint32_t ts;                   // parser.GetTradeLastSize()
    ATExchangeType texch;          // parser.GetTradeLastExchange(),  // ATExchangeType == char
    ATTradeConditionType tcond[4]; // parser.GetTradeCondition(0);  NOTE: this is only the first condition!!
                                   // ATTradeConditionsCount == 4
    ATPRICE bp;                    // parser.GetQuoteBidPrice().precision, parser.GetQuoteBidPrice().price,
    ATPRICE ap;                    // parser.GetQuoteAskPrice().precision, parser.GetQuoteAskPrice().price,
    uint32_t bs;                   // parser.GetQuoteBidSize()
    uint32_t as;                   // parser.GetQuoteAskSize(),
    ATExchangeType bexch;          // parser.GetQuoteBidExchange(),
    ATExchangeType aexch;          // parser.GetQuoteAskExchange(),
    ATQuoteConditionType qcond;          // parser.GetQuoteCondition();
  };
*/
static void tickhist_callback_wrapper(ATTickHistoryResponseType status,
                                       std::string symbol, const Requestor::DataVec& ticks)
{
  // call the python function here
  if(_py_tickhist_callback == NULL)
  {
    printf("tickhist_callback is NULL. returning\n");
    return;
  }

  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure(); // acquire lock

  PyObject *list = PyList_New( ticks.size() ); // we know the size before hand
  
  // printf("callback wrapper: symbol %s, received %d records\n", symbol.c_str(), ticks.size());
  Requestor::DataVec::const_iterator it;
  int i;
  for(it=ticks.begin(), i=0; it!=ticks.end(); it++, i++)
  {
    ATTickHistoryRecordType type = it->type;
    std::string dt = Helper::ATTimeToString(it->datetime);

    // if(type == TickHistoryRecordTrade)
    // {
    //   printf("T,%s,%s,%.3f\n", symbol.c_str(), dt.c_str(), it->tp.price);
    // } else {
    //   printf("Q,%s,%s,%.3f,%.3f\n", symbol.c_str(), dt.c_str(), it->bp.price, it->ap.price);
    // }

    // list of dictionaries - easy to import into pandas
    // probably could create numpy arrays directly.
    PyObject *dict = Py_BuildValue("{s:s,s:s,s:f,s:i,s:i,s:i,s:f,s:f,s:i,s:i,s:i,s:i,s:i}",
      "type", (type==TickHistoryRecordTrade) ? "T":"Q",
      "datetime", dt.c_str(),
      "tp", it->tp.price,
      "ts", it->ts,
      "tc", it->tcond[0],
      "tex", it->texch,
      "bp", it->bp.price,
      "ap", it->ap.price,
      "bs", it->bs,
      "as", it->as,
      "bex", it->bexch,
      "aex", it->aexch,
      "qc", it->qcond
      );
    PyList_SetItem(list, i, dict);
  }
  // printf("current requests: %d\n", prequestor->NumPendingRequests());

  PyObject *arglist = Py_BuildValue("(sO)", symbol.c_str(), list);
  PyObject *result = PyObject_CallObject(_py_tickhist_callback, arglist);
  Py_DECREF(arglist);
  Py_DECREF(list);

  PyGILState_Release(gstate); 
}


/**********************************************************************************************
 * _activetick.make_tick_request()
 *    Initiates tick history requests. The callback function receives the data
 **********************************************************************************************/
static char doc_make_tick_request[] = "Creates tick history data request.\n\
  Usage: at.make_tick_request(ticker, startdate, enddate)\nArguments are all strings";
static PyObject* make_tick_request(PyObject *self, PyObject *args) // or third param PyObject *kwargs with METH_KEYWORDS def
{
  const char *ticker;
  const char *sdate;
  const char *edate;

  if(!PyArg_ParseTuple(args, "sss", &ticker, &sdate, &edate))  // or PyArg_ParseTupleAndKeywords()
    return NULL;

  printf("creating tick request with: %s, %s - %s\n", ticker, sdate, edate);
  prequestor->MakeTickRequest(std::string(ticker), std::string(sdate), std::string(edate));
  Py_RETURN_NONE;
}


/**********************************************************************************************
 * _activetick.num_pending_requests()
 *    Returns the number of pending requests in the internal queue.
 **********************************************************************************************/
static char doc_num_pending_requests[] = "Returns the number of requests currently pending\n";
static PyObject* num_pending_requests(PyObject *self, PyObject *args) // or third param PyObject *kwargs with METH_KEYWORDS def
{
  uint64_t num = prequestor->NumPendingRequests();
  // return Py_BuildValue("l", num); 
  return PyLong_FromLong(num);
}


/**********************************************************************************************
 * Method Registration
 **********************************************************************************************/
static PyMethodDef ATPyMethods[] = {
  {"version", version, METH_VARARGS, doc_version},
  {"did_login", did_login, METH_VARARGS, doc_did_login},
  {"connect", connect, METH_VARARGS, doc_connect},
  {"set_login_callback", set_login_callback, METH_VARARGS, doc_set_login_callback},
  {"set_tickhist_callback", set_tickhist_callback, METH_VARARGS, doc_set_tickhist_callback},
  {"make_tick_request", make_tick_request, METH_VARARGS, doc_make_tick_request},
  {"num_pending_requests", num_pending_requests, METH_VARARGS, doc_num_pending_requests},
  {NULL, NULL, 0, NULL}
};


void cleanup(void)
{
  printf("calling ATShutdownAPI()\n");
  psession = NULL;
  if(prequestor != NULL)
  {
    prequestor->CloseAllATRequests();
    delete prequestor;
  }
  prequestor = NULL;
  ATShutdownAPI();
}


PyMODINIT_FUNC // void return type, and extern-ed for c++
init_activetick(void) // has to be named init<module>
{
  // Make sure the GIL has been created since we need to acquire it in our
  // callback to safely call into the python application.
  if (! PyEval_ThreadsInitialized()) {
      PyEval_InitThreads();
  }

  ATInitAPI();  
  // atexit(cleanup); // redundant
  Py_AtExit(cleanup);

  {
    Session& session = Session::instance();
    psession = &session; // set the pointer
    session.registerLoginCallback(login_callback_wrapper);

    prequestor = new Requestor(session);
    prequestor->setBatchSize(100000);
    prequestor->SetTickHistCB(tickhist_callback_wrapper);

    PyObject *m;
    m = Py_InitModule("_activetick", ATPyMethods);
    if (m == NULL)
      return;

    ATPyError = PyErr_NewException("_activetick.error", NULL, NULL);
    Py_INCREF(ATPyError);
    PyModule_AddObject(m, "error", ATPyError);
  }
}


#ifdef __cplusplus
}
#endif
