RapidJSON Utils
===============

This header-only utility includes a set of templates to help with RapidJSON
usage.

All the templates functions defined here are inline and shouldn't rely on any
dependencies, other than RapidJSON includes and the standard C/C++ headers and
libraries.

This code is meant to simplify the development, but still keep the best
performance possible, being possible to be used inside loops without
slowdowns.

There are two versions of this library: for C++17 and for C++14. You can find
them in their respective directories.


C++17 version
-------------

By using C++17 `if constexpr` is possible to generate specialized templates
with as minimum code duplication as possible without losing performance to
runtime checks.

There is one caveat though: The use of automatic type detection will default to
`int32_t` if an explicit type isn't passed when using the templated functions
(e.g. `Extract<uint64_t>`) or when not constructing the `default_value` to an
explicit default type (e.g. `Extract(doc, "example", int64_t(0));`).
Keep that in mind and specialize whenever necessary!

There are a few asserts in the code to warn developers about attempting to
extract to the wrong type - e.g. trying to extract an int64_t in an uint32_t.

```cpp
inline bool ParseFile(const char* file_name, rapidjson::Document& out_document)
```
This function opens and parses a file, and return if the parsing was successful.

---

```cpp
inline bool IsValid(const RapidJsonTarget& target_element, const Ch* member)
```
*NOTE: This function requires a specialization to be used.*

This function checks if given `member` exists inside `target_element` and if it
is of the specialized type - e.g.:
```cpp
if (IsValid<uint64_t>(document, "distance"))
{
	// Do stuff
}
```

---

```cpp
inline bool IsValidArray(const RapidJsonTarget& target_element, const Ch* member)
```
This function checks if given `member` is an Array.

---

```cpp
inline bool IsValidObject(const RapidJsonTarget& target_element, const Ch* member)
```
This function checks if given `member` is an Object

---

```cpp
inline DataType Extract(const RapidJsonTarget& target_element, const Ch* member, DataType default_value)
```
*NOTE: This function can be specialized. It is recommended to do so whenever
supplying integers as the `default_value`!*

This function attempts to extract the value of the given `member` if it exists
inside `target_element`. If it fails to extract, returns the supplied
`default_value`.

Due how to the type detection works, it will almost always deduce `int32_t` as
the type when working as integers. Remember to specialize the function whenever
necessary.

---

```cpp
inline DataType ExtractFromNumericOrString(const RapidJsonTarget& target_element, const Ch* member, DataType default_value)
```
*NOTE: This function can be specialized. It is recommended to do so whenever
supplying integers as the `default_value`!*

Sometimes we get that odd data file that has numbers as strings. This function
attempt to simplify this extraction process, when the type isn't reliable or
when numbers are given as strings.

Due how to the type detection works, it will almost always deduce `int32_t` as
the type when working as integers. Remember to specialize the function whenever
necessary.

---

C++14 Version
-------------

Soon™!