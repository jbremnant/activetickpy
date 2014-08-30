#pragma once
#include "Session.h"
#include <map>
#include <vector>
#include <ActiveTickServerRequestor.h>


class Requestor : public ActiveTickServerRequestor
{
public:
	Requestor(const Session& session);
	Requestor(const Session& session, int batch_size_);
	virtual ~Requestor(void);

  typedef struct {
    ATSYMBOL symbol;
    ATTIME starttime;
    ATTIME endtime;
    int retrievedRecords; // retain the total number of records per name
  } RequestItem;

  typedef std::map<uint64_t, RequestItem> Requests;
  typedef std::pair<uint64_t, RequestItem> RequestPair;

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
  
  typedef std::vector<DataTick> DataVec; // stores a copy
  typedef std::map<std::string, DataVec> Ticks; // mapping of symbol -> data vector

  // support for external callbacks
  typedef void (*TickHistoryCB)(ATTickHistoryResponseType status, std::string symbol, const DataVec& ticks);

  // uint64_t SendATTickHistoryDbRequest(const ATSYMBOL& symbol, bool selectTrades, bool selectQuotes, const ATTIME& beginDateTime, const ATTIME& endDateTime, uint32_t timeout);
  void SetTickHistCB(TickHistoryCB cb_)  { m_tick_cb = cb_; }
  uint64_t MakeTickRequest(const std::string& symbol, const std::string& sdate, const std::string& edate); 

  int NumPendingRequests() { return m_requests.size(); }
  void setBatchSize(int batch_size) { m_batch_size = batch_size; }


private:
  int m_batch_size;        // we want to batch the results so that external callbacks get response in bulk
  int m_tot_requests;  // per symbol number of "bad" requests

  Requests m_requests; // save the outstanding request
  Ticks m_ticks;
  TickHistoryCB m_tick_cb;
  
	// ActiveTickServerRequestor's virtual overrides
	virtual void		OnATBarHistoryDbResponse(uint64_t origRequest, ATBarHistoryResponseType responseType, LPATBARHISTORY_RESPONSE pResponse);
	virtual void		OnATTickHistoryDbResponse(uint64_t origRequest, ATTickHistoryResponseType responseType, LPATTICKHISTORY_RESPONSE pResponse);

	virtual void		OnATMarketMoversDbResponse(uint64_t origRequest, ATMarketMoversDbResponseType responseType, LPATMARKET_MOVERSDB_RESPONSE pResponse);
	virtual void		OnATQuoteDbResponse(uint64_t origRequest, ATQuoteDbResponseType responseType, LPATQUOTEDB_RESPONSE pResponse, uint32_t responseCount);

	virtual void		OnATMarketMoversStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATMARKET_MOVERS_STREAM_RESPONSE pResponse);
	virtual void		OnATQuoteStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATQUOTESTREAM_RESPONSE pResponse, uint32_t responseCount);

	virtual void		OnATSectorListResponse(uint64_t origRequest, LPATSECTORLIST_RECORD pRecords, uint32_t recordsCount);
	virtual void		OnATConstituentListResponse(uint64_t origRequest, LPATSYMBOL pSymbols, uint32_t symbolsCount);

	virtual void		OnATRequestTimeout(uint64_t origRequest);
};

