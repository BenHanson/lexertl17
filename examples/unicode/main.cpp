#include <fstream>
#include <lexertl/generator.hpp>
#include <iomanip>
#include <lexertl/lookup.hpp>
#include <lexertl/memory_file.hpp>
#include <lexertl/stream_shared_iterator.hpp>
#include <lexertl/utf_iterators.hpp>

#ifdef WIN32
#include <windows.h>
#endif

void test_unicode()
{
    lexertl::cmatch r_;
    const char *i_ = "";

    r_.clear();
    r_.reset(i_, i_);

    const char utf8_[] = "\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88\x7f";
    lexertl::basic_utf8_in_iterator<const char *, char32_t> u8iter_(utf8_,
        utf8_ + sizeof(utf8_));
    char32_t i = *u8iter_; // 0x10346

    i = *++u8iter_; // 0x65e5
    i = *u8iter_++; // 0x65e5
    i = *u8iter_; // 0x0448
    i = *++u8iter_; // 0x7f

    const char32_t utf32_[] = U"\U0010ffff\U00010401\U00010000\U0000d7ff";
    lexertl::basic_utf16_out_iterator<const char32_t*, char16_t> u16iter_(utf32_);
    char16_t i16 = *u16iter_; // 0xdbff

    assert(i16 == 0xdbff);
    ++u16iter_;
    i16 = *u16iter_; // 0xdfff
    assert(i16 == 0xdfff);
    ++u16iter_;
    i16 = *u16iter_; // 0xd801
    assert(i16 == 0xd801);
    ++u16iter_;
    i16 = *u16iter_; // 0xdc01
    assert(i16 == 0xdc01);
    ++u16iter_;
    i16 = *u16iter_; // 0xd800
    assert(i16 == 0xd800);
    ++u16iter_;
    i16 = *u16iter_; // 0xdc00
    assert(i16 == 0xdc00);
    ++u16iter_;
    i16 = *u16iter_; // 0xd7ff
    assert(i16 == 0xd7ff);

    const char16_t utf16_[] = u"\xdbff\xdfff\xd801\xdc01\xd800\xdc00\xd7ff";
    lexertl::basic_utf16_in_iterator<const char16_t*, char32_t> u32iter_(utf16_,
        utf16_ + sizeof(utf16_) / sizeof(char16_t));

    i = *u32iter_; // 0x10ffff
    assert(i == 0x10ffff);
    i = *++u32iter_; // 0x10401
    assert(i == 0x10401);
    i = *u32iter_++; // 0x10401
    assert(i == 0x10401);
    i = *u32iter_; // 0x10000
    assert(i == 0x10000);
    i = *++u32iter_; // 0xd7ff
    assert(i == 0xd7ff);

    lexertl::basic_rules<char, char32_t> rules_(*lexertl::regex_flags::icase);
    lexertl::u32state_machine sm_;
    const char32_t in_[] = {0x393, ' ', 0x393, 0x398, ' ', 0x398,
        '1', ' ', 'i', 'd', 0x41f, 0};
    std::u32string input_(in_);
    const char32_t* iter_ = input_.c_str();
    const char32_t* end_ = iter_ + input_.size();
    lexertl::u32cmatch results_(iter_, end_);

    rules_.push("\\p{LC}[\\p{LC}0-9]*", 1);
    lexertl::basic_generator<lexertl::basic_rules<char, char32_t>,
        lexertl::u32state_machine>::build(rules_, sm_);

#ifdef WIN32
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwBytesWritten = 0;
#endif

    do
    {
#ifdef WIN32
        std::wstring str_;
#else
        std::string str_;
#endif

        lexertl::lookup(sm_, results_);

#ifdef WIN32
        str_.assign(lexertl::basic_utf16_out_iterator<const char32_t *, wchar_t>
            (results_.first),
            lexertl::basic_utf16_out_iterator<const char32_t *, wchar_t>
            (results_.second));
        std::wcout << L"Id: " << results_.id << L", Token: '";
        ::WriteConsoleW(hStdOut, str_.c_str(), static_cast<DWORD>(str_.size()),
            &dwBytesWritten, nullptr);
        std::wcout << '\'' << std::endl;
#else
        str_.assign(lexertl::basic_utf8_out_iterator<const char32_t *>(results_.first),
            lexertl::basic_utf8_out_iterator<const char32_t *>(results_.second));
        std::cout << "Id: " << results_.id << ", Token: '" <<
            str_ << '\'' << std::endl;
#endif
    } while (results_.id != 0);
}

int main(int /*argc*/, char ** /*argv*/)
{
    test_unicode();
    return 0;
}
