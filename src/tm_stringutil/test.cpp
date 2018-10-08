#include <string_view>
using std::string_view;

#if 1
#define TM_STRING_VIEW string_view
#define TM_STRING_VIEW_DATA(str) (str).data()
#define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
#endif

#define TM_STRINGUTIL_IMPLEMENTATION
#include "main.cpp"

#include <cstdio>

auto my_begin(string_view v) { return v.data(); }
auto my_end(string_view v) { return v.data() + v.size(); }

int main() {
    auto str = "- This, a sample string.";
    auto delimeters = " ,.-";

    // nullterminated version
    {
        auto tokenizer = tmsu_make_tokenizer(str);
        tmsu_stringview view = {};
        while (tmsu_next_token(&tokenizer, delimeters, &view)) {
            printf("%.*s\n", (int)view.size, view.data);
        }
    }

    // iterator version
    {
        auto str_first = str;
        auto str_last = str_first + strlen(str_first);
        auto delimeters_first = delimeters;
        auto delimeters_last = delimeters_first + strlen(delimeters_first);
        auto tokenizer = tmsu_make_tokenizer_n(str_first, str_last);
        tmsu_stringview view = {};
        while (tmsu_next_token_n(&tokenizer, delimeters_first, delimeters_last, &view)) {
            printf("%.*s\n", (int)view.size, view.data);
        }
    }

    {
    	string_view p = "This";
    	string_view f = "This";

    	auto first = tmsu_find_n(my_begin(p), my_end(p), my_begin(f), my_end(f));
    	if(first != my_end(p)) {
    		printf("%.*s\n", (int)(my_end(p) - first), first);
    	}

    	auto last = tmsu_find_last_n(my_begin(p), my_end(p), my_begin(f), my_end(f));
    	if(last != my_end(p)) {
    		printf("%.*s\n", (int)(my_end(p) - last), last);
    	}
    }

    return 0;
}