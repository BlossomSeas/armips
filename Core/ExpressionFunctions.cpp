#include "stdafx.h"
#include "ExpressionFunctions.h"
#include "Misc.h"
#include "Common.h"
#if ARMIPS_REGEXP
#include <regex>
#endif

bool getExpFuncParameter(const std::vector<ExpressionValue>& parameters, size_t index, int64_t& dest,
	const std::wstring& funcName, bool optional)
{
	if (optional && index >= parameters.size())
		return true;

	if (index >= parameters.size() || parameters[index].isInt() == false)
	{
		Logger::queueError(Logger::Error,L"Invalid parameter %d for %s: expecting integer",index+1,funcName);
		return false;
	}

	dest = parameters[index].intValue;
	return true;
}

bool getExpFuncParameter(const std::vector<ExpressionValue>& parameters, size_t index, const std::wstring*& dest,
	const std::wstring& funcName, bool optional)
{
	if (optional && index >= parameters.size())
		return true;

	if (index >= parameters.size() || parameters[index].isString() == false)
	{
		Logger::queueError(Logger::Error,L"Invalid parameter %d for %s: expecting string",index+1,funcName);
		return false;
	}

	dest = &parameters[index].strValue;
	return true;
}

#define GET_PARAM(params,index,dest) \
	if (getExpFuncParameter(params,index,dest,funcName,false) == false) \
		return ExpressionValue();
#define GET_OPTIONAL_PARAM(params,index,dest,defaultValue) \
	dest = defaultValue; \
	if (getExpFuncParameter(params,index,dest,funcName,true) == false) \
		return ExpressionValue();


ExpressionValue expFuncVersion(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	int64_t value = ARMIPS_VERSION_MAJOR*1000 + ARMIPS_VERSION_MINOR*10 + ARMIPS_VERSION_REVISION;
	return ExpressionValue(value);
}

ExpressionValue expFuncEndianness(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;
	result.type = ExpressionValueType::String;

	switch (g_fileManager->getEndianness())
	{
	case Endianness::Little:
		return ExpressionValue(L"little");
	case Endianness::Big:
		return ExpressionValue(L"big");
	}

	return ExpressionValue();
}

ExpressionValue expFuncOutputName(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	std::shared_ptr<AssemblerFile> file = g_fileManager->getOpenFile();
	if (file == nullptr)
	{
		Logger::queueError(Logger::Error,L"outputName: no file opened");
		return ExpressionValue();
	}

	std::wstring value = file->getFileName();
	return ExpressionValue(value);
}

ExpressionValue expFuncOrg(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	if(!g_fileManager->hasOpenFile())
	{
		Logger::queueError(Logger::Error,L"org: no file opened");
		return ExpressionValue();
	}
	return ExpressionValue(g_fileManager->getVirtualAddress());
}

ExpressionValue expFuncOrga(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	if(!g_fileManager->hasOpenFile())
	{
		Logger::queueError(Logger::Error,L"orga: no file opened");
		return ExpressionValue();
	}
	return ExpressionValue(g_fileManager->getPhysicalAddress());
}

ExpressionValue expFuncHeaderSize(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	if(!g_fileManager->hasOpenFile())
	{
		Logger::queueError(Logger::Error,L"headersize: no file opened");
		return ExpressionValue();
	}
	return ExpressionValue(g_fileManager->getHeaderSize());
}

ExpressionValue expFuncFileExists(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* fileName;
	GET_PARAM(parameters,0,fileName);

	std::wstring fullName = getFullPathName(*fileName);
	return ExpressionValue(fileExists(fullName) ? INT64_C(1) : INT64_C(0));
}

ExpressionValue expFuncFileSize(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* fileName;
	GET_PARAM(parameters,0,fileName);

	std::wstring fullName = getFullPathName(*fileName);
	return ExpressionValue((int64_t) fileSize(fullName));
}

ExpressionValue expFuncToString(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;

	switch (parameters[0].type)
	{
	case ExpressionValueType::String:
		result.strValue = parameters[0].strValue;
		break;
	case ExpressionValueType::Integer:
		result.strValue = formatString(L"%d",parameters[0].intValue);
		break;
	case ExpressionValueType::Float:
		result.strValue = formatString(L"%#.17g",parameters[0].floatValue);
		break;
	default:
		return result;
	}

	result.type = ExpressionValueType::String;
	return result;
}

ExpressionValue expFuncToHex(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	int64_t value, digits;
	GET_PARAM(parameters,0,value);
	GET_OPTIONAL_PARAM(parameters,1,digits,8);

	return ExpressionValue(formatString(L"%0*X",digits,value));
}

ExpressionValue expFuncInt(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Integer:
		result.intValue = parameters[0].intValue;
		break;
	case ExpressionValueType::Float:
		result.intValue = (int64_t) parameters[0].floatValue;
		break;
	default:
		return result;
	}

	result.type = ExpressionValueType::Integer;
	return result;
}

ExpressionValue expFuncRound(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Integer:
		result.intValue = parameters[0].intValue;
		break;
	case ExpressionValueType::Float:
		result.intValue = llround(parameters[0].floatValue);
		break;
	default:
		return result;
	}

	result.type = ExpressionValueType::Integer;
	return result;
}

ExpressionValue expFuncFloat(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Integer:
		result.floatValue = (double) parameters[0].intValue;
		break;
	case ExpressionValueType::Float:
		result.floatValue = parameters[0].floatValue;
		break;
	default:
		return result;
	}

	result.type = ExpressionValueType::Float;
	return result;
}

ExpressionValue expFuncFrac(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;
	double intPart;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Float:
		result.floatValue = modf(parameters[0].floatValue,&intPart);
		break;
	default:
		return result;
	}

	result.type = ExpressionValueType::Float;
	return result;
}

ExpressionValue expFuncMin(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;
	double floatA, floatB;
	int64_t intA, intB;

	bool isInt = true;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Integer:
		intA = parameters[0].intValue;
		floatA = (double)parameters[0].intValue;
		break;
	case ExpressionValueType::Float:
		floatA = parameters[0].floatValue;
		isInt = false;
		break;
	default:
		return result;
	}

	switch (parameters[1].type)
	{
	case ExpressionValueType::Integer:
		intB = parameters[1].intValue;
		floatB = (double)parameters[1].intValue;
		break;
	case ExpressionValueType::Float:
		floatB = parameters[1].floatValue;
		isInt = false;
		break;
	default:
		return result;
	}

	if (isInt)
	{
		result.intValue = std::min(intA, intB);
		result.type = ExpressionValueType::Integer;
	}
	else
	{
		result.floatValue = std::min(floatA, floatB);
		result.type = ExpressionValueType::Float;
	}

	return result;
}

ExpressionValue expFuncMax(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;
	double floatA, floatB;
	int64_t intA, intB;

	bool isInt = true;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Integer:
		intA = parameters[0].intValue;
		floatA = (double)parameters[0].intValue;
		break;
	case ExpressionValueType::Float:
		floatA = parameters[0].floatValue;
		isInt = false;
		break;
	default:
		return result;
	}

	switch (parameters[1].type)
	{
	case ExpressionValueType::Integer:
		intB = parameters[1].intValue;
		floatB = (double)parameters[1].intValue;
		break;
	case ExpressionValueType::Float:
		floatB = parameters[1].floatValue;
		isInt = false;
		break;
	default:
		return result;
	}

	if (isInt)
	{
		result.intValue = std::max(intA, intB);
		result.type = ExpressionValueType::Integer;
	}
	else
	{
		result.floatValue = std::max(floatA, floatB);
		result.type = ExpressionValueType::Float;
	}

	return result;
}

ExpressionValue expFuncAbs(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	ExpressionValue result;

	switch (parameters[0].type)
	{
	case ExpressionValueType::Float:
		result.type = ExpressionValueType::Float;
		result.floatValue = fabs(parameters[0].floatValue);
		break;
	case ExpressionValueType::Integer:
		result.type = ExpressionValueType::Integer;
		result.intValue = parameters[0].intValue >= 0 ?
			parameters[0].intValue : -parameters[0].intValue;
		break;
	default:
		break;
	}

	return result;
}

ExpressionValue expFuncStrlen(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* source;
	GET_PARAM(parameters,0,source);

	return ExpressionValue((int64_t)source->size());
}

ExpressionValue expFuncSubstr(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	int64_t start, count;
	const std::wstring* source;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,start);
	GET_PARAM(parameters,2,count);

	return ExpressionValue(source->substr((size_t)start,(size_t)count));
}

#if ARMIPS_REGEXP
ExpressionValue expFuncRegExMatch(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* source;
	const std::wstring* regexString;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,regexString);

#if ARMIPS_EXCEPTIONS
	try
	{
#endif
		std::wregex regex(*regexString);
		bool found = std::regex_match(*source,regex);
		return ExpressionValue(found ? INT64_C(1) : INT64_C(0));
#if ARMIPS_EXCEPTIONS
	} catch (std::regex_error&)
	{
		Logger::queueError(Logger::Error,L"Invalid regular expression");
		return ExpressionValue();
	}
#endif
}

ExpressionValue expFuncRegExSearch(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* source;
	const std::wstring* regexString;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,regexString);

#if ARMIPS_EXCEPTIONS
	try
	{
#endif
		std::wregex regex(*regexString);
		bool found = std::regex_search(*source,regex);
		return ExpressionValue(found ? INT64_C(1) : INT64_C(0));
#if ARMIPS_EXCEPTIONS
	} catch (std::regex_error&)
	{
		Logger::queueError(Logger::Error,L"Invalid regular expression");
		return ExpressionValue();
	}
#endif
}

ExpressionValue expFuncRegExExtract(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* source;
	const std::wstring* regexString;
	int64_t matchIndex;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,regexString);
	GET_OPTIONAL_PARAM(parameters,2,matchIndex,0);

#if ARMIPS_EXCEPTIONS
	try
	{
#endif
		std::wregex regex(*regexString);
		std::wsmatch result;
		bool found = std::regex_search(*source,result,regex);
		if (found == false || (size_t)matchIndex >= result.size())
		{
			Logger::queueError(Logger::Error,L"Capture group index %d does not exist",matchIndex);
			return ExpressionValue();
		}
	
		return ExpressionValue(result[(size_t)matchIndex].str());
#if ARMIPS_EXCEPTIONS
	} catch (std::regex_error&)
	{
		Logger::queueError(Logger::Error,L"Invalid regular expression");
		return ExpressionValue();
	}
#endif
}
#endif

ExpressionValue expFuncFind(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	int64_t start;
	const std::wstring* source;
	const std::wstring* value;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,value);
	GET_OPTIONAL_PARAM(parameters,2,start,0);

	size_t pos = source->find(*value,(size_t)start);
	return ExpressionValue(pos == std::wstring::npos ? INT64_C(-1) : (int64_t) pos);
}

ExpressionValue expFuncRFind(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	int64_t start;
	const std::wstring* source;
	const std::wstring* value;

	GET_PARAM(parameters,0,source);
	GET_PARAM(parameters,1,value);
	GET_OPTIONAL_PARAM(parameters,2,start,std::wstring::npos);

	size_t pos = source->rfind(*value,(size_t)start);
	return ExpressionValue(pos == std::wstring::npos ? INT64_C(-1) : (int64_t) pos);
}


template<typename T>
ExpressionValue expFuncRead(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* fileName;
	int64_t pos;

	GET_PARAM(parameters,0,fileName);
	GET_OPTIONAL_PARAM(parameters,1,pos,0);

	std::wstring fullName = getFullPathName(*fileName);

	BinaryFile file;
	if (file.open(fullName,BinaryFile::Read) == false)
	{
		Logger::queueError(Logger::Error,L"Could not open %s",*fileName);
		return ExpressionValue();
	}

	file.setPos(pos);

	T buffer;
	if (file.read(&buffer, sizeof(T)) != sizeof(T))
	{
		Logger::queueError(Logger::Error, L"Failed to read %d byte(s) from offset 0x%08X of %s", sizeof(T), pos, *fileName);
		return ExpressionValue();
	}

	return ExpressionValue((int64_t) buffer);
}

ExpressionValue expFuncReadAscii(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	const std::wstring* fileName;
	int64_t start;
	int64_t length;

	GET_PARAM(parameters,0,fileName);
	GET_OPTIONAL_PARAM(parameters,1,start,0);
	GET_OPTIONAL_PARAM(parameters,2,length,0);

	std::wstring fullName = getFullPathName(*fileName);

	int64_t totalSize = fileSize(fullName);
	if (length == 0 || start+length > totalSize)
		length = totalSize-start;

	BinaryFile file;
	if (file.open(fullName,BinaryFile::Read) == false)
	{
		Logger::queueError(Logger::Error,L"Could not open %s",fileName);
		return ExpressionValue();
	}

	file.setPos((long)start);

	unsigned char* buffer = new unsigned char[length];
	file.read(buffer,(size_t)length);

	std::wstring result;
	for (size_t i = 0; i < (size_t) length; i++)
	{
		if (buffer[i] < 0x20 || buffer[i] > 0x7F)
		{
			Logger::printError(Logger::Warning,L"%s: Non-ASCII character",funcName);
			return ExpressionValue();
		}

		result += (wchar_t) buffer[i];
	}

	delete[] buffer;

	return ExpressionValue(result);
}

ExpressionValue expFuncDefined(ExpressionInternal* exp)
{
	if (exp == nullptr || exp->isIdentifier() == false)
	{
		Logger::queueError(Logger::Error,L"Invalid parameter 1 for defined: expecting identifier");
		return ExpressionValue();
	}

	const std::wstring& name = exp->getStringValue();
	std::shared_ptr<Label> label = Global.symbolTable.getLabel(name,exp->getFileNum(),exp->getSection());

	if (label == nullptr)
		return ExpressionValue();

	return ExpressionValue(label->isDefined() ? INT64_C(1) : INT64_C(0));
}



const ExpressionFunctionMap expressionFunctions = {
	{ L"version",		{ &expFuncVersion,			0,	0,	ExpFuncSafety::Safe } },
	{ L"endianness",	{ &expFuncEndianness,		0,	0,	ExpFuncSafety::Unsafe } },
	{ L"outputname",	{ &expFuncOutputName,		0,	0,	ExpFuncSafety::Unsafe } },
	{ L"org",			{ &expFuncOrg,				0,	0,	ExpFuncSafety::Unsafe } },
	{ L"orga",			{ &expFuncOrga,				0,	0,	ExpFuncSafety::Unsafe } },
	{ L"headersize",	{ &expFuncHeaderSize,		0,	0,	ExpFuncSafety::Unsafe } },
	{ L"fileexists",	{ &expFuncFileExists,		1,	1,	ExpFuncSafety::Safe } },
	{ L"filesize",		{ &expFuncFileSize,			1,	1,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"tostring",		{ &expFuncToString,			1,	1,	ExpFuncSafety::Safe } },
	{ L"tohex",			{ &expFuncToHex,			1,	2,	ExpFuncSafety::Safe } },

	{ L"int",			{ &expFuncInt,				1,	1,	ExpFuncSafety::Safe } },
	{ L"float",			{ &expFuncFloat,			1,	1,	ExpFuncSafety::Safe } },
	{ L"frac",			{ &expFuncFrac,				1,	1,	ExpFuncSafety::Safe } },
	{ L"abs",			{ &expFuncAbs,				1,	1,	ExpFuncSafety::Safe } },
	{ L"round",			{ &expFuncRound,			1,	1,	ExpFuncSafety::Safe } },
	{ L"min",			{ &expFuncMin,				2,	2,	ExpFuncSafety::Safe } },
	{ L"max",			{ &expFuncMax,				2,	2,	ExpFuncSafety::Safe } },

	{ L"strlen",		{ &expFuncStrlen,			1,	1,	ExpFuncSafety::Safe } },
	{ L"substr",		{ &expFuncSubstr,			3,	3,	ExpFuncSafety::Safe } },
#if ARMIPS_REGEXP
	{ L"regex_match",	{ &expFuncRegExMatch,		2,	2,	ExpFuncSafety::Safe } },
	{ L"regex_search",	{ &expFuncRegExSearch,		2,	2,	ExpFuncSafety::Safe } },
	{ L"regex_extract",	{ &expFuncRegExExtract,		2,	3,	ExpFuncSafety::Safe } },
#endif
	{ L"find",			{ &expFuncFind,				2,	3,	ExpFuncSafety::Safe } },
	{ L"rfind",			{ &expFuncRFind,			2,	3,	ExpFuncSafety::Safe } },

	{ L"readbyte",		{ &expFuncRead<uint8_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"readu8",		{ &expFuncRead<uint8_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"readu16",		{ &expFuncRead<uint16_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"readu32",		{ &expFuncRead<uint32_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"readu64",		{ &expFuncRead<uint64_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"reads8",		{ &expFuncRead<int8_t>,		1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"reads16",		{ &expFuncRead<int16_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"reads32",		{ &expFuncRead<int32_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"reads64",		{ &expFuncRead<int64_t>,	1,	2,	ExpFuncSafety::ConditionalUnsafe } },
	{ L"readascii",		{ &expFuncReadAscii,		1,	3,	ExpFuncSafety::ConditionalUnsafe } },
};
