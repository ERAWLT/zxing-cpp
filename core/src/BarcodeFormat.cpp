/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
*/
// SPDX-License-Identifier: Apache-2.0

#include "BarcodeFormat.h"

#include "ZXAlgorithms.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <sstream>
#include <stdexcept>

namespace ZXing {

struct BarcodeFormatName
{
	BarcodeFormat format;
	std::string_view name;
};

/*
 * ZXING_CUSTOM - Reduced barcode format table for QR-only usage
 * Removed: Aztec, Codabar, Code39, Code93, Code128, DataBar, DataBarExpanded,
 *          DataMatrix, DXFilmEdge, EAN8, EAN13, ITF, MaxiCode, PDF417, UPCA, UPCE
 */
static BarcodeFormatName NAMES[] = {
	{BarcodeFormat::None, "None"},
	{BarcodeFormat::MicroQRCode, "MicroQRCode"},
	{BarcodeFormat::QRCode, "QRCode"},
	{BarcodeFormat::RMQRCode, "rMQRCode"},
	{BarcodeFormat::MatrixCodes, "Matrix-Codes"},
};

std::string ToString(BarcodeFormat format)
{
	auto i = FindIf(NAMES, [format](auto& v) { return v.format == format; });
	return i == std::end(NAMES) ? std::string() : std::string(i->name);
}

std::string ToString(BarcodeFormats formats)
{
	if (formats.empty())
		return ToString(BarcodeFormat::None);
	std::string res;
	for (auto f : formats)
		res += ToString(f) + "|";
	return res.substr(0, res.size() - 1);
}

static std::string NormalizeFormatString(std::string_view sv)
{
	std::string str(sv);
	std::transform(str.begin(), str.end(), str.begin(), [](char c) { return (char)std::tolower(c); });
	str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return Contains("_-[]", c); }), str.end());
	return str;
}

static BarcodeFormat ParseFormatString(const std::string& str)
{
	auto i = FindIf(NAMES, [str](auto& v) { return NormalizeFormatString(v.name) == str; });
	return i == std::end(NAMES) ? BarcodeFormat::None : i->format;
}

BarcodeFormat BarcodeFormatFromString(std::string_view str)
{
	return ParseFormatString(NormalizeFormatString(str));
}

BarcodeFormats BarcodeFormatsFromString(std::string_view str)
{
	auto normalized = NormalizeFormatString(str);
	std::replace_if(
		normalized.begin(), normalized.end(), [](char c) { return Contains(" ,", c); }, '|');
	std::istringstream input(normalized);
	BarcodeFormats res;
	for (std::string token; std::getline(input, token, '|');) {
		if(!token.empty()) {
			auto bc = ParseFormatString(token);
			/*
			 * ZXING_CUSTOM
			 *
			if (bc == BarcodeFormat::None)
				throw std::invalid_argument("This is not a valid barcode format: " + token);
			*/
			res |= bc;
		}
	}
	return res;
}

} // ZXing
