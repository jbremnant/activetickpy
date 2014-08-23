#pragma once

#include <Shared/ATServerAPIDefines.h>
#include <list>
#include <map>
#include <string>

namespace boost { class recursive_mutex; }

class ActiveTickServerRequestor;
class ServerConnection;
class StreamQueue;
class StreamSubscriptions;

/**
* ActiveTickStreamListener class.
* This class is used as generic listener for all ActiveTick's API streams. Extend this class and override its virtual functions to easily receive streaming notifications.
* @note In order to use this class for notifications, derived class also needs to extend ActiveTickServerRequestor class. The Requestor class is used for sending stream requests, and Listener class is used for receiving them.
* @see ActiveTickServerRequestor
*/
class ActiveTickStreamListener
{
	friend class ActiveTickServerRequestor;
	friend class ServerConnection;
	friend class StreamQueue;
	friend class StreamSubscriptions;
	friend ACTIVETICKSERVERAPI_API bool ATInitAPI();
	friend ACTIVETICKSERVERAPI_API bool	ATShutdownAPI();

public:
	/**
	* ActiveTickStreamListener constructor.
	* @param session Session handle created by ATCreateSession function.
	* @param processAllUpdates If set to false, each notification will be checked against all subscribed symbols.
	* @see ATCreateSession
	*/
	ACTIVETICKSERVERAPI_API ActiveTickStreamListener(uint64_t session, bool processAllUpdates = false);

	/**
	* ActiveTickStreamListener destructor.
	*/
	ACTIVETICKSERVERAPI_API virtual ~ActiveTickStreamListener(void);

protected:
	/**
	* Virtual method which is called by ActiveTick API when trade message is received for subscribed symbol.
	* @param pUpdate Pointer to ATQUOTESTREAM_TRADE_UPDATE containing trade notification data.
	* @see ATQUOTESTREAM_TRADE_UPDATE
	* @see ActiveTickServerRequestor::SendATQuoteStreamRequest
	*/
	ACTIVETICKSERVERAPI_API virtual void	OnATStreamTradeUpdate(LPATQUOTESTREAM_TRADE_UPDATE pUpdate) {}

	/**
	* Virtual method which is called by ActiveTick API when quote message is received for subscribed symbol.
	* @param pUpdate Pointer to ATQUOTESTREAM_QUOTE_UPDATE containing quote notification data.
	* @see ATQUOTESTREAM_QUOTE_UPDATE
	* @see ActiveTickServerRequestor::SendATQuoteStreamRequest
	*/
	ACTIVETICKSERVERAPI_API virtual void	OnATStreamQuoteUpdate(LPATQUOTESTREAM_QUOTE_UPDATE pUpdate) {}

	/**
	* Virtual method which is called by ActiveTick API when refresh message is received for subscribed symbol.
	* @param pUpdate Pointer to ATQUOTESTREAM_REFRESH_UPDATE containing refresh notification data.
	* @see ATQUOTESTREAM_REFRESH_UPDATE
	* @see ActiveTickServerRequestor::SendATQuoteStreamRequest
	*/
	ACTIVETICKSERVERAPI_API virtual void	OnATStreamRefreshUpdate(LPATQUOTESTREAM_REFRESH_UPDATE pUpdate) {}

	/**
	* Virtual method which is called by ActiveTick API when market movers message is received for subscribed symbol.
	* @param pUpdate Pointer to ATMARKET_MOVERS_STREAM_UPDATE containing market movers notification data.
	* @see ATMARKET_MOVERS_STREAM_UPDATE
	* @see ActiveTickServerRequestor::SendATQuoteStreamRequest
	*/
	ACTIVETICKSERVERAPI_API virtual void	OnATStreamTopMarketMoversUpdate(LPATMARKET_MOVERS_STREAM_UPDATE pUpdate) {}

private:
	void			AddSymbols(const LPATSYMBOL pSymbols, uint16_t symbolCount, ATStreamRequestType requestType);
	void			RemoveSymbols(const LPATSYMBOL pSymbols, uint16_t symbolCount, ATStreamRequestType requestType);
	void			ProcessStreamUpdate(LPATSTREAM_UPDATE pUpdate);

private:
	static void		Init();
	static void		Destroy();
	static void		ProcessStreamUpdateForAllListeners(LPATSTREAM_UPDATE pUpdate);

private:
	uint64_t		m_session;
	bool			m_bProcessAllUpdates;

	std::map<std::string, int32_t>	m_mapSymbols[3];
	boost::recursive_mutex*			m_mutexSymbols;

private:
	static std::list<ActiveTickStreamListener*>*	s_lstListeners;
	static boost::recursive_mutex*					s_mutexListeners;
};
