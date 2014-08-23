#pragma once

#include <Shared/ATServerAPIDefines.h>

/**
* ATTickHistoryDBResponseParser class.
* This class is used for parsing the history database response initiated by ATCreateTickHistoryDbRequest.
* @see ATCreateTickHistoryDbRequest
*/
class ACTIVETICKSERVERAPI_API ATTickHistoryDBResponseParser
{
public:
	/**
	* ATTickHistoryDBResponseParser constructor.
	* @param pResponse Pointer to ATTICKHISTORY_RESPONSE received from the API callback.
	*/
	ATTickHistoryDBResponseParser(LPATTICKHISTORY_RESPONSE pResponse);

	/**
	* ATTickHistoryDBResponseParser destructor.
	*/
	~ATTickHistoryDBResponseParser(void);

public:
	/**
	* Determine validity of the current object state.
	* @return TRUE if object is valid, FALSE otherwise.
	*/
	bool						IsValid() const;

	/**
	* If request was made with offset parameter, this member function returns next offset value
	* @return uint64_t value.
	*/
	uint64_t					GetNextOffset() const;

	ATTIME						GetOffsetDbDate() const;

	/**
	* Move internal parser pointer to first tick record.
	* @return TRUE if move was successful, FALSE otherwise.
	*/
	bool						MoveToFirstRecord();
	
	/**
	* Move internal parser pointer to next tick record.
	* @return TRUE if move was successful, FALSE otherwise.
	*/
	bool						MoveToNextRecord();
	
	/**
	* Retrieve one of ATTickHistoryRecordType enum types for current record.
	* @return ATTickHistoryRecordType
	* @see ATTickHistoryRecordType
	*/
	ATTickHistoryRecordType		GetRecordType() const;
	
	/**
	* Retrieve date+time for current record.
	* @return Current record's date+time in ATTIME structure.
	*/
	ATTIME						GetRecordDateTime() const;

	/**
	* Retrieve one of ATExchangeType enum types for last traded exchange for current record.
	* @return ATExchangeType
	* @see ATExchangeType
	*/
	ATExchangeType				GetTradeLastExchange() const;

	/**
	* Retrieve last traded price for current record.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetTradeLastPrice() const;

	/**
	* Retrieve last traded size for current record.
	* @return UINT
	*/
	uint32_t					GetTradeLastSize() const;

	/**
	* Retrieve one of ATTradeConditionType enum types for last traded condition for current record.
	* @return ATTradeConditionType
	* @see ATTradeConditionType
	*/
	ATTradeConditionType		GetTradeCondition(uint32_t index) const;

	/**
	* Retrieve pointer to array of ATTradeConditionType enum types for last traded conditions for current record.
	* @note Maximum number of trading conditions is defined in ATTradeConditionsCount.
	* @return ATTradeConditionType
	* @see ATTradeConditionsCount
	* @see ATTradeConditionType
	*/
	ATTradeConditionType*		GetTradeConditions() const;

	/**
	* Retrieve one of ATExchangeType enum types for bid exchange for current record.
	* @return ATExchangeType
	* @see ATExchangeType
	*/
	ATExchangeType				GetQuoteBidExchange() const;

	/**
	* Retrieve one of ATExchangeType enum types for ask exchange for current record.
	* @return ATExchangeType
	* @see ATExchangeType
	*/
	ATExchangeType				GetQuoteAskExchange() const;
	
	/**
	* Retrieve bid price for current record.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetQuoteBidPrice() const;

	/**
	* Retrieve ask price for current record.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetQuoteAskPrice() const;

	/**
	* Retrieve bid size for current record.
	* @return UINT
	*/
	uint32_t					GetQuoteBidSize() const;

	/**
	* Retrieve ask size for current record.
	* @return UINT
	*/
	uint32_t					GetQuoteAskSize() const;

	/**
	* Retrieve one of ATQuoteConditionType enum types for quote condition for current record.
	* @return ATQuoteConditionType
	* @see ATQuoteConditionType
	*/
	ATQuoteConditionType		GetQuoteCondition() const;

	/**
	* Retrieve pointer to symbol.
	* @return ATSYMBOL
	* @see ATSYMBOL
	*/
	LPATSYMBOL					GetSymbol() const;

	/**
	* Retrieve one of ATSymbolStatus enum types for symbol status.
	* @return ATSymbolStatus
	* @see ATSymbolStatus
	*/
	ATSymbolStatus				GetSymbolStatus() const;	

	/**
	* Retrieve record count.
	* @return UINT
	*/
	uint32_t					GetRecordCount() const;

private:
	LPATTICKHISTORY_RESPONSE	m_pResponse;
	uint8_t*					m_pCurrPtr;
	uint32_t					m_nCurrRecord;
};
