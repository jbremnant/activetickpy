#pragma once

#include <Shared/ATServerAPIDefines.h>

/**
* ATQuoteStreamResponseParser class.
* This class is used for parsing the quote stream response initiated by ATCreateQuoteStreamRequest.
* @see ATCreateQuoteStreamRequest
*/
class ACTIVETICKSERVERAPI_API ATQuoteStreamResponseParser
{
public:
	/**
	* ATQuoteStreamResponseParser constructor.
	* @param pResponse Pointer to ATQUOTESTREAM_RESPONSE received from the API callback.
	*/
	ATQuoteStreamResponseParser(LPATQUOTESTREAM_RESPONSE pResponse);

	/**
	* ATQuoteStreamResponseParser destructor.
	*/
	virtual ~ATQuoteStreamResponseParser(void);

	/**
	* Move internal parser pointer to beginning of ATQUOTEDB_RESPONSE response.	
	* @see ATQUOTEDB_RESPONSE
	*/
	void						MoveToBeginning();

	/**
	* Move internal parser pointer to first data item within ATQUOTEDB_RESPONSE response.	
	* @return TRUE if pointer was moved successfully, FALSE otherwise.
	* @see ATQUOTEDB_RESPONSE
	*/
	bool						MoveToFirstDataItem();

	/**
	* Move internal parser pointer to next data item within ATQUOTEDB_RESPONSE response.	
	* @note This method can only be called after successful call to MoveToFirstDataItem.
	* @return TRUE if pointer was moved successfully, FALSE otherwise.
	* @see ATQUOTEDB_RESPONSE
	* @see MoveToFirstDataItem
	*/
	bool						MoveToNextDataItem();

	/**
	* Get one of ATStreamResponseType enum types for current response.
	* @return ATStreamRespomnseType for current response.
	* @note This method can only be called after successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @see ATStreamResponseType
	*/
	ATStreamResponseType		GetResponseType() const;

	/**
	* Get pointer to ATSYMBOL for current response.
	* @return ATSYMBOL for current response if object is in valid state, NULL otherwise.
	* @note This method can only be called after successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @see ATSYMBOL
	*/
	LPATSYMBOL					GetSymbol() const;

	/**
	* Get one of ATSymbolStatus enum types for current response.
	* @return ATSymbolStatus for current response.
	* @note This method can only be called after successful call to MoveToFirstDataItem or MoveToNextDataItem.
	* @see ATSymbolStatus
	*/
	ATSymbolStatus				GetSymbolStatus() const;	

private:
	const uint8_t*		m_pResponse;
	uint16_t					m_sCurrDataItemIndex;
};
