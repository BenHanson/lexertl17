lexertl17: The Modular Lexical Analyser Generator
=======

lexertl is a header-only library for writing lexical analysers. With lexertl you can:

- Build lexical analysers at runtime
- Scan Unicode and ASCII input
- Scan from files or memory
- Generate C++ code or even write your own code generator

### Construct a Lexer and Tokenise input

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/lookup.hpp>
#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::smatch results(input.begin(), input.end());

    // Read ahead
    lexertl::lookup(sm, results);

    while (results.id != 0)
    {
        std::cout << "Id: " << results.id << ", Token: '" <<
            results.view() << "'\n";
        lexertl::lookup(sm, results);
    }

    return 0;
}
```

#### The same thing using `lexertl::iterator`:

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/iterator.hpp>
#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::siterator iter(input.begin(), input.end(), sm);
    lexertl::siterator end;

    for (; iter != end; ++iter)
    {
        std::cout << "Id: " << iter->id << ", Token: '" <<
            iter->view() << "'\n";
    }

    return 0;
}
```

#### The same thing using `lexertl::range`:

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/iterator.hpp>
#include <lexertl/range.hpp>

#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::siterator iter(input.begin(), input.end(), sm);
    lexertl::siterator end;
    lexertl::range range(iter, end);

    for (const auto& results : range)
    {
        std::cout << "Id: " << results.id << ", Token: '" <<
            results.view() << "'\n";
    }

    return 0;
}
```

### Search Text With a Lexer

All that is required to allow a lexer to search is to add a simple `skip()` rule:

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/iterator.hpp>
#include <lexertl/range.hpp>

#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("(?s:.)", lexertl::rules::skip());
    lexertl::generator::build(rules, sm);

    std::string input("abc012Ad3e4");
    lexertl::siterator iter(input.begin(), input.end(), sm);
    lexertl::siterator end;
    lexertl::range range(iter, end);

    for (const auto& results : range)
    {
        std::cout << "Id: " << results.id << ", Token: '" <<
            results.view() << "'\n";
    }

    return 0;
}
```

You can increase the sophistication of the search by completely ignoring comments (for example) by adding the following rule:

```cpp
    rules.push(R"("//".*|"/*"(?s:.)*?"*/")", lexertl::rules::skip());
```

You can exclude strings with the following rules:

```cpp
    // Exclude instances of '"'
    rules.push(R"('([^'\\\r\n]|\\.)*')", lexertl::rules::skip());
    rules.push(R"(\"([^"\\\r\n]|\\.)*\")", lexertl::rules::skip());
    rules.push(R"(R\"\((?s:.)*?\)\")", lexertl::rules::skip());
```

### Use `lexertl::replace()`

Note the use of a `skip()` rule to avoid replacing characters that do not match the other rules.

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/replace.hpp>

#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    rules.push("(?s:.)", lexertl::rules::skip());
    lexertl::generator::build(rules, sm);

    std::string input("abc 012 A d 3 e 4");

    std::cout<< lexertl::replace(input, sm, "rep") << '\n';
    return 0;
}
```

Outputs `rep rep A rep rep rep rep`

#### Replace using a map of tokens:

(The `skip()` rule is not strictly necessary here but it is more efficient to include it)

```cpp
#include <lexertl/generator.hpp>
#include <lexertl/replace.hpp>

#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    rules.push("(?s:.)", lexertl::rules::skip());
    lexertl::generator::build(rules, sm);

    std::string input("abc 012 A d 3 e 4");

    std::cout<< lexertl::replace(input, sm,
        {
            {1, "number"},
            {2, "word"}
        }) << '\n';
    return 0;
}
```

Outputs `word number A word number word number`

#### Replace using a map of functions:


```cpp
#include <lexertl/generator.hpp>
#include <lexertl/replace.hpp>

#include <iostream>

int main()
{
    lexertl::rules rules;
    lexertl::state_machine sm;

    rules.push("[0-9]+", 1);
    rules.push("[a-z]+", 2);
    rules.push("(?s:.)", lexertl::rules::skip());
    lexertl::generator::build(rules, sm);

    std::string input("abc 012 A d 3 e 4");

	std::map<uint16_t, std::string (*)(std::string::const_iterator&,
		std::string::const_iterator&)> map =
	{
		{
            (uint16_t)1, [](std::string::const_iterator& first, std::string::const_iterator& second)
                {
                    return "Number: " + std::string(first, second);
                }
        },
        {
            (uint16_t)2, [](std::string::const_iterator& first, std::string::const_iterator& second)
                {
                    return "Word: " + std::string(first, second);
                }
        }
	};

	lexertl::replace(std::ostreambuf_iterator<char>(std::cout),
		input.begin(), input.end(), sm, map);
	std::cout << '\n';
    return 0;
}
```

Outputs `Word: abc Number: 012 A Word: d Number: 3 Word: e Number: 4`

## More examples and documentation

See http://www.benhanson.net/lexertl.html for full documentation and more usage examples.
