#include "StdAfx.h"
#include "Helper.h"
#include "Requestor.h"
#include <ATBarHistoryDbResponseParser.h>
#include <ATMarketMoversDbResponseParser.h>
#include <ATQuoteDbResponseParser.h>
#include <ATQuoteStreamResponseParser.h>
#include <ATTickHistoryDbResponseParser.h>

#include <fstream>
#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

Requestor::Requestor(const Session& session) :
  ActiveTickServerRequestor(session.getSessionHandle()),
  m_batch_size(1000000),
  m_tot_requests(0)
{
  m_tick_cb = NULL;
}

Requestor::~Requestor(void)
{
  CloseAllATRequests(); // just in case
}


// Wrapper for send request function for ticks
uint64_t Requestor::MakeTickRequest(const std::string& symbol, const std::string& sdate, const std::string& edate)
{
  const ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
  const ATTIME atBeginTime = Helper::StringToATTime(sdate, true);
  const ATTIME atEndTime = Helper::StringToATTime(edate, false);
  RequestItem req = {atSymbol, atBeginTime, atEndTime, 0};

  // parent class ActiveTickServerRequestor has this function implemented
  uint64_t request = SendATTickHistoryDbRequest(atSymbol, true, true, atBeginTime, atEndTime, DEFAULT_REQUEST_TIMEOUT);

  // as long as requests exist in this map, the request is still valid
  m_requests.insert(RequestPair(request, req));

  printf("inserted request[%d] for symbol: %s, dates %s - %s\n", request, symbol.c_str(),
      Helper::ATTimeToString(atBeginTime).c_str(), Helper::ATTimeToString(atEndTime).c_str());

  return request;
}



/*virtual*/ void Requestor::OnATSectorListResponse(uint64_t origRequest, LPATSECTORLIST_RECORD pRecords, uint32_t recordsCount)
{
	printf("RECV (%llu): Sector list response\n--------------------------------------------------------------\n", (uint64_t)origRequest);
	
	for(uint32_t i = 0; i < recordsCount; ++i)
	{
		printf("[%d/%d] %s / %s\n", i + 1, recordsCount, 
			Helper::ConvertString(pRecords[i].sector, ATSectorNameMaxLength).c_str(),
			Helper::ConvertString(pRecords[i].industry, ATIndustryNameMaxLength).c_str());
	}

	printf("--------------------------------------------------------------\nTotal records:%u\n", recordsCount);
}

/*virtual*/ void Requestor::OnATConstituentListResponse(uint64_t origRequest, LPATSYMBOL pSymbols, uint32_t symbolsCount)
{
	printf("RECV (%llu): Constituents list response\n--------------------------------------------------------------\n", (uint64_t)origRequest);
	
	for(uint32_t i = 0; i < symbolsCount; ++i)
		printf("[%d/%d] %s\n", i + 1, symbolsCount, Helper::ConvertString(pSymbols[i].symbol, _countof(pSymbols[i].symbol)).c_str());

	printf("--------------------------------------------------------------\nTotal symbols:%u\n", symbolsCount);
}

/*virtual*/ void Requestor::OnATBarHistoryDbResponse(uint64_t origRequest, ATBarHistoryResponseType responseType, LPATBARHISTORY_RESPONSE pResponse)
{
	string strResponseType;
	switch(responseType)
	{
	case BarHistoryResponseSuccess: strResponseType = "BarHistoryResponseSuccess"; break;
	case BarHistoryResponseInvalidRequest: strResponseType = "BarHistoryResponseInvalidRequest"; break;
	case BarHistoryResponseMaxLimitReached: strResponseType = "BarHistoryResponseMaxLimitReached"; break;
	case BarHistoryResponseDenied: strResponseType = "BarHistoryResponseDenied"; break;
	default: break;
	}

	printf("RECV (%llu): Bar history response [%s]\n--------------------------------------------------------------\n", (uint64_t)origRequest, strResponseType.c_str());

	uint32_t index = 1;
	ATBarHistoryDbResponseParser parser(pResponse);
	if(parser.MoveToFirstRecord())
	{
		while(true)
		{
			ATTIME recordDateTime = parser.GetDateTime();

			printf("[%d/%d] [%0.2d/%0.2d/%0.4d %0.2d:%0.2d:%0.2d] [o:%0.*f h:%0.*f l:%0.*f c:%0.*f vol:%llu]\n",
				index++, parser.GetRecordCount(),
				recordDateTime.month, recordDateTime.day, recordDateTime.year, recordDateTime.hour, recordDateTime.minute, recordDateTime.second,
				parser.GetOpen().precision, parser.GetOpen().price,
				parser.GetHigh().precision, parser.GetHigh().price,
				parser.GetLow().precision, parser.GetLow().price,
				parser.GetClose().precision, parser.GetClose().price,
				parser.GetVolume());

			if(parser.MoveToNextRecord() == false)
				break;
		}
	}

	printf("--------------------------------------------------------------\nTotal records:%d\n", parser.GetRecordCount());
}

/*virtual*/ void Requestor::OnATTickHistoryDbResponse(uint64_t origRequest, ATTickHistoryResponseType responseType, LPATTICKHISTORY_RESPONSE pResponse)
{
	char buffer[1000];
	int bufferLen = 0;

	string strResponseType;
	switch(responseType)
	{
	  case TickHistoryResponseSuccess: strResponseType = "TickHistoryResponseSuccess"; break;
	  case TickHistoryResponseInvalidRequest: strResponseType = "TickHistoryResponseInvalidRequest"; break;
	  case TickHistoryResponseMaxLimitReached: strResponseType = "TickHistoryResponseMaxLimitReached"; break;
	  case TickHistoryResponseDenied: strResponseType = "TickHistoryResponseDenied"; break;
	  default: break;
	}

  // look up the currently outstanding request
  Requests::iterator it;
  it = m_requests.find(origRequest);
  if( it == m_requests.end() )
  {
    printf("how did this callback get called??\n"); 
    return;
  }

  ATSYMBOL reqSym = it->second.symbol; 
  string rsymbol = Helper::ConvertString(reqSym.symbol, ATSymbolMaxLength);
  ATTIME sdt = it->second.starttime;
  ATTIME edt = it->second.endtime;
  int numTot = it->second.retrievedRecords;

	string symbol = Helper::ConvertString(pResponse->symbol.symbol, ATSymbolMaxLength);

  Ticks::iterator datait = m_ticks.find(symbol);
  if(datait == m_ticks.end()) // create on the fly
  {
    DataVec vec;
    m_ticks.insert( std::pair<std::string, DataVec>(symbol, vec) );
    datait = m_ticks.find(symbol);
  }

	uint32_t index = 1;
	ATTickHistoryDBResponseParser parser(pResponse);
  int numCur = parser.GetRecordCount();

	printf("RECV (%llu): Tick history response [%s] %s, %d records, %d cumrecords\n",
    (uint64_t)origRequest, strResponseType.c_str(), symbol.c_str(), numCur, numTot+numCur);

  if(numTot == 0 && numCur == 0)
  {
    printf("WARN: no data on first request %s : %s - %s\n",
      symbol.c_str(), Helper::ATTimeToString(sdt).c_str(), Helper::ATTimeToString(edt).c_str());
    // remove the record
    m_requests.erase(it);
    return;
  }

  // Cumulate the count of records we've received so far, but only for the same symbol
  // The requests and their association of the symbol are undifferentiated in this routine.
  // Thus, you need to keep track of the total records retrieved for each name.
  if(symbol == rsymbol)
    it->second.retrievedRecords += numCur;

  ATTIME rdt;
	if(parser.MoveToFirstRecord())
	{
		while(true)
		{
			rdt = parser.GetRecordDateTime();
      DataTick row;
			
      row.type = parser.GetRecordType();
      row.datetime = rdt;

			switch(parser.GetRecordType())
			{
			  case TickHistoryRecordTrade:
				  bufferLen = sprintf(buffer, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d,TRADE,last=%0.*f,size=%u,exchange=%u,condition=%d\r\n",					
					rdt.year, rdt.month, rdt.day, rdt.hour, rdt.minute, rdt.second,
					parser.GetTradeLastPrice().precision, parser.GetTradeLastPrice().price,
					parser.GetTradeLastSize(), parser.GetTradeLastExchange(), parser.GetTradeCondition(0));
          
          row.tp = parser.GetTradeLastPrice();
          row.ts = parser.GetTradeLastSize();
          row.texch = parser.GetTradeLastExchange();
          for(int i=0; i<4; i++) { row.tcond[i] = parser.GetTradeCondition(i); }
				break;
			case TickHistoryRecordQuote:
				  bufferLen = sprintf(buffer, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d,QUOTE,bid=%0.*f,ask=%0.*f,bidsize=%d,asksize=%d,bidexch=%d,askexch=%d,cond=%d\r\n",
					rdt.year, rdt.month, rdt.day, rdt.hour, rdt.minute, rdt.second,
					parser.GetQuoteBidPrice().precision, parser.GetQuoteBidPrice().price,
					parser.GetQuoteAskPrice().precision, parser.GetQuoteAskPrice().price,
					parser.GetQuoteBidSize(), parser.GetQuoteAskSize(),
					parser.GetQuoteBidExchange(), parser.GetQuoteAskExchange(),
					parser.GetQuoteCondition());

          row.bp = parser.GetQuoteBidPrice();
          row.ap = parser.GetQuoteAskPrice();
          row.bs = parser.GetQuoteBidSize();
          row.as = parser.GetQuoteAskSize();
          row.bexch = parser.GetQuoteBidExchange();
          row.aexch = parser.GetQuoteAskExchange();
          row.qcond = parser.GetQuoteCondition();
				break;
            default: 
				  bufferLen = 0;
				break;
			}

			// printf(buffer);

      datait->second.push_back(row); 

      if(datait->second.size() >= m_batch_size)
      {
        if(m_tick_cb != NULL)
          m_tick_cb(responseType, symbol, datait->second); // callback with the data collected
        // discard the tick data buffer
        datait->second.clear();  
      }

			if(parser.MoveToNextRecord() == false)
				break;
		}
	}

  int numTotNew = it->second.retrievedRecords;

  // if current records are zero, something happened, so we remove
  if( numCur == 0 && numTot > 0)
  {
    printf("%s end of records reached. Total: %d\n", symbol.c_str(), numTotNew);

    if(!datait->second.empty())
    {
      if(m_tick_cb != NULL)
        m_tick_cb(responseType, symbol, datait->second); // callback with the data collected

      datait->second.clear();  
    }
    m_requests.erase(it); // we are done with the request
    return;
  }

  if( Helper::ATTimeToSeconds(rdt) < Helper::ATTimeToSeconds(edt) )
  {
    ATSYMBOL atSymbol = pResponse->symbol;
    ATTIME atBeginTime = rdt; // the current time
    ATTIME atEndTime = edt;
    atBeginTime.milliseconds += 1; // NOTE: possible dropping of data if duplicates happen within the same millisec on 20k boundary
    RequestItem req = {atSymbol, atBeginTime, atEndTime, numTotNew};

    printf("sending request for %s : %s - %s\n", symbol.c_str(), Helper::ATTimeToString(atBeginTime).c_str(), Helper::ATTimeToString(atEndTime).c_str());
    // parent class ActiveTickServerRequestor has this function implemented
    uint64_t request = SendATTickHistoryDbRequest(atSymbol, true, true, atBeginTime, atEndTime, DEFAULT_REQUEST_TIMEOUT);
    // as long as requests exist in this map, the request is still valid
    m_requests.insert(RequestPair(request, req));
    m_requests.erase(it); // delete the old one after we create the new one
    return;
  }

  m_requests.erase(it); // request processed
	// printf("--------------------------------------------------------------\nTotal records:%d\n", parser.GetRecordCount());

	//if offset is valid, then try retrieving next 10 pages
	// if(pResponse->nextOffset != 0xffffffffffffffffULL && pResponse->nextOffset != 0xfffffffffffffff0ULL && pResponse->nextOffset != 0)
	// 	SendATTickHistoryDbRequest(pResponse->symbol, true, true, 10, pResponse->nextOffset, pResponse->offsetDatabaseDate, DEFAULT_REQUEST_TIMEOUT);
}

/*virtual*/ void Requestor::OnATMarketMoversDbResponse(uint64_t origRequest, ATMarketMoversDbResponseType responseType, LPATMARKET_MOVERSDB_RESPONSE pResponse)
{
	string strResponseType;
	switch(responseType)
	{
	case MarketMoversDbResponseSuccess: strResponseType = "MarketMoversDbResponseSuccess"; break;
	case MarketMoversDbResponseInvalidRequest: strResponseType = "MarketMoversDbResponseInvalidRequest"; break;
	case MarketMoversDbResponseDenied: strResponseType = "MarketMoversDbResponseDenied"; break;
	default: break;
	}

	printf("RECV (%llu): Market movers response [%s]\n--------------------------------------------------------------\n", (uint64_t)origRequest, strResponseType.c_str());

	ATMarketMoversDbResponseParser parser(pResponse);

	if(parser.MoveToFirstRecord() == true)
	{
		while(true)
		{
			printf("Market movers symbol: %s\n------------------\n", Helper::ConvertString(parser.GetRecordSymbol()->symbol, _countof(parser.GetRecordSymbol()->symbol)).c_str());

			if(parser.MoveToFirstItem() == true)
			{
				while(true)
				{
					printf("symbol:%s last:%0.*f volume:%llu\n",
						Helper::ConvertString(parser.GetItemSymbol()->symbol, _countof(parser.GetItemSymbol()->symbol)).c_str(),
						parser.GetItemLastPrice().precision, parser.GetItemLastPrice().price,
						parser.GetItemVolume());

					if(parser.MoveToNextItem() == false)
						break;
				}

				if(parser.MoveToNextRecord() == false)
					break;
			}
		}
	}
}

/*virtual*/ void Requestor::OnATQuoteDbResponse(uint64_t origRequest, ATQuoteDbResponseType responseType, LPATQUOTEDB_RESPONSE pResponse, uint32_t responseCount)
{
	string strResponseType;
	switch(responseType)
	{
	case QuoteDbResponseSuccess: strResponseType = "QuoteDbResponseSuccess"; break;
	case QuoteDbResponseInvalidRequest: strResponseType = "QuoteDbResponseInvalidRequest"; break;
	case QuoteDbResponseDenied: strResponseType = "QuoteDbResponseDenied"; break;
	default: break;
	}

	printf("RECV (%llu): QuoteDb response [%s]\n--------------------------------------------------------------\n", (uint64_t)origRequest, strResponseType.c_str());

	ATQuoteDbResponseParser parser(pResponse, responseCount);

	if(parser.MoveToFirstResponse() == true)
	{
		while(true)
		{
			string symbolStatus;
			switch(parser.GetSymbolStatus())
			{
			case SymbolStatusSuccess: symbolStatus = "SymbolStatusSuccess"; break;
			case SymbolStatusInvalid: symbolStatus = "SymbolStatusInvalid"; break;
			case SymbolStatusUnavailable: symbolStatus = "SymbolStatusUnavailable"; break;
			case SymbolStatusNoPermission: symbolStatus = "SymbolStatusNoPermission"; break;
			default: break;
			}

			printf("Symbol:%s [status:%s]\n-------------------------\n", Helper::ConvertString(parser.GetSymbol()->symbol, _countof(parser.GetSymbol()->symbol)).c_str(), symbolStatus.c_str());

			if(parser.GetSymbolStatus() == SymbolStatusSuccess && parser.MoveToFirstDataItem() == true)
			{
				while(true)
				{
					char data[512] = {0};

					switch(parser.GetDataItemDataType())
					{
					case DataByte:
						sprintf(data, "%c", *(char*)parser.GetDataItemData());
						break;
					case DataByteArray:
						sprintf(data, "byte data");
						break;
					case DataUInteger32:
						sprintf(data, "%u", *(uint32_t*)parser.GetDataItemData());
						break;
					case DataUInteger64:
						sprintf(data, "%llu", *(uint64_t*)parser.GetDataItemData());
						break;
					case DataInteger32:
						sprintf(data, "%d", *(int32_t*)parser.GetDataItemData());
						break;
					case DataInteger64:
						sprintf(data, "%lld", *(int64_t*)parser.GetDataItemData());
						break;
					case DataPrice:
						{
							ATPRICE price = *(LPATPRICE)parser.GetDataItemData();
							sprintf(data, "%0.*f", price.precision, price.price);
						}
						break;
					case DataString:
						{
							char* pString = (char*)parser.GetDataItemData();
							strncpy(data, pString, sizeof(data) - 1);
						}
						break;
					case DataUnicodeString:
						{
							wchar16_t* pString = (wchar16_t*)parser.GetDataItemData();
							std::string s = Helper::ConvertString(pString, Helper::StringLength(pString));
							strncpy(data, s.c_str(), sizeof(data));
						}
						break;
					case DataDateTime:
						{
							LPATTIME pst = (LPATTIME)parser.GetDataItemData();
							sprintf(data, "%0.2d/%0.2d/%0.4d %0.2d:%0.2d:%0.2d",
								pst->month, pst->day, pst->year, pst->hour, pst->minute, pst->second);
						}
						break;
                    default: break;
					}

					printf("\tATQuoteFieldType:%d\n\tATFieldStatus:%d\n\tATDataType:%d\n\tData:%s\n",
						parser.GetDataItemQuoteFieldType(),
						parser.GetDataItemFieldStatus(),
						parser.GetDataItemDataType(),
						data);

					if(parser.MoveToNextDataItem() == false)
						break;
				}
			}

			if(parser.MoveToNextResponse() == false)
				break;
		}
	}
}

/*virtual*/ void Requestor::OnATMarketMoversStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATMARKET_MOVERS_STREAM_RESPONSE pResponse)
{
	string strResponseType;
	switch(responseType)
	{
	case StreamResponseSuccess: strResponseType = "StreamResponseSuccess"; break;
	case StreamResponseInvalidRequest: strResponseType = "StreamResponseInvalidRequest"; break;
	case StreamResponseDenied: strResponseType = "StreamResponseDenied"; break;
	default: break;
	}

	printf("RECV (%llu): Market movers stream response [%s]\n--------------------------------------------------------------\n", (uint64_t)origRequest, strResponseType.c_str());
}

/*virtual*/ void Requestor::OnATQuoteStreamResponse(uint64_t origRequest, ATStreamResponseType responseType, LPATQUOTESTREAM_RESPONSE pResponse, uint32_t responseCount)
{
	string strResponseType;
	switch(responseType)
	{
	case StreamResponseSuccess: strResponseType = "StreamResponseSuccess"; break;
	case StreamResponseInvalidRequest: strResponseType = "StreamResponseInvalidRequest"; break;
	case StreamResponseDenied: strResponseType = "StreamResponseDenied"; break;
	default: break;
	}

	printf("RECV (%llu): Quote stream response [%s]\n--------------------------------------------------------------\n", (uint64_t)origRequest, strResponseType.c_str());

	if(responseType == StreamResponseSuccess)
	{
		ATQuoteStreamResponseParser parser(pResponse);
		parser.MoveToBeginning();

		if(parser.MoveToFirstDataItem() == true)
		{
			while(true)
			{
				string symbolStatus;
				switch(parser.GetSymbolStatus())
				{
				case SymbolStatusSuccess: symbolStatus = "SymbolStatusSuccess"; break;
				case SymbolStatusInvalid: symbolStatus = "SymbolStatusInvalid"; break;
				case SymbolStatusUnavailable: symbolStatus = "SymbolStatusUnavailable"; break;
				case SymbolStatusNoPermission: symbolStatus = "SymbolStatusNoPermission"; break;
				default: break;
				}

				printf("\tsymbol:%s symbolStatus:%s\n", Helper::ConvertString(parser.GetSymbol()->symbol, _countof(parser.GetSymbol()->symbol)).c_str(), symbolStatus.c_str());

				if(parser.MoveToNextDataItem() == false)
					break;
			}
		}
	}
}

/*virtual*/ void Requestor::OnATRequestTimeout(uint64_t origRequest)
{
	printf("(%llu): Request timed-out\n", (uint64_t)origRequest);
}
