#include <string_view>
using std::string_view;

#define TM_STRING_VIEW string_view
#define TM_STRING_VIEW_DATA(str) (str).data()
#define TM_STRING_VIEW_SIZE(str) ((tm_size_t)(str).size())
#define TM_STRING_VIEW_MAKE(data, size) string_view{data, (size_t)(size)}

#define TM_STRINGUTIL_IMPLEMENTATION
#include "main.cpp"

#include <cstdio>

auto my_begin(string_view v) { return v.data(); }
auto my_end(string_view v) { return v.data() + v.size(); }

int main() {
    auto str = "- This, a sample string.";
    auto delimiters = " ,.-";

    // nullterminated version
    {
        auto tokenizer = tmsu_tokenizer(str);
        tmsu_view_t view = {};
        while (tmsu_next_token(&tokenizer, delimiters, &view)) {
            printf("%.*s\n", (int)tmsu_view_size(view), tmsu_view_data(view));
        }
    }

    // iterator version
    {
        auto str_view = tmsu_view(str);
        auto delimiters_view = tmsu_view(delimiters);
        auto tokenizer = str_view;
        tmsu_view_t view = {};
        while (tmsu_next_token_v(&tokenizer, delimiters_view, &view)) {
            printf("%.*s\n", (int)tmsu_view_size(view), tmsu_view_data(view));
        }
    }

    {
        string_view p = "Hello";
        string_view f = "Hello";

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