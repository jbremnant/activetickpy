#pragma once
#include <Shared/ATServerAPIDefines.h>
#include <string>
#include <vector>

class Helper
{
public:
	// Convert string input into ATMTIME
	static ATTIME StringToATTime(const std::string& datetime);
	static ATTIME StringToATTime(const std::string& datetime, bool startofday);
	static uint32_t ATTimeToDays(const ATTIME& dt);
	static uint32_t ATTimeToSeconds(const ATTIME& dt);
	static uint32_t ATTimeToMilliSeconds(const ATTIME& dt);
	static std::string ATTimeToString(const ATTIME& dt);

	// Convert string input into ATSYMBOL
	static ATSYMBOL StringToSymbol(const std::string& symbol);

	static std::vector<ATSYMBOL> StringToSymbols(const std::string& symbols);

	// Convert string to ATGuid
	static ATGUID StringToATGuid(const std::string& guid);

	// Convert char* to wchar_t*
	static bool ConvertString(const char* src, wchar16_t* dest, uint32_t destcount);
	static std::vector<wchar16_t> ConvertString(const std::string& src);
	static std::string ConvertString(const wchar16_t* src, uint32_t count);

	static void CopyString(wchar16_t* dest, const wchar16_t* src);
	static void	CopyString(wchar16_t* dest, const wchar16_t* src, uint32_t count);
	static uint32_t StringLength(const wchar16_t* src);
};
