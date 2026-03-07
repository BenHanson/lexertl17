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

The same thing using lexertl::iterator:

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

The same thing using lexertl::range:

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

### Use lexertl::replace()

Note the use of a `skip()` rule to avoid replacing everything

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

Replace using a map of tokens:

(there is no need for a skip() rule in this case)

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

## More examples and documentation

See http://www.benhanson.net/lexertl.html for full documentation and more usage examples.
