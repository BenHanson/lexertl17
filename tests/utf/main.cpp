#include <lexertl/utf_iterators.hpp>

#include <iostream>
#include <iterator>

const char32_t g_out[] =
{
	U'\x000048', U'\x000065', U'\x00006c', U'\x00006c', U'\x00006f', U'\x000020',
	U'\x000057', U'\x00006f', U'\x000072', U'\x00006c', U'\x000064', U'\x00000a',
	U'\x000063', U'\x000061', U'\x000066', U'\x0000e9', U'\x00000a',
	U'\x00041f', U'\x000440', U'\x000438', U'\x000432', U'\x000435', U'\x000442', U'\x00000a',
	U'\x01f600', U'\x00000a',
	U'\x010400', U'\x00000a'
};

bool utf8()
{
	bool success = true;
	const char in[] =
	{
		'\x48', '\x65', '\x6c', '\x6c', '\x6f', '\x20',
		'\x57', '\x6f', '\x72', '\x6c', '\x64', '\x0a',
		'\x63', '\x61', '\x66', '\xc3', '\xa9', '\x0a',
		'\xd0', '\x9f', '\xd1', '\x80', '\xd0', '\xb8', '\xd0', '\xb2', '\xd0',
		'\xb5', '\xd1', '\x82', '\x0a',
		'\xf0', '\x9f', '\x98', '\x80', '\x0a',
		'\xf0', '\x90', '\x90', '\x80', '\x0a'
	};
	lexertl::cutf8_in_utf32_out_iterator iter(std::begin(in),
		std::end(in));
	lexertl::cutf8_in_utf32_out_iterator end(std::end(in), std::end(in));
	const char32_t* expected_iter = std::begin(g_out);
	const char32_t* expected_end = std::end(g_out);

	for (; iter != end; ++iter, ++expected_iter)
	{
		const auto out = *iter;

		if (expected_iter == expected_end || out != *expected_iter)
		{
			success = false;
			break;
		}
	}

	if (!success)
		std::cerr << "utf8 test failed\n";

	return success;
}

bool utf16()
{
    bool success = true;
	const char16_t in[] =
	{
		u'\x0048', u'\x0065', u'\x006c', u'\x006c', u'\x006f', u'\x0020',
		u'\x0057', u'\x006f', u'\x0072', u'\x006c', u'\x0064', u'\x000a',
		u'\x0063', u'\x0061', u'\x0066', u'\x00e9', u'\x000a',
		u'\x041f', u'\x0440', u'\x0438', u'\x0432', u'\x0435', u'\x0442', u'\x000a',
		u'\xd83d', u'\xde00', u'\x000a',
		u'\xd801', u'\xdc00', u'\x000a' };
	lexertl::cutf16_in_utf32_out_iterator iter(std::begin(in),
		std::end(in));
	lexertl::cutf16_in_utf32_out_iterator end(std::end(in), std::end(in));
	const char32_t* expected_iter = std::begin(g_out);
	const char32_t* expected_end = std::end(g_out);

	for (; iter != end; ++iter, ++expected_iter)
	{
		const auto out = *iter;

		if (expected_iter == expected_end || out != *expected_iter)
		{
			success = false;
			break;
		}
	}

    if (!success)
        std::cerr << "utf16 test failed\n";

    return success;
}

int main()
{
	bool success = utf16();

    success &= utf8();

	if (success)
	{
		std::cout << "All utf tests passed\n";
		return 0;
	}
	else
		return 1;
}
