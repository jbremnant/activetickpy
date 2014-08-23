#pragma once

#include <Shared/ATServerAPIDefines.h>

/**
* ATBarHistoryDbResponseParser class.
* This class is used for parsing the history database response initiated by ATCreateBarHistoryDbRequest.
* @see ATCreateBarHistoryDbRequest
*/
class ACTIVETICKSERVERAPI_API ATBarHistoryDbResponseParser
{
public:
	/**
	* ATBarHistoryDbResponseParser constructor.
	* @param pResponse Pointer to ATBARHISTORY_RESPONSE received from the API callback.
	*/
	ATBarHistoryDbResponseParser(LPATBARHISTORY_RESPONSE pResponse);

	/**
	* ATBarHistoryDbResponseParser destructor.
	*/
	virtual ~ATBarHistoryDbResponseParser(void);

	/**
	* Determine validity of the current object state.
	* @return TRUE if object is valid, FALSE otherwise.
	*/
	bool						IsValid() const;

	/**
	* Move internal parser pointer to first bar record.
	* @return TRUE if move was successful, FALSE otherwise.
	*/
	bool						MoveToFirstRecord();

	/**
	* Move internal parser pointer to next bar record.
	* @return TRUE if move was successful, FALSE otherwise.
	*/
	bool						MoveToNextRecord();

	/**
	* Retrieve current bar's open price.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetOpen() const;

	/**
	* Retrieve current bar's high price.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetHigh() const;

	/**
	* Retrieve current bar's low price.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetLow() const;

	/**
	* Retrieve current bar's close price.
	* @return ATPRICE
	* @see ATPRICE
	*/
	ATPRICE						GetClose() const;

	/**
	* Retrieve current bar's volume.
	* @return ULONGLONG integer.
	*/
	uint64_t					GetVolume() const;

	/**
	* Retrieve current bar's date/time.
	* @return ATTIME
	*/
	ATTIME						GetDateTime() const;

	/**
	* Retrieve pointer to response's symbol.
	* @return LPATSYMBOL
	* @see ATSYMBOL
	*/
	LPATSYMBOL					GetSymbol() const;

	/**
	* Retrieve one of the ATSymbolStatus enum types.
	* @return ATSymbolStatus
	* @see ATSymbolStatus
	*/
	ATSymbolStatus				GetSymbolStatus() const;

	/**
	* Retrieve count of records within current response.
	* @return UINT Count of bar records.
	*/
	uint32_t					GetRecordCount() const;

private:
	LPATBARHISTORY_RESPONSE		m_pResponse;
	uint32_t					m_nCurrRecord;
};
