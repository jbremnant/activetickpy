#pragma once

#include <Shared/ATServerAPIDefines.h>

/**
* ATMarketMoversDbResponseParser class.
* This class is used for parsing the market movers snapshot database response initiated by ATCreateMarketMoversDbRequest.
* @see ATCreateMarketMoversDbRequest
*/
class ACTIVETICKSERVERAPI_API ATMarketMoversDbResponseParser
{
public:
	/**
	* ATMarketMoversDbResponseParser constructor.
	* @param pResponse Pointer to ATMARKET_MOVERSDB_RESPONSE received from the API callback.
	*/
	ATMarketMoversDbResponseParser(LPATMARKET_MOVERSDB_RESPONSE pResponse);

	/**
	* ATMarketMoversDbResponseParser destructor.
	*/
	virtual ~ATMarketMoversDbResponseParser(void);

public:
	/**
	* Determine validity of the current object state.
	* @return TRUE if object is valid, FALSE otherwise.
	*/
	bool						IsValid() const;

	/**
	* Move internal parser pointer to first ATMARKET_MOVERS_RECORD record.	
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATMARKET_MOVERS_RECORD
	*/
	bool						MoveToFirstRecord();

	/**
	* Move internal parser pointer to first ATMARKET_MOVERS_ITEM record.
	* @note This method can be called only after a successful call to MoveToFirstRecord or MoveToNextRecord.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATMARKET_MOVERS_ITEM
	* @see MoveToFirstRecord
	* @see MoveToNextRecord	
	*/
	bool						MoveToFirstItem();

	/**
	* Move internal parser pointer to next ATMARKET_MOVERS_RECORD record.
	* @note This method can be called only after a successful call to MoveToFirstItem.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATMARKET_MOVERS_RECORD
	* @see MoveToFirstItem	
	*/
	bool						MoveToNextRecord();

	/**
	* Move internal parser pointer to next ATMARKET_MOVERS_ITEM record.
	* @note This method can be called only after a successful call to MoveToFirstItem.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATMARKET_MOVERS_ITEM
	* @see MoveToFirstItem	
	*/
	bool						MoveToNextItem();

	/**
	* Retrieve pointer to current ATMARKET_MOVERS_RECORD record.
	* @return Valid pointer to the ATMARKET_MOVERS_RECORD structure if object is in a valid state, or NULL otherwise.
	* @see ATMARKET_MOVERS_RECORD	
	*/
	LPATMARKET_MOVERS_RECORD	GetCurrentRecord() const;

	/**
	* Retrieve pointer to current ATMARKET_MOVERS_ITEM record.	
	* @return Valid pointer to the ATMARKET_MOVERS_ITEM structure if object is in a valid state, or NULL otherwise.
	* @see ATMARKET_MOVERS_ITEM
	*/
	LPATMARKET_MOVERS_ITEM		GetCurrentItem() const;

	/**
	* Retrieve pointer to ATSYMBOL symbol for current record.	
	* @return Valid pointer to the ATSYMBOL structure if object is in a valid state, or NULL otherwise.
	* @see ATSYMBOL
	*/
	LPATSYMBOL					GetRecordSymbol() const;
	
	/**
	* Retrieve one of the ATSymbolStatus enum types for current record.	
	* @return Valid pointer to the ATSYMBOL structure if object is in a valid state, or NULL otherwise.
	* @see ATSymbolStatus
	*/
	ATSymbolStatus				GetRecordSymbolStatus() const;	

	/**
	* Retrieve record count.	
	* @return Record count.
	*/
	uint32_t					GetRecordCount() const;

	/**
	* Retrieve pointer to ATSYMBOL symbol for current item.	
	* @return Valid pointer to the ATSYMBOL structure if object is in a valid state, or NULL otherwise.
	* @see ATSYMBOL
	*/
	LPATSYMBOL					GetItemSymbol() const;

	/**
	* Retrieve last price for current item.
	* @return Last price for current item.
	* @see ATPRICE
	*/
	ATPRICE						GetItemLastPrice() const;

	/**
	* Retrieve previous close price for current item.
	* @return Previous close price for current item.
	* @see ATPRICE
	*/
	ATPRICE						GetItemClosePrice() const;

	/**
	* Retrieve volume for current item.
	* @return Volume for current item.
	*/
	uint64_t					GetItemVolume() const;

	/**
	* Retrieve date/time for current item.
	* @return Date/time for current item.
	*/
	ATTIME						GetItemLastDateTime() const;

private:
	LPATMARKET_MOVERSDB_RESPONSE	m_pResponse;
	uint32_t						m_nCurrRecord;
	uint32_t						m_nCurrItem;
};
