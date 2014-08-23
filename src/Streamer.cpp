#include "StdAfx.h"
#include "Helper.h"
#include "Streamer.h"
#include <string>
#include <stdio.h>

using namespace std;

Streamer::Streamer(const APISession& session)
: ActiveTickStreamListener(session.GetSessionHandle())
, Requestor(session)
, m_session(session)
{
}

Streamer::~Streamer(void)
{
}

/*virtual*/ void Streamer::OnATStreamTradeUpdate(LPATQUOTESTREAM_TRADE_UPDATE pUpdate)
{	
	printf("RECV: [%0.2d:%0.2d:%0.2d] STREAMTRADE [symbol:%s last:%0.*f lastSize:%d]\n",
		pUpdate->lastDateTime.hour, pUpdate->lastDateTime.minute, pUpdate->lastDateTime.second,
		Helper::ConvertString(pUpdate->symbol.symbol, _countof(pUpdate->symbol.symbol)).c_str(),
		pUpdate->lastPrice.precision, pUpdate->lastPrice.price,
		pUpdate->lastSize);

	//print out trade info, plus how many updates are currently queued up locally
	/*printf("RECV: [%0.2d:%0.2d:%0.2d] STREAMTRADE(%u) [symbol:%s last:%0.*f lastSize:%d]\n",
		pUpdate->lastDateTime.wHour, pUpdate->lastDateTime.wMinute, pUpdate->lastDateTime.wSecond,
		ATGetSessionQueuedStreamUpdates(m_session.GetSessionHandle()),
		Helper::ConvertString(pUpdate->symbol.symbol, _countof(pUpdate->symbol.symbol)).c_str(),
		pUpdate->lastPrice.precision, pUpdate->lastPrice.price,
		pUpdate->lastSize);*/
}

/*virtual*/ void Streamer::OnATStreamQuoteUpdate(LPATQUOTESTREAM_QUOTE_UPDATE pUpdate)
{
	printf("RECV: [%0.2d:%0.2d:%0.2d] STREAMQUOTE [symbol:%s bid:%0.*f ask:%0.*f bidSize:%d askSize:%d]\n",
		pUpdate->quoteDateTime.hour, pUpdate->quoteDateTime.minute, pUpdate->quoteDateTime.second,
		Helper::ConvertString(pUpdate->symbol.symbol, _countof(pUpdate->symbol.symbol)).c_str(),
		pUpdate->bidPrice.precision, pUpdate->bidPrice.price,
		pUpdate->askPrice.precision, pUpdate->askPrice.price,
		pUpdate->bidSize,
		pUpdate->askSize);

	//print out quote info, plus how many updates are currently queued up locally
	/*printf("RECV: [%0.2d:%0.2d:%0.2d] STREAMQUOTE(%u) [symbol:%s bid:%0.*f ask:%0.*f bidSize:%d askSize:%d]\n",
		pUpdate->quoteDateTime.wHour, pUpdate->quoteDateTime.wMinute, pUpdate->quoteDateTime.wSecond,
		ATGetSessionQueuedStreamUpdates(m_session.GetSessionHandle()),
		Helper::ConvertString(pUpdate->symbol.symbol, _countof(pUpdate->symbol.symbol)).c_str(),
		pUpdate->bidPrice.precision, pUpdate->bidPrice.price,
		pUpdate->askPrice.precision, pUpdate->askPrice.price,
		pUpdate->bidSize,
		pUpdate->askSize);*/
}

/*virtual*/ void Streamer::OnATStreamTopMarketMoversUpdate(LPATMARKET_MOVERS_STREAM_UPDATE pUpdate)
{
	printf("RECV: [%0.2d:%0.2d:%0.2d] STREAMMOVERS [symbol:%s\n",
		pUpdate->lastUpdateTime.hour, pUpdate->lastUpdateTime.minute, pUpdate->lastUpdateTime.second,
		Helper::ConvertString(pUpdate->marketMovers.symbol.symbol, _countof(pUpdate->marketMovers.symbol.symbol)).c_str());

	for(int32_t i = 0; i < ATMarketMoversMaxRecords; i++)
	{
		printf("\tsybmol:%s last:%0.*f\n",
			Helper::ConvertString(pUpdate->marketMovers.items[i].symbol.symbol, _countof(pUpdate->marketMovers.items[i].symbol.symbol)).c_str(),
			pUpdate->marketMovers.items[i].lastPrice.precision, pUpdate->marketMovers.items[i].lastPrice.price);
	}
}
