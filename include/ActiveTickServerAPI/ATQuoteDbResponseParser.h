#pragma once

#include <Shared/ATServerAPIDefines.h>

/**
* ATQuoteDbResponseParser class.
* This class is used for parsing the quote database response initiated by ATCreateQuoteDbRequest.
* @see ATCreateQuoteDbRequest
*/
class ACTIVETICKSERVERAPI_API ATQuoteDbResponseParser
{
public:
	/**
	* ATQuoteDbResponseParser constructor.
	* @param pResponse Pointer to ATQUOTEDB_RESPONSE received from the API callback.
	* @param responseCount Count of responses passed in pResponse parameter.
	*/
	ATQuoteDbResponseParser(LPATQUOTEDB_RESPONSE pResponse, uint32_t responseCount);

	/**
	* ATQuoteDbResponseParser destructor.
	*/
	virtual ~ATQuoteDbResponseParser(void);

public:
	/**
	* Determine validity of the current object state.
	* @return TRUE if object is valid, FALSE otherwise.
	*/
	bool				IsValid() const;

	/**
	* Move internal parser pointer to first ATQUOTEDB_RESPONSE response.	
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATQUOTEDB_RESPONSE
	*/
	bool				MoveToFirstResponse();

	/**
	* Move internal parser pointer to next ATQUOTEDB_RESPONSE response.
	* @note This method can be called only after a successful call to MoveToFirstResponse.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see ATQUOTEDB_RESPONSE
	* @see MoveToFirstResponse	
	*/
	bool				MoveToNextResponse();

	/**
	* Move internal parser pointer to first data item inside current response.
	* @note This method can be called only after a successful call to MoveToFirstResponse or MoveToNextResponse.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see MoveToFirstResponse
	* @see MoveToNextResponse
	*/
	bool				MoveToFirstDataItem();

	/**
	* Move internal parser pointer to next data item inside current response.
	* @note This method can be called only after a successful call to MoveToFirstDataItem.
	* @return TRUE if move was successful, FALSE otherwise.
	* @see MoveToFirstDataItem	
	*/
	bool				MoveToNextDataItem();

	/**
	* Get symbol for current response.
	* @note This method can be called only after a successful call to MoveToFirstResponse or MoveToNextResponse.
	* @return Pointer to ATSYMBOL if object is in valid state, or NULL otherwise.
	* @see ATSYMBOL
	* @see MoveToFirstResponse	
	* @see MoveToNextResponse
	*/
	LPATSYMBOL			GetSymbol() const;

	/**
	* Get one of the ATSymbolStatus enum types for current response.
	* @note This method can be called only after a successful call to MoveToFirstResponse or MoveToNextResponse.
	* @return ATSymbolStatus.
	* @see ATSymbolStatus
	* @see MoveToFirstResponse	
	* @see MoveToNextResponse
	*/
	ATSymbolStatus		GetSymbolStatus() const;
	
	/**
	* Get count of all symbols.
	* @return Count of symbols.
	*/
	uint16_t			GetSymbolCount() const;

	/**
	* Get count of data items for current response.
	* @note This method can be called only after a successful call to MoveToFirstResponse or MoveToNextResponse.
	* @return Count of data items.
	* @see MoveToFirstResponse	
	* @see MoveToNextResponse
	*/
	uint16_t			GetDataItemCount() const;

	/**
	* Get one of the ATQuoteFieldType enum types for current data item.
	* @note This method can be called only after a successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @return ATQuoteFieldType representing the type of the field.
	* @see ATQuoteFieldType	
	* @see MoveToFirstDataItem
	* @see MoveToNextDataItem
	*/
	ATQuoteFieldType	GetDataItemQuoteFieldType() const;

	/**
	* Get one of the ATFieldStatus enum types for current data item.
	* @note This method can be called only after a successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @return ATFieldStatus representing the type of the field.
	* @see ATFieldStatus	
	* @see MoveToFirstDataItem
	* @see MoveToNextDataItem
	*/
	ATFieldStatus		GetDataItemFieldStatus() const;

	/**
	* Get one of the ATDataType enum types for current data item.
	* @note This method can be called only after a successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @return ATDataType representing the type of the field.
	* @see ATDataType	
	* @see MoveToFirstDataItem
	* @see MoveToNextDataItem
	*/
	ATDataType			GetDataItemDataType() const;

	/**
	* Get data pointer to raw value of current data item.
	* @note This method can be called only after a successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @return Pointer to raw data if object is in valid state, NULL otherwise.
	* @see MoveToFirstDataItem
	* @see MoveToNextDataItem
	*/
	const void*			GetDataItemData() const;
	
private:
	uint8_t*		m_pResponse;
	uint32_t		m_sResponseCount;

	uint32_t		m_sCurrResponseIndex;
	uint32_t		m_sCurrDataItemIndex;
	uint8_t*		m_pCurrResponse;
	uint8_t*		m_pCurrDataItem;
};
