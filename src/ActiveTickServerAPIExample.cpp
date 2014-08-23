#include "StdAfx.h"
#include "Helper.h"
#include "Requestor.h"
#include "Session.h"
#include "Streamer.h"
#include <ActiveTickServerAPI/ActiveTickServerAPI.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

void PrintUsage()
{
	cout << "Available commands:" << endl;
	cout << "-------------------------------------------------------------" << endl;
	cout << "?" << endl;
	cout << "quit" << endl;

	cout << "init [serverIpAddress] [serverPort] [apiUseridGuid] [userid] [password]" << endl;

	cout << "getIntradayHistoryBars [symbol] [minutes] [beginTime] [endTime]" << endl;
	cout << "getDailyHistoryBars [symbol] [beginTime] [endTime]" << endl;
	cout << "getWeeklyHistoryBars [symbol] [beginTime] [endTime]" << endl;

	cout << "getTicks [symbol] [beginTime] [endTime]" << endl;
	cout << "getTicksForEntireDay [symbol] [date]" << endl;

	cout << "getMarketMovers [symbol] [exchange]" << endl;

	cout << "getQuoteDb [symbol]" << endl;

	cout << "getSectorList" << endl;
	cout << "getSectoryConstituentsList [sectorname] [industryname] (use _ for spaces)" << endl;

	cout << "getOptionChain [symbol]" << endl;

	cout << "subscribeMarketMovers [symbol] [exchange]" << endl;
	cout << "unsubscribeMarketMovers [symbol] [exchange]" << endl;

	cout << "subscribeQuoteStream [symbol1,symbol2...]" << endl;
	cout << "unsubscribeQuoteStream [symbol,symbol2...]" << endl;

	cout << "subscribeTradesOnlyQuoteStream [symbol,symbol2...]" << endl;
	cout << "unsubscribeTradesOnlyQuoteStream [symbol,symbol2...]" << endl;

	cout << "subscribeQuotesOnlyQuoteStream [symbol,symbol2...]" << endl;
	cout << "unsubscribeQuotesOnlyQuoteStream [symbol,symbol2...]" << endl;

	cout << "-------------------------------------------------------------" << endl;
	cout << "Note: all date/time parameters must be entered in the following format: YYYYMMDDHHMMSS" << endl;
	cout << "apiUseridGuid must be entered using full 32 characters, ex: EF1C0A768BBB11DFBCB3F923E0D72085" << endl;
	cout << "-------------------------------------------------------------" << endl;
	cout.flush();
}

int main()
{
	ATInitAPI();

	{
		APISession session;
		Requestor requestor(session);
		Streamer streamer(session);

		PrintUsage();

		string cmd;
		while(true)
		{
			cin >> cmd;
			if(cmd == "?")
				PrintUsage();
			else if(cmd == "quit")
			{
				session.Destroy();
				break;
			}
			else if(cmd == "init")
			{
				string serverIpAddress, apiUserid, userid, password;
				uint32_t serverPort = 0;
				cin >> serverIpAddress >> serverPort >> apiUserid >> userid >> password;
				
                ATGUID guidApiUserid = Helper::StringToATGuid(apiUserid);
				bool rc = session.Init(guidApiUserid, serverIpAddress, serverPort, &Helper::ConvertString(userid).front(), &Helper::ConvertString(password).front());
				printf("init status: %d\n", rc);
			}
			else if(cmd == "getIntradayHistoryBars")
			{
				string symbol;
				string beginTime, endTime;
				uint32_t minutes = 0;
				cin >> symbol >> minutes >> beginTime >> endTime;

				ATTIME atBeginTime = Helper::StringToATTime(beginTime);
				ATTIME atEndTime = Helper::StringToATTime(endTime);

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
				uint64_t request = requestor.SendATBarHistoryDbRequest(atSymbol, BarHistoryIntraday, (uint8_t)minutes, atBeginTime, atEndTime, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Intraday bar history request [%s]\n", request, symbol.c_str());
			}
			else if(cmd == "getDailyHistoryBars" || cmd == "getWeeklyHistoryBars")
			{
				string symbol;
				string beginTime, endTime;
				cin >> symbol >> beginTime >> endTime;

				ATTIME atBeginTime = Helper::StringToATTime(beginTime);
				ATTIME atEndTime = Helper::StringToATTime(endTime);

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
				uint64_t request = requestor.SendATBarHistoryDbRequest(atSymbol, cmd == "getDailyHistoryBars" ? BarHistoryDaily : BarHistoryWeekly,
					0, atBeginTime, atEndTime, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): %s bar history request [%s]\n", request, cmd == "getDailyHistoryBars" ? "Daily" : "Weekly", symbol.c_str());
			}
			else if(cmd == "getTicks")
			{
				string symbol;
				string beginTime, endTime;
				cin >> symbol >> beginTime >> endTime;

				ATTIME atBeginTime = Helper::StringToATTime(beginTime);
				ATTIME atEndTime = Helper::StringToATTime(endTime);

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
				uint64_t request = requestor.SendATTickHistoryDbRequest(atSymbol, true, true, atBeginTime, atEndTime, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Tick history request [%s]\n", request, symbol.c_str());
			}
			else if(cmd == "getTicksForEntireDay")
			{
				string symbol;
				string dbdate;
				cin >> symbol >> dbdate;

				if(dbdate.length() < 8)
				{
					std::cout << "Invalid date\n";
					continue;
				}

				ATTIME atDbDate = {0};
				atDbDate.year = atol(dbdate.substr(0, 4).c_str());
				atDbDate.month = atol(dbdate.substr(4, 2).c_str());
				atDbDate.day = atol(dbdate.substr(6, 2).c_str());

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);

				//get 10 pages, and start from very end (-1)
				uint64_t request = requestor.SendATTickHistoryDbRequest(atSymbol, true, true, 10, -1, atDbDate, DEFAULT_REQUEST_TIMEOUT);
			}
			else if(cmd == "getMarketMovers")
			{
				string symbol;
				uint8_t exchange = 0;
				cin >> symbol >> exchange;

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
				atSymbol.symbolType = SymbolTopMarketMovers;
				atSymbol.exchangeType = exchange;

				uint64_t request = requestor.SendATMarketMoversDbRequest(&atSymbol, 1, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Market movers request [%s]\n", request, symbol.c_str());
			}
			else if(cmd == "getQuoteDb")
			{
				string symbol;
				cin >> symbol;

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);

				ATQuoteFieldType fields[] = { QuoteFieldLastPrice, QuoteFieldVolume, QuoteFieldProfileShortName, QuoteFieldBalanceSheetsShareDataTotalCommonSharesOutstanding, QuoteFieldProfileIndustry };
				uint64_t request = requestor.SendATQuoteDbRequest(&atSymbol, 1, fields, _countof(fields), DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): QuoteDb request [%s]\n", request, symbol.c_str());
			}
			else if(cmd == "getOptionChain")
			{
				string asciisymbol;
				cin >> asciisymbol;

				wchar16_t symbol[ATSymbolMaxLength];
				Helper::ConvertString(asciisymbol.c_str(), symbol, ATSymbolMaxLength);

				uint64_t request = requestor.SendATConstituentListRequest(ATConstituentListOptionChain, symbol, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): OptionChain request [%s]\n", request, asciisymbol.c_str());
			}
			else if(cmd == "getSectorList")
			{
				uint64_t request = requestor.SendATSectorListRequest(DEFAULT_REQUEST_TIMEOUT);
				printf("SEND (%llu): SectorList request\n", request);
			}
			else if(cmd == "getSectoryConstituentsList")
			{
				string sector, industry;
				cin >> sector >> industry;
				
				string asciikey = sector + "\n" + industry;

				for(string::size_type i = 0; i < asciikey.length(); ++i)
				{
					if(asciikey[i] == '_')
						asciikey[i] = ' ';
				}

				wchar16_t key[ATSymbolMaxLength];
				Helper::ConvertString(asciikey.c_str(), key, ATSymbolMaxLength);

				uint64_t request = requestor.SendATConstituentListRequest(ATConstituentListSector, key, DEFAULT_REQUEST_TIMEOUT);
				printf("SEND (%llu): ConstituentList request\n", request);
			}
			else if(cmd == "subscribeMarketMovers" || cmd == "unsubscribeMarketMovers")
			{
				uint8_t exchange = 0;
				string symbol;
				cin >> symbol >> exchange;

				ATSYMBOL atSymbol = Helper::StringToSymbol(symbol);
				atSymbol.symbolType = SymbolTopMarketMovers;
				atSymbol.exchangeType = exchange;

				ATStreamRequestType requestType = (cmd == "subscribeMarketMovers" ? StreamRequestSubscribe : StreamRequestUnsubscribe);
				uint64_t request = streamer.SendATMarketMoversStreamRequest(atSymbol, requestType, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Market movers stream request [%s]\n", request, symbol.c_str());
			}
			else if(cmd == "subscribeQuoteStream" || cmd == "unsubscribeQuoteStream")
			{
				string s;
				cin >> s;
				std::vector<ATSYMBOL> symbols = Helper::StringToSymbols(s);

				ATStreamRequestType requestType = (cmd == "subscribeQuoteStream" ? StreamRequestSubscribe : StreamRequestUnsubscribe);
				uint64_t request = streamer.SendATQuoteStreamRequest(symbols.data(), (uint16_t)symbols.size(), requestType, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Quote stream request [%s]\n", request, s.c_str());
			}
			else if(cmd == "subscribeTradesOnlyQuoteStream" || cmd == "unsubscribeTradesOnlyQuoteStream")
			{
				string s;
				cin >> s;
				std::vector<ATSYMBOL> symbols = Helper::StringToSymbols(s);

				ATStreamRequestType requestType = (cmd == "subscribeTradesOnlyQuoteStream" ? StreamRequestSubscribeTradesOnly : StreamRequestUnsubscribeTradesOnly);
				uint64_t request = streamer.SendATQuoteStreamRequest(symbols.data(), (uint16_t)symbols.size(), requestType, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Quote stream (trades only) request [%s]\n", request, s.c_str());
			}
			else if(cmd == "subscribeQuotesOnlyQuoteStream" || cmd == "unsubscribeQuotesOnlyQuoteStream")
			{
				string s;
				cin >> s;
				std::vector<ATSYMBOL> symbols = Helper::StringToSymbols(s);

				ATStreamRequestType requestType = (cmd == "subscribeQuotesOnlyQuoteStream" ? StreamRequestSubscribeQuotesOnly : StreamRequestUnsubscribeQuotesOnly);
				uint64_t request = streamer.SendATQuoteStreamRequest(symbols.data(), (uint16_t)symbols.size(), requestType, DEFAULT_REQUEST_TIMEOUT);

				printf("SEND (%llu): Quote stream (quotes only) request [%s]\n", request, s.c_str());
			}
		}
	}

	ATShutdownAPI();

	return 0;
}
