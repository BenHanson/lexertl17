// utf_iterators.hpp
// Copyright (c) 2015-2023 Ben Hanson (http://www.benhanson.net/)
// Inspired by http://utfcpp.sourceforge.net/
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef LEXERTL_UTF_ITERATORS_HPP
#define LEXERTL_UTF_ITERATORS_HPP

#include <iterator>
#include <stdexcept>

namespace lexertl
{
    template<typename char_iterator, typename char_type>
    class basic_utf8_in_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = char_type;
        using difference_type =
            typename std::iterator_traits<char_iterator>::difference_type;
        using pointer =
            typename std::iterator_traits<char_iterator>::pointer;
        using reference =
            typename std::iterator_traits<char_iterator>::reference;

        basic_utf8_in_iterator() = default;

        explicit basic_utf8_in_iterator(const char_iterator& it_,
            const char_iterator& end_) :
            _it(it_),
            _end(it_),
            _eoi(end_)
        {
            if (_it != _eoi)
            {
                next();
            }
        }

        char_type operator *() const
        {
            return _char;
        }

        bool operator >(const basic_utf8_in_iterator& rhs_) const
        {
            return _it > rhs_._it;
        }

        bool operator ==(const basic_utf8_in_iterator& rhs_) const
        {
            return _it == rhs_._it;
        }

        bool operator !=(const basic_utf8_in_iterator& rhs_) const
        {
            return _it != rhs_._it;
        }

        basic_utf8_in_iterator& operator ++()
        {
            _it = _end;
            next();
            return *this;
        }

        basic_utf8_in_iterator operator ++(int)
        {
            basic_utf8_in_iterator temp_ = *this;

            _it = _end;
            next();
            return temp_;
        }

        basic_utf8_in_iterator operator +(const std::size_t count_) const
        {
            basic_utf8_in_iterator temp_ = *this;

            for (std::size_t i_ = 0; i_ < count_; ++i_)
            {
                ++temp_;
            }

            return temp_;
        }

        basic_utf8_in_iterator operator -(const std::size_t count_) const
        {
            basic_utf8_in_iterator temp_ = *this;

            for (std::size_t i_ = 0; i_ < count_; ++i_)
            {
                temp_._end = temp_._it;
                --temp_._it;

                while ((*temp_._it & 0xc0) == 0x80) --temp_._it;
            }

            temp_.next();
            return temp_;
        }

        char_iterator get() const
        {
            return _it;
        }

    private:
        char_iterator _it = char_iterator();
        char_iterator _end = char_iterator();
        char_iterator _eoi = char_iterator();
        char_type _char = 0;

        void next()
        {
            const char len_ = len(_it);
            char_type ch_ = *_it & 0xff;

            switch (len_)
            {
            case 1:
                _end = _it;
                ++_end;
                break;
            case 2:
                _end = _it;
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ = (ch_ << 6 & 0x7ff) | (*_end & 0x3f);
                ++_end;
                break;
            case 3:
                _end = _it;
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ = (ch_ << 12 & 0xffff) | ((*_end & 0xff) << 6 & 0xfff);
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ |= *_end & 0x3f;
                ++_end;
                break;
            case 4:
                _end = _it;
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ = (ch_ << 18 & 0x1fffff) | ((*_end & 0xff) << 12 & 0x3ffff);
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ |= (*_end & 0xff) << 6 & 0xfff;
                ++_end;

                if (_end == _eoi)
                    truncated_utf8();

                if ((*_end & 0xc0) != 0x80)
                    invalid_utf8();

                ch_ |= *_end & 0x3f;
                ++_end;
                break;
            default:
                invalid_utf8();
                break;
            }

            _char = ch_;
        }

        char len(const char_iterator& it_) const
        {
            const unsigned char ch_ = *it_;

            if (ch_ < 0x80)
                return 1;

            if ((ch_ & 0xe0) == 0xc0)
                return 2;

            if ((ch_ & 0xf0) == 0xe0)
                return 3;

            if ((ch_ & 0xf8) == 0xf0)
                return 4;

            // Invalid UTF-8
            return 127;
        }

        void truncated_utf8()
        {
            throw std::range_error("Truncated UTF-8");
        }

        void invalid_utf8()
        {
            throw std::range_error("Invalid UTF-8");
        }
    };

    template<typename char_iterator>
    class basic_utf8_out_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = char;
        using difference_type =
            typename std::iterator_traits<char_iterator>::difference_type;
        using pointer =
            typename std::iterator_traits<char_iterator>::pointer;
        using reference =
            typename std::iterator_traits<char_iterator>::reference;

        basic_utf8_out_iterator() = default;

        explicit basic_utf8_out_iterator(const char_iterator& it_,
            const char_iterator& end_) :
            _it(it_)
        {
            if (it_ != end_)
            {
                next();
            }
        }

        char operator *() const
        {
            return _bytes[_index];
        }

        bool operator ==(const basic_utf8_out_iterator& rhs_) const
        {
            return _it == rhs_._it;
        }

        bool operator !=(const basic_utf8_out_iterator& rhs_) const
        {
            return _it != rhs_._it;
        }

        basic_utf8_out_iterator& operator ++()
        {
            ++_index;

            if (_index >= _count)
            {
                ++_it;
                next();
            }

            return *this;
        }

        basic_utf8_out_iterator operator ++(int)
        {
            basic_utf8_out_iterator temp_ = *this;

            ++_index;

            if (_index >= _count)
            {
                ++_it;
                next();
            }

            return temp_;
        }

    private:
        char_iterator _it = char_iterator();
        char _bytes[4]{};
        unsigned char _count = 0;
        unsigned char _index = 0;

        void next()
        {
            const std::size_t ch_ = *_it;

            _count = len(ch_);
            _index = 0;

            switch (_count)
            {
            case 1:
                _bytes[0] = static_cast<char>(ch_);
                break;
            case 2:
                _bytes[0] = static_cast<char>((ch_ >> 6) | 0xc0);
                _bytes[1] = (ch_ & 0x3f) | 0x80;
                break;
            case 3:
                _bytes[0] = static_cast<char>((ch_ >> 12) | 0xe0);
                _bytes[1] = ((ch_ >> 6) & 0x3f) | 0x80;
                _bytes[2] = (ch_ & 0x3f) | 0x80;
                break;
            case 4:
                _bytes[0] = static_cast<char>((ch_ >> 18) | 0xf0);
                _bytes[1] = ((ch_ >> 12) & 0x3f) | 0x80;
                _bytes[2] = ((ch_ >> 6) & 0x3f) | 0x80;
                _bytes[3] = (ch_ & 0x3f) | 0x80;
                break;
            default:
                break;
            }
        }

        char len(const std::size_t ch_) const
        {
            return ch_ < 0x80 ? 1 :
                ch_ < 0x800 ? 2 :
                ch_ < 0x10000 ? 3 :
                4;
        }
    };

    template<typename char_iterator, typename char_type>
    class basic_utf16_in_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = char_type;
        using difference_type =
            typename std::iterator_traits<char_iterator>::difference_type;
        using pointer =
            typename std::iterator_traits<char_iterator>::pointer;
        using reference =
            typename std::iterator_traits<char_iterator>::reference;

        basic_utf16_in_iterator() = default;

        explicit basic_utf16_in_iterator(const char_iterator& it_,
            const char_iterator& end_) :
            _it(it_),
            _end(it_),
            _eoi(end_)
        {
            if (_it != _eoi)
            {
                next();
            }
        }

        char_type operator *() const
        {
            return _char;
        }

        bool operator ==(const basic_utf16_in_iterator& rhs_) const
        {
            return _it == rhs_._it;
        }

        bool operator !=(const basic_utf16_in_iterator& rhs_) const
        {
            return _it != rhs_._it;
        }

        basic_utf16_in_iterator& operator ++()
        {
            _it = _end;
            next();
            return *this;
        }

        basic_utf16_in_iterator operator ++(int)
        {
            basic_utf16_in_iterator temp_ = *this;

            _it = _end;
            next();
            return temp_;
        }

        basic_utf16_in_iterator operator +(const std::size_t count_) const
        {
            basic_utf16_in_iterator temp_ = *this;

            for (std::size_t i_ = 0; i_ < count_; ++i_)
            {
                ++temp_;
            }

            return temp_;
        }

        basic_utf16_in_iterator operator -(const std::size_t count_) const
        {
            basic_utf16_in_iterator temp_ = *this;

            for (std::size_t i_ = 0; i_ < count_; ++i_)
            {
                temp_._end = temp_._it;
                --temp_._it;

                if (*temp_._it >= 0xdc00 && *temp_._it <= 0xdfff) --temp_._it;
            }

            temp_.next();
            return temp_;
        }

    private:
        char_iterator _it = char_iterator();
        char_iterator _end = char_iterator();
        char_iterator _eoi = char_iterator();
        char_type _char = 0;

        void next()
        {
            char_type ch_ = *_it & 0xffff;

            if (ch_ >= 0xdc00 && ch_ <= 0xdfff)
                invalid_utf16();

            _end = _it;

            if (ch_ >= 0xd800 && ch_ <= 0xdbff)
            {
                const char_type surrogate_ = *++_end & 0xffff;

                if (_end == _eoi)
                    truncated_utf16();

                if (ch_ < 0xdc00 || ch_ > 0xdfff)
                    invalid_utf16();

                ch_ = (((ch_ - 0xd800) << 10) |
                    (surrogate_ - 0xdc00)) + 0x10000;
            }

            _char = ch_;
            ++_end;
        }

        void truncated_utf16()
        {
            throw std::range_error("Truncated UTF-16");
        }

        void invalid_utf16()
        {
            throw std::range_error("Invalid UTF-16");
        }
    };

    template<typename char_iterator, typename out_char>
    class basic_utf16_out_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = out_char;
        using difference_type =
            typename std::iterator_traits<char_iterator>::difference_type;
        using pointer =
            typename std::iterator_traits<char_iterator>::pointer;
        using reference =
            typename std::iterator_traits<char_iterator>::reference;

        basic_utf16_out_iterator() = default;

        explicit basic_utf16_out_iterator(const char_iterator& it_,
            const char_iterator& end_) :
            _it(it_)
        {
            if (it_ != end_)
            {
                next();
            }
        }

        out_char operator *() const
        {
            return _chars[_index];
        }

        bool operator ==(const basic_utf16_out_iterator& rhs_) const
        {
            return _it == rhs_._it;
        }

        bool operator !=(const basic_utf16_out_iterator& rhs_) const
        {
            return _it != rhs_._it;
        }

        basic_utf16_out_iterator& operator ++()
        {
            ++_index;

            if (_index >= _count)
            {
                ++_it;
                next();
            }

            return *this;
        }

        basic_utf16_out_iterator operator ++(int)
        {
            basic_utf16_out_iterator temp_ = *this;

            ++_index;

            if (_index >= _count)
            {
                ++_it;
                next();
            }

            return temp_;
        }

    private:
        char_iterator _it = char_iterator();
        out_char _chars[2]{};
        unsigned char _count = 0;
        unsigned char _index = 0;

        void next()
        {
            const std::size_t ch_ = *_it;

            _count = len(ch_);
            _index = 0;

            switch (_count)
            {
            case 1:
                _chars[0] = static_cast<out_char>(ch_);
                break;
            case 2:
                _chars[0] = static_cast<out_char>((ch_ >> 10) + 0xdc00u -
                    (0x10000 >> 10));
                _chars[1] = static_cast<out_char>((ch_ & 0x3ff) + 0xdc00u);
                break;
            default:
                break;
            }
        }

        char len(const std::size_t ch_) const
        {
            return ch_ > 0xffff ? 2 : 1;
        }
    };

    template<typename char_iterator>
    class basic_flip_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type =
            typename std::iterator_traits<char_iterator>::value_type;
        using difference_type =
            typename std::iterator_traits<char_iterator>::difference_type;
        using pointer =
            typename std::iterator_traits<char_iterator>::pointer;
        using reference =
            typename std::iterator_traits<char_iterator>::reference;

        basic_flip_iterator() = default;

        explicit basic_flip_iterator(const char_iterator& it_) :
            _it(it_)
        {
        }

        value_type operator *() const
        {
            value_type val_ = *_it;
            auto first_ = reinterpret_cast<char*>(&val_);
            auto second_ = reinterpret_cast<char*>(&val_ + 1) - 1;

            for (; first_ < second_; ++first_, --second_)
            {
                std::swap(*first_, *second_);
            }

            return val_;
        }

        bool operator ==(const basic_flip_iterator& rhs_) const
        {
            return _it == rhs_._it;
        }

        bool operator !=(const basic_flip_iterator& rhs_) const
        {
            return _it != rhs_._it;
        }

        basic_flip_iterator& operator ++()
        {
            ++_it;
            return *this;
        }

        basic_flip_iterator operator ++(int)
        {
            basic_flip_iterator temp_ = *this;

            ++_it;
            return temp_;
        }

    private:
        char_iterator _it = char_iterator();
    };

    using cutf8_in_utf32_out_iterator =
        basic_utf8_in_iterator<const char*, char32_t>;
    using cutf16_in_utf32_out_iterator =
        basic_utf16_in_iterator<const char16_t*, char32_t>;
    using cwchar_t_in_utf32_out_iterator =
        basic_utf16_in_iterator<const wchar_t*, char32_t>;
    using cutf8_in_utf16_out_iterator =
        basic_utf16_out_iterator<cutf8_in_utf32_out_iterator, char16_t>;
    using cutf8_in_wchar_t_out_iterator =
        basic_utf16_out_iterator<cutf8_in_utf32_out_iterator, wchar_t>;
    using cutf16_in_utf8_out_iterator =
        basic_utf8_out_iterator<cutf16_in_utf32_out_iterator>;
    using cwchar_t_in_utf8_out_iterator =
        basic_utf8_out_iterator<cwchar_t_in_utf32_out_iterator>;
}

#endif
