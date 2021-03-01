#pragma once

//
// RapidJSON Utils
//
// Felipe Guedes
// March 2021
//
// This header - only utility includes a set of templates to help with RapidJSON
// usage.
//
// All the templates functions defined here are inline and shouldn't rely on any
// dependencies, other than RapidJSON includesand the standard C / C++ headers and
// libraries.
//
// This code is meant to simplify the development, but still keep the best
// performance possible, being possible to be used inside loops without
// slowdowns.
//
// By using C++17 `if constexpr` is possible to generate specialized templates
// with as minimum code duplication as possible without losing performance to
// runtime checks.
//
// There is one caveat though: The use of automatic type detection will default to
// `int32_t` if an explicit type isn't passed when using the templated functions
// (e.g. `Extract<uint64_t>`) or when not constructing the default_value to an
// explicit default type (e.g. `Extract(doc, "example", int64_t(0));`).
// Keep that in mind and specialize whenever necessary!
//

#include <cassert>
#include <cstdio>
#include <climits>
#include "RapidJSON/rapidjson.h"
#include "RapidJSON/document.h"
#include "RapidJSON/encodings.h"
#include "RapidJSON/filereadstream.h"

namespace rjutils  // RapidJSON Utils
{
	inline bool ParseFile(const char* file_name, rapidjson::Document& out_document)
	{
		// We avoid using ifstream here as recommended in the documentation to improve the performance
		// https://rapidjson.org/md_doc_stream.html
	#ifdef _WIN32
		#pragma warning(disable:4996)
		FILE* fp = fopen(file_name, "rb");
		#pragma warning(default:4996)
	#else
		FILE* fp = fopen(file_name, "r");
	#endif
		assert(fp);
		if (!fp)
			return false;

		constexpr int BUFFER_SIZE = 65536;
		char* readBuffer = static_cast<char*>(malloc(BUFFER_SIZE));
		if (!readBuffer)
			return false;

		rapidjson::FileReadStream is(fp, readBuffer, BUFFER_SIZE);
		out_document.ParseStream(is);
		fclose(fp);
		free(readBuffer);

		return !out_document.HasParseError();
	}

	template<typename DataType, typename Ch = char, typename RapidJsonTarget>
	inline bool IsValid(const RapidJsonTarget& target_element, const Ch* member)
	{
		static_assert (std::is_same<RapidJsonTarget, rapidjson::Document>::value || std::is_same<RapidJsonTarget, rapidjson::Value>::value, "rjutil::IsValid<>() only supports rapidjson::Document and rapidjson::Value as the first parameter");
		if (!target_element.HasMember(member))
			return false;

		if constexpr (std::is_same<DataType, int32_t>::value)
			return target_element[member].IsInt();
		else if constexpr (std::is_same<DataType, uint32_t>::value)
			return target_element[member].IsUint();
		else if constexpr (std::is_same<DataType, int64_t>::value)
			return target_element[member].IsInt64();
		else if constexpr (std::is_same<DataType, uint64_t>::value)
			return target_element[member].IsUint64();
		else if constexpr (std::is_same<DataType, bool>::value)
			return target_element[member].IsBool();
		else if constexpr (std::is_floating_point<DataType>::value)
			return target_element[member].IsDouble();
		else if constexpr (std::is_same<DataType, char>::value
			|| std::is_same<DataType, const char>::value
			|| std::is_same<DataType, char*>::value
			|| std::is_same<DataType, const char*>::value
			|| std::is_same<DataType, std::string>::value
			|| std::is_same<DataType, const std::string>::value)
			return target_element[member].IsString();
		else if constexpr (std::is_integral<DataType>::value && std::is_signed<DataType>::value)
			return target_element[member].IsInt64();
		else if constexpr (std::is_integral<DataType>::value && std::is_unsigned<DataType>::value)
			return target_element[member].IsUint64();
		else
			static_assert(false, "Attempting to invoke rjutil::IsValid<>() with invalid data type");

		return false;
	}

	template<typename Ch = char, typename RapidJsonTarget>
	inline bool IsValidArray(const RapidJsonTarget& target_element, const Ch* member)
	{
		static_assert (std::is_same<RapidJsonTarget, rapidjson::Document>::value || std::is_same<RapidJsonTarget, rapidjson::Value>::value, "rjutil::IsValidJsonArray() only supports rapidjson::Document and rapidjson::Value as the first parameter");
		return target_element.HasMember(member) && target_element[member].IsArray();
	}

	template<typename Ch = char, typename RapidJsonTarget>
	inline bool IsValidObject(const RapidJsonTarget& target_element, const Ch* member)
	{
		static_assert (std::is_same<RapidJsonTarget, rapidjson::Document>::value || std::is_same<RapidJsonTarget, rapidjson::Value>::value, "rjutil::IsValidJsonObject() only supports rapidjson::Document and rapidjson::Value as the first parameter");
		return target_element.HasMember(member) && target_element[member].IsObject();
	}

	template<typename DataType, typename Ch = char, typename RapidJsonTarget>
	inline DataType Extract(const RapidJsonTarget& target_element, const Ch* member, DataType default_value)
	{
		static_assert (std::is_same<RapidJsonTarget, rapidjson::Document>::value || std::is_same<RapidJsonTarget, rapidjson::Value>::value, "rjutil::Extract<>() only supports rapidjson::Document and rapidjson::Value as the first parameter");

		if (!target_element.HasMember(member))
			return default_value;

		if constexpr (std::is_same<DataType, int32_t>::value)
		{
			if (target_element[member].IsInt())
				return target_element[member].GetInt();

			// If you hit this assert, there's a good chance you're not specializing the function
			// call (e.g. `Extract<int32_t>`) and the compiler is relying in detecting the type of
			// DataType parameter. If you're specializing it, you probably selected the wrong type
			// for the specialization.
			assert(!target_element[member].IsNumber());
		}
		else if constexpr (std::is_same<DataType, uint32_t>::value)
		{
			if (target_element[member].IsUint())
				return target_element[member].GetUint();
		}
		else if constexpr (std::is_same<DataType, int64_t>::value)
		{
			if (target_element[member].IsInt64())
				return target_element[member].GetInt64();
		}
		else if constexpr (std::is_same<DataType, uint64_t>::value)
		{
			if (target_element[member].IsUint64())
				return target_element[member].GetUint64();
		}
		else if constexpr (std::is_same<DataType, bool>::value)
		{
			if (target_element[member].IsBool())
				return target_element[member].GetBool();
		}
		else if constexpr (std::is_same<DataType, float>::value)
		{
			if (target_element[member].IsDouble())
				return static_cast<float>(target_element[member].GetDouble());
		}
		else if constexpr (std::is_floating_point<DataType>::value)
		{
			if (target_element[member].IsDouble())
				return target_element[member].GetDouble();
		}
		else if constexpr (std::is_same<DataType, char>::value
			|| std::is_same<DataType, const char>::value
			|| std::is_same<DataType, char*>::value
			|| std::is_same<DataType, const char*>::value
			|| std::is_same<DataType, std::string>::value
			|| std::is_same<DataType, const std::string>::value)
		{
			if (target_element[member].IsString())
				return target_element[member].GetString();
		}
		else if constexpr (std::is_integral<DataType>::value && std::is_signed<DataType>::value)
		{
			if (target_element[member].IsInt64())
				return target_element[member].GetInt64();
		}
		else if constexpr (std::is_integral<DataType>::value && std::is_unsigned<DataType>::value)
		{
			if (target_element[member].IsUint64())
				return target_element[member].GetUint64();
		}
		else
		{
			static_assert(false, "Attempting to invoke rjutil::Extract<>() with invalid data type");
		}

		return default_value;
	}

	template<typename DataType, typename Ch = char, typename RapidJsonTarget>
	inline DataType ExtractFromNumericOrString(const RapidJsonTarget& target_element, const Ch* member, DataType default_value)
	{
		static_assert (std::is_same<RapidJsonTarget, rapidjson::Document>::value || std::is_same<RapidJsonTarget, rapidjson::Value>::value, "ExtractFromNumericOrString() only supports rapidjson::Document and rapidjson::Value as the first parameter");

		if (!target_element.HasMember(member))
			return default_value;

		if (target_element[member].IsNumber())
			return Extract<DataType>(target_element, member, default_value);

		if (target_element[member].IsString())
		{
			if constexpr (std::is_same<DataType, int32_t>::value)
			{
				const int32_t value = strtol(target_element[member].GetString(), nullptr, 10);

				// If you hit this assert, there's a good chance you're not specializing the function
				// call (e.g. `ExtractFromNumericOrString<int32_t>`) and the compiler is relying in
				// detecting the type of DataType parameter. If you're specializing it, you probably
				// selected the wrong type for the specialization.
				//
				// Please notice: If you ARE specializing it and still getting this error, use a
				// larger variable type (e.g. int64_t). You're getting results out of signed 32 bits
				// int bounds.
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
			else if constexpr (std::is_same<DataType, uint32_t>::value)
			{
				const uint32_t value = strtoul(target_element[member].GetString(), nullptr, 10);

				// Out of bounds?
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
			else if constexpr (std::is_same<DataType, int64_t>::value)
			{
				const int64_t value = strtoll(target_element[member].GetString(), nullptr, 10);

				// Out of bounds?
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
			else if constexpr (std::is_same<DataType, uint64_t>::value)
			{
				const uint64_t value = strtoull(target_element[member].GetString(), nullptr, 10);

				// Out of bounds?
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
			else if constexpr (std::is_same<DataType, float>::value)
			{
				const float value = strtof(target_element[member].GetString(), nullptr);

				// Out of bounds?
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
			else if constexpr (std::is_floating_point<DataType>::value)
			{
				const double value = strtod(target_element[member].GetString(), nullptr);

				// Out of bounds?
				assert(errno != ERANGE);

				if (errno != 0)
					return default_value;

				return value;
			}
		}

		return default_value;
	}
}
