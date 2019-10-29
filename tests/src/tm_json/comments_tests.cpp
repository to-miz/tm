TEST_CASE("comments") {
    SUBCASE("single_line") {
        const char* json = R"(
            // Hello
            //
            // Hello
            //
            ////// //
            {
                // Hello
                "test"
                // Hello
                //
                // Hello
                //
                ////// //
                :
                // Hello
                //
                // Hello
                //
                ////// //
                {
                    // Hello
                }
                // Hello
                //
                // Hello
                //
                ////// //
                ,
                //
                // Hello
                //
                ////// //
                "test"
                // Hello
                //
                // Hello
                //
                ////// //
                :
                // Hello
                //
                // Hello
                //
                ////// //
                [
                    // Hello
                    //
                    // Hello
                    //
                    ////// //
                ]
                // Hello
                //
                // Hello
                //
                ////// //
            }
            // Hello
            //
            // Hello
            //
            ////// //
        )";
        CHECK(check_json(json, JSON_READER_SINGLE_LINE_COMMENTS));
        CHECK(check_json(json, JSON_READER_JSON5));
        CHECK(check_json(json, JSON_READER_STRICT) == false);
    }
    SUBCASE("block") {
        const char* json = R"(
            /* Hello */
            /* Hello */
            /**/
            /**//**//**/
            /*/*/
            /**/ { /**/
                /* Hello */
                "test"
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                :
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                /**/ { /**/
                    /* Hello */
                    /* Hello */
                    /**/
                    /**//**//**/
                    /*/*/
                /**/ } /**/
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                /**/ , /**/
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                "test"
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                /**/ : /**/
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
                /**/ [ /**/
                    /* Hello */
                    /* Hello */
                    /**/
                    /**//**//**/
                    /*/*/
                /**/ ] /**/
                /* Hello */
                /* Hello */
                /**/
                /**//**//**/
                /*/*/
            /**/ } /**/
            /* Hello */
            /* Hello */
            /**/
            /**//**//**/
            /*/*/
        )";
        CHECK(check_json_ex(json, JSON_READER_BLOCK_COMMENTS));
        CHECK(check_json_ex(json, JSON_READER_JSON5));
        CHECK(check_json(json, JSON_READER_STRICT) == false);
    }
    SUBCASE("python") {
        const char* json = R"(
            # Hello
            # Hello
            ##
            ###
            {
                # Hello
                "test"
                # Hello
                # Hello
                ##
                ###
                :
                # Hello
                # Hello
                ##
                ###
                {
                    # Hello
                    # Hello
                    ##
                    ###
                }
                # Hello
                # Hello
                ##
                ###
                ,
                # Hello
                # Hello
                ##
                ###
                "test"
                # Hello
                # Hello
                ##
                ###
                :
                # Hello
                # Hello
                ##
                ###
                [
                    # Hello
                    # Hello
                    ##
                    ###
                ]
                # Hello
                # Hello
                ##
                ###
            }
            # Hello
            # Hello
            ##
            ###
        )";
        CHECK(check_json_ex(json, JSON_READER_PYTHON_COMMENTS));
        CHECK(check_json_ex(json, JSON_READER_JSON5 | JSON_READER_PYTHON_COMMENTS));
        CHECK(check_json(json, JSON_READER_STRICT) == false);
    }
    SUBCASE("mixed") {
        const char* json = R"(
            # Hello
            // Hello
            /* Hello */
            # Hello
            // Hello
            /* Hello */
            ##
            //
            /**/
            ###
            ////// //
            /**//**//**/
            /*/*/
            {
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                "test"
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                :
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                {
                    # Hello
                    // Hello
                    /* Hello */
                    # Hello
                    // Hello
                    /* Hello */
                    ##
                    //
                    /**/
                    ###
                    ////// //
                    /**//**//**/
                    /*/*/
                }
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                ,
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                "test"
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                :
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
                [
                    # Hello
                    // Hello
                    /* Hello */
                    # Hello
                    // Hello
                    /* Hello */
                    ##
                    //
                    /**/
                    ###
                    ////// //
                    /**//**//**/
                    /*/*/
                ]
                # Hello
                // Hello
                /* Hello */
                # Hello
                // Hello
                /* Hello */
                ##
                //
                /**/
                ###
                ////// //
                /**//**//**/
                /*/*/
            }
            # Hello
            // Hello
            /* Hello */
            # Hello
            // Hello
            /* Hello */
            ##
            //
            /**/
            ###
            ////// //
            /**//**//**/
            /*/*/
        )";
        CHECK(check_json_ex(
            json, JSON_READER_SINGLE_LINE_COMMENTS | JSON_READER_BLOCK_COMMENTS | JSON_READER_PYTHON_COMMENTS));
        CHECK(check_json_ex(json, JSON_READER_JSON5 | JSON_READER_PYTHON_COMMENTS));
        CHECK(check_json(json, JSON_READER_STRICT) == false);
    }
}