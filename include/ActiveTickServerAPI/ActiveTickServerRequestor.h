#pragma once

#include "ActiveTickServerAPI.h"

#include <map>
#include <string>

namespace boost { class recursive_mutex; }

/**
* ActiveTickServerRequestor class. 
* This is used as generic requestor for all ActiveTick's API requests. Extend this class and override its virtual functions to easily receive various notifications.
* @note In order to receive stream notifications, derived class must also extend from ActiveTickStreamListener class.
* @see ActiveTickStreamListener
*/
class ActiveTickServerRequestor
{
	friend ACTIVETICKSERVERAPI_API bool ATInitAPI();
	friend ACTIVETICKSERVERAPI_API bool	ATShutdownAPI();

public:
	/**
	* ActiveTickServerRequestor constructor.
	* @param session Session handle created by ATCreateSession function.
	* @see ATCreateSession
	*/
	ACTIVETICKSERVERAPI_API ActiveTickServerRequestor(uint64_t session);

	/**
	* ActiveTickServerRequestor destructor.
	*/
	ACTIVETICKSERVERAPI_API virtual ~ActiveTickServerRequestor(void);

public:
	/**
	* Closes all outstanding requests.
	* @return uint32_t Number of requests closed.
	*/
	ACTIVETICKSERVERAPI_API uint32_t			CloseAllATRequests();

	/**
	* Close specific request.
	* @param request Request handle for which to close the request.
	* @return true if successfully closed the request, false otherwise.
	*/
	ACTIVETICKSERVERAPI_API bool				CloseATRequest(uint64_t request);
	
	/**
	* Send bar history request.
	* @return Handle to newly created request.
	* @see ATCreateBarHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATBarHistoryDbRequest(const ATSYMBOL& symbol, ATBarHistoryType barHistoryType, uint8_t intradayMinuteCompression, const ATTIME& beginDateTime, 
													const ATTIME& endDateTime, uint32_t timeout);

	/**
	* Send bar history request.
	* @return Handle to newly created request.
	* @see ATCreateBarHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATBarHistoryDbRequest(const ATSYMBOL& symbol, ATBarHistoryType barHistoryType, uint8_t intradayMinuteCompression, uint32_t recordsWanted, uint32_t timeout);

	/**
	* Send bar history request.
	* @return Handle to newly created request.
	* @see ATCreateBarHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATBarHistoryDbRequest(const ATSYMBOL& symbol, ATBarHistoryType barHistoryType, uint8_t intradayMinuteCompression, const ATTIME& beginDateTime, 
													uint32_t recordsWanted, ATCursorType cursorType, uint32_t timeout);

	/**
	* Send login request.
	* @return Handle to newly created request.
	* @see ATCreateLoginRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATLoginRequest(const wchar16_t* userid, const wchar16_t* password, uint32_t timeout);

	/**
	* Send market holidays list request.
	* @return Handle to newly created request.
	* @see ATCreateMarketHolidaysRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATMarketHolidaysRequest(uint8_t yearsGoingBack, uint8_t yearsGoingForward, ATExchangeType exchangeType, ATCountryType countryType, uint32_t timeout);

	/**
	* Send market movers snapshot request.
	* @return Handle to newly created request.
	* @see ATCreateMarketMoversDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATMarketMoversDbRequest(const LPATSYMBOL symbols, uint16_t symbolCount, uint32_t timeout);

	/**
	* Send market movers stream request.
	* @note In order to receive stream notifications, extended derived class from ActiveTickStreamListener, and override necessary virtual methods.
	* @return Handle to newly created request.
	* @see ATCreateMarketMoversStreamRequest	
	* @see ActiveTickStreamListener
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATMarketMoversStreamRequest(const ATSYMBOL& symbol, ATStreamRequestType requestType, uint32_t timeout);	

	/**
	* Send quote snapshot request.
	* @return Handle to newly created request.
	* @see ATCreateQuoteDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATQuoteDbRequest(const LPATSYMBOL symbols, uint16_t symbolCount, const ATQuoteFieldType* fields, uint16_t fieldCount, uint32_t timeout);	

	/**
	* Send quote stream request.
	* @return Handle to newly created request.
	* @note In order to receive stream notifications, extended derived class from ActiveTickStreamListener, and override necessary virtual methods.
	* @see ATCreateQuoteStreamRequest	
	* @see ActiveTickStreamListener
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATQuoteStreamRequest(const LPATSYMBOL symbols, uint16_t symbolCount, ATStreamRequestType requestType, uint32_t timeout);	

	/**
	* Send tick history data request.
	* @return Handle to newly created request.
	* @see ATCreateTickHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATTickHistoryDbRequest(const ATSYMBOL& symbol, bool selectTrades, bool selectQuotes, const ATTIME& beginDateTime, const ATTIME& endDateTime, uint32_t timeout);

	/**
	* Send tick history data request.
	* @return Handle to newly created request.
	* @see ATCreateTickHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATTickHistoryDbRequest(const ATSYMBOL& symbol, bool selectTrades, bool selectQuotes, uint32_t recordsWanted, uint32_t timeout);

	/**
	* Send tick history data request.
	* @return Handle to newly created request.
	* @see ATCreateTickHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATTickHistoryDbRequest(const ATSYMBOL& symbol, bool selectTrades, bool selectQuotes, const ATTIME& beginDateTime, uint32_t recordsWanted, ATCursorType cursorType, uint32_t timeout);

	/**
	* Send tick history data request.
	* @return Handle to newly created request.
	* @see ATCreateTickHistoryDbRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATTickHistoryDbRequest(const ATSYMBOL& symbol, bool selectTrades, bool selectQuotes, uint32_t pagesWanted, uint64_t offset, const ATTIME& dbdate, uint32_t timeout);

	/**
	* Send sector list request. The response will contain sector/industry pairs for all defined sectors.
	* @return Handle to newly created request.
	* @see ATCreateSectorListRequest	
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATSectorListRequest(uint32_t timeout);

	/**
	* Send constituent list request. The response will contain a list of symbols for key.
	* @param constituentListType One of ATConstituentListType types.
	* @param key Specifies the key for which to retrieve the list. If constituentListType is sector, the key should contain SectorName IndustryName with 0x10 character between the names, 
	* for example: "Services" 0x10 "Restaurants". If listType is index, then key should contain an index symbol, for example $DJI. For options, the key should be the underlying symbol.
	* @param timeout Timeout value in milliseconds
	* @return Handle to newly created request.
	* @see ATCreateConstituentListRequest
	*/
	ACTIVETICKSERVERAPI_API uint64_t			SendATConstituentListRequest(ATConstituentListType constituentListType, const wchar16_t* key, uint32_t timeout);

protected:

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATBarHistoryResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATBarHistoryDbResponse(uint64_t origRequest, ATBarHistoryResponseType responseType, LPATBARHISTORY_RESPONSE pResponse) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATLoginResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATLoginResponse(uint64_t origRequest, LPATLOGIN_RESPONSE pResponse) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATMarketHolidaysResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnAtMarketHolidaysResponse(uint64_t origRequest, LPATMARKET_HOLIDAYSLIST_ITEM pItems, uint32_t itemsCount) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATMarketMoversDbResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATMarketMoversDbResponse(uint64_t origRequest, ATMarketMoversDbResponseType responseType, LPATMARKET_MOVERSDB_RESPONSE pResponse) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATMarketMoversStreamResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATMarketMoversStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATMARKET_MOVERS_STREAM_RESPONSE pResponse) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATQuoteDbResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATQuoteDbResponse(uint64_t origRequest, ATQuoteDbResponseType responseType, LPATQUOTEDB_RESPONSE pResponse, uint32_t responseCount) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATQuoteStreamResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATQuoteStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATQUOTESTREAM_RESPONSE pResponse, uint32_t responseCount) {}

	/**
	* Virtual override callback that will be called when time out occurs.
	* @see ATRequestTimeoutCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATRequestTimeout(uint64_t origRequest) {}

	/**
	* Virtual override callback that will be called once response has been received from the server.
	* @see ATTickHistoryResponseCallback
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATTickHistoryDbResponse(uint64_t origRequest, ATTickHistoryResponseType responseType, LPATTICKHISTORY_RESPONSE pResponse) {}	

	/**
	* Virtual override callback that will be called for response to sector list request.
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATSectorListResponse(uint64_t origRequest, LPATSECTORLIST_RECORD pRecords, uint32_t recordsCount) {}

	/**
	* Virtual override callback that will be called for response to constituent list request.
	*/
	ACTIVETICKSERVERAPI_API virtual void		OnATConstituentListResponse(uint64_t origRequest, LPATSYMBOL pSymbols, uint32_t symbolsCount) {}

private:
	void				RemoveRequestFromMap(uint64_t request);

private:
	static void 		ATBarHistoryDbResponseCallback(uint64_t origRequest, ATBarHistoryResponseType responseType, LPATBARHISTORY_RESPONSE pResponse);
	static void 		ATLoginResponseCallback(uint64_t hSession, uint64_t hRequest, LPATLOGIN_RESPONSE pResponse);
	static void 		ATMarketHolidaysResponseCallback(uint64_t origRequest, LPATMARKET_HOLIDAYSLIST_ITEM pItems, uint32_t itemsCount);
	static void 		ATMarketMoversDbResponseCallback(uint64_t origRequest, ATMarketMoversDbResponseType responseType, LPATMARKET_MOVERSDB_RESPONSE pResponse);
	static void 		ATMarketMoversStreamResponseCallback(uint64_t origRequest, ATStreamResponseType responseType, LPATMARKET_MOVERS_STREAM_RESPONSE pResponse);
	static void 		ATQuoteDbResponseCallback(uint64_t origRequest, ATQuoteDbResponseType responseType, LPATQUOTEDB_RESPONSE pResponse, uint32_t responseCount);
	static void 		ATQuoteStreamResponseCallback(uint64_t origRequest, ATStreamResponseType responseType, LPATQUOTESTREAM_RESPONSE pResponse, uint32_t responseCount);
	static void 		ATRequestTimeoutCallback(uint64_t origRequest);
	static void 		ATTickHistoryResponseCallback(uint64_t origRequest, ATTickHistoryResponseType responseType, LPATTICKHISTORY_RESPONSE pResponse);	
	static void			ATSectorListResponseCallback(uint64_t origRequest, LPATSECTORLIST_RECORD pRecords, uint32_t recordsCount);
	static void			ATConstituentListResponseCallback(uint64_t origRequest, LPATSYMBOL pSymbols, uint32_t symbolsCount);

private:
	static void			Init();
	static void			Destroy();

private:
	uint64_t			m_session;
	
	static std::map<uint64_t, ActiveTickServerRequestor*>*	s_mapRequests;
	static boost::recursive_mutex*							s_mutexRequests;
};
