#define TM_XML_IMPLEMENTATION
#include "main.h"

#include <cstdio>
#include <string>

int main(int argc, char const *argv[]) {
    static char const* const token_strings[] = {
        "tmxml_tok_eof",
        "tmxml_tok_error",
        "tmxml_tok_tag_open",
        "tmxml_tok_tag_close",
        "tmxml_tok_attribute_name",
        "tmxml_tok_attribute_value",
        "tmxml_tok_contents",
        "tmxml_tok_contents_cdata",
        "tmxml_tok_comment",
        "tmxml_tok_processing_instructions"
    };

    const char* test_xml = R"asd(<?xml version="1.1" encoding="utf-8" standalone="yes"?>
        <!DOCTYPE some_bullshit>
        <root>
            <haha>asd</haha>
            <another><![CDATA[SOME   RAW DATA   ]]></another>
            <another><![CDATA[   ]]></another>
            <?hasdha asodjaojda ogaoh?>
            <?hasdha asodjaojda ogaoh?>
            <?hasdha asodjaojda ogaoh?>
            <hehe with_some_attr="asdasd&quot;"> </hehe>
            <asd />
            &#97;
            &#x40;  asdasd as
        </root>
    )asd";

    std::string file_data;
    if (argc <= 1) {
        file_data = test_xml;
    } else {
        auto f = fopen(argv[1], "rb");
        if (!f) return -1;
        char buffer[1024];
        for (;;) {
            size_t read = fread(buffer, sizeof(char), 1024, f);
            if (!read) break;
            file_data.append(buffer, read);
        }
        fclose(f);
    }

    tmxml_reader reader = tmxml_make_reader(file_data.c_str(), tmxml_flags_trim_all);

    tmxml_prolog prolog = {};
    if (tmxml_read_prolog(&reader, &prolog)) {
        if (prolog.present) {
            printf("xml-prolog:\n");
            printf("  version: %.*s\n", (int)prolog.version.size, prolog.version.data);
            if (prolog.encoding.size) printf("  encoding: %.*s\n", (int)prolog.encoding.size, prolog.encoding.data);
            if (prolog.standalone_specified) printf("  standalone: %s\n", prolog.standalone ? "yes" : "no");
        }
        while (tmxml_next_token(&reader)) {
            auto tok = reader.current;
            printf("%s(%d:%d): %.*s\n", token_strings[tok.type], tok.location.line + 1, tok.location.column + 1,
                   (int)tok.contents.size, tok.contents.data);
        }
    } else {
        printf("XML PROLOG ERROR\n");
    }
    tmxml_destroy_reader(&reader);

    if (reader.context == tmxml_rc_error) {
        printf("ERROR\n");
        return -1;
    }
    printf("SUCCESS\n");
    return 0;
}
