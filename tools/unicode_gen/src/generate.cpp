struct static_string {
    char data[100];
    size_t size;
};
static static_string get_type_from_size(ucd_min_sizes::pair pair) {
    assert(pair.size > 0);
    static_string result = {};
    auto written = snprintf(result.data, std::size(result.data), "%sint%d_t", (pair.is_signed) ? "" : "u", pair.size);
    assert(written > 0 && (size_t)written < std::size(result.data));
    if (written <= 0 || (size_t)written >= std::size(result.data)) fatal_error("Internal error.");
    result.data[written] = 0;
    result.size = (size_t)written;
    return result;
}

void generate_multistage_table_based(const unique_ucd& ucd, size_t pruned_stage_one_size, const char* prefix, FILE* f) {
    assert(f);
    auto flags = ucd.flags;

    // Generate stage one table.
    auto print_block = [](FILE* f, const int32_t* values, size_t count, bool last) {
        for (size_t i = 0; i + 16 <= count; i += 16) {
            fprintf(f, "    %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d%s\n", values[i + 0],
                    values[i + 1], values[i + 2], values[i + 3], values[i + 4], values[i + 5], values[i + 6],
                    values[i + 7], values[i + 8], values[i + 9], values[i + 10], values[i + 11], values[i + 12],
                    values[i + 13], values[i + 14], values[i + 15], (last && i + 16 == count) ? "" : ",");
        }
        if (count % 16) {
            fprintf(f, "    ");
            // clang-format off
            switch (count % 16) {
                case 15: fprintf(f, "%3d,", values[count - 15]); // Fallthrough
                case 14: fprintf(f, "%3d,", values[count - 14]); // Fallthrough
                case 13: fprintf(f, "%3d,", values[count - 13]); // Fallthrough
                case 12: fprintf(f, "%3d,", values[count - 12]); // Fallthrough
                case 11: fprintf(f, "%3d,", values[count - 11]); // Fallthrough
                case 10: fprintf(f, "%3d,", values[count - 10]); // Fallthrough
                case 9:  fprintf(f, "%3d,", values[count -  9]); // Fallthrough
                case 8:  fprintf(f, "%3d,", values[count -  8]); // Fallthrough
                case 7:  fprintf(f, "%3d,", values[count -  7]); // Fallthrough
                case 6:  fprintf(f, "%3d,", values[count -  6]); // Fallthrough
                case 5:  fprintf(f, "%3d,", values[count -  5]); // Fallthrough
                case 4:  fprintf(f, "%3d,", values[count -  4]); // Fallthrough
                case 3:  fprintf(f, "%3d,", values[count -  3]); // Fallthrough
                case 2:  fprintf(f, "%3d,", values[count -  2]); // Fallthrough
                case 1:  fprintf(f, "%3d%s\n", values[count -  1], (last) ? "" : ","); break;
            }
            // clang-format on
        }
    };

    size_t stage_one_size = ucd.stage_one.size();
    if (flags & generate_flags_prune_stage_one) stage_one_size = pruned_stage_one_size;

    auto stage_one_table_type = get_type_from_size(ucd.min_sizes.stage_one_table);
    auto stage_two_type = get_type_from_size(ucd.min_sizes.stage_two);

    fprintf(f,
            "/* Unicode data stage one: %zu bytes. */\n"
            "static const size_t %sucd_stage_one_size = %zu;\n"
            "static const %s %sucd_stage_one[%zu] = {\n",
            stage_one_size * (ucd.min_sizes.stage_one_table.size / 8), prefix, stage_one_size,
            stage_one_table_type.data, prefix, stage_one_size);
    print_block(f, ucd.stage_one.data(), stage_one_size, /*last*/true);
    fprintf(f, "};\n\n");

    // Generate stage two table.
    size_t stage_two_size = ucd.stage_two.size() * block_size;
    if (flags & generate_flags_prune_stage_two) {
        stage_two_size = ucd.pruned_stage_two_start * block_size;
    }
    fprintf(f,
            "/* Unicode data stage two: %zu bytes.*/\n"
            "static const uint32_t %sucd_block_size = %u;\n"
            "static const uint32_t %sucd_stage_two_blocks_count = %zu;\n"
            "static const size_t %sucd_stage_two_size = %zu;\n"
            "static const %s %sucd_stage_two[%zu] = {\n",
            stage_two_size * (ucd.min_sizes.stage_two.size / 8), prefix, block_size, prefix, ucd.stage_two.size(),
            prefix, stage_two_size, stage_two_type.data, prefix, stage_two_size);
    {
        size_t block_index = 0;
        for (auto& block : ucd.stage_two) {
            if (block.ranges.count) continue;
            fprintf(f, "    /* Block %zu */\n", block_index);
            print_block(f, block.offset, block_size, block_index + 1 == ucd.stage_two.size());
            ++block_index;
        }
    }
    fprintf(f, "};\n\n");
}

#if 0
void generate_linear(const vector<data_entry>& raw, const unique_ucd& ucd, FILE* f, size_t first, size_t last) {
    auto gb = grapheme_break_other;
    uint32_t last_cp = raw[first].data->cp;
    while (first < last) {
        auto data = raw[first].data;
        if (data->grapheme_break != gb) {
            if (data->cp - last_cp > 0) {
                fprintf(f, "                if (cp < %u) return %s;\n", data->cp, to_string(gb));
                last_cp = data->cp;
            }
            gb = data->grapheme_break;
        }
        ++first;
    }
#if 0
    auto data = raw[first].data;
    if (first - first > 0) {
        fprintf(f, "                if (cp < %u) return %s;\n", data->cp, to_string(data->grapheme_break));
        first = first;
    }
#endif
}

void generate_binary_search_based(const vector<data_entry>& raw, const unique_ucd& ucd, FILE* f) {
    assert(f);
    assert(!ucd.entries.empty());

    auto flags = ucd.flags;
    auto size = raw.size();

    if (flags & generate_flags_grapheme_break) {
        fprintf(f, "ucd_grapheme_break_enum get_grapheme_break_property(uint32_t codepoint) {\n");

        size_t min_range = size / 8;

        // clang-format off
        fprintf( f, "    if (cp < %u) {\n", raw[size / 2].cp);

            fprintf( f, "        if (cp < %u) {\n", raw[size / 4].cp);

                fprintf( f, "            if (cp < %u) {\n", raw[min_range * 1].cp);
                    generate_linear(raw, ucd, f, min_range * 0, min_range * 1);
                fprintf( f, "            } else {\n");
                    generate_linear(raw, ucd, f, min_range * 1, min_range * 2);
                fprintf( f, "            }\n");

            fprintf( f, "        } else {\n");

                fprintf( f, "            if (cp < %u) {\n", raw[min_range * 3].cp);
                    generate_linear(raw, ucd, f, min_range * 2, min_range * 3);
                fprintf( f, "            } else {\n");
                    generate_linear(raw, ucd, f, min_range * 3, min_range * 4);
                fprintf( f, "            }\n");

            fprintf( f, "        }\n");

        fprintf( f, "    } else {\n");

            fprintf( f, "        if (cp < %u) {\n", raw[size / 4 + size / 2].cp);

                fprintf( f, "            if (cp < %u) {\n", raw[min_range * 5].cp);
                    generate_linear(raw, ucd, f, min_range * 4, min_range * 5);
                fprintf( f, "            } else {\n");
                    generate_linear(raw, ucd, f, min_range * 5, min_range * 6);
                fprintf( f, "            }\n");

            fprintf( f, "        } else {\n");

                fprintf( f, "            if (cp < %u) {\n", raw[min_range * 7].cp);
                    generate_linear(raw, ucd, f, min_range * 6, min_range * 7);
                fprintf( f, "            } else {\n");
                    generate_linear(raw, ucd, f, min_range * 7, min_range * 8);
                fprintf( f, "            }\n");

            fprintf( f, "        }\n");

        fprintf( f, "    }\n");
        // clang-format on

        fprintf(f, "    return grapheme_break_other;\n}\n\n");
    }
}
#endif

void generate_source_file(const parsed_data& parsed, const unique_ucd& ucd, const option_strings& strings, FILE* f) {
    assert(f);
    MAYBE_UNUSED(parsed);

    auto flags = ucd.flags;
    auto uppercase_prefix = strings.uppercase_prefix;
    auto prefix = strings.prefix;
    auto assert_name = strings.assert_name;

    auto count_codepoints = [](const vector<codepoint_run>& runs) {
        size_t count = 0;
        if (runs.size() > 1) {
            /* We only output data if there is more than just the default entry. */
            for (auto& run : runs) {
                count += (size_t)run.count + 1;  // + 1 Null terminator.
            }
        }
        return count;
    };

    auto default_block_pos = (flags & generate_flags_prune_stage_two) ? (int32_t)ucd.pruned_stage_two_start : 0;
    size_t pruned_stage_one_size = ucd.pruned_stage_one_size;

    size_t full_upper_offset = 0;
    size_t full_lower_offset = full_upper_offset + count_codepoints(ucd.full_upper);
    size_t full_title_offset = full_lower_offset + count_codepoints(ucd.full_lower);
    size_t full_case_fold_offset = full_title_offset + count_codepoints(ucd.full_title);
    size_t canonical_offset = full_case_fold_offset + count_codepoints(ucd.full_case_fold);
    size_t compatibility_offset = canonical_offset + count_codepoints(ucd.canonical);
    size_t codepoints_count = compatibility_offset + count_codepoints(ucd.compatibility);

    size_t stage_one_size = ucd.stage_one.size();
    if (flags & generate_flags_prune_stage_one) stage_one_size = pruned_stage_one_size;
    size_t stage_two_size = ucd.stage_two.size() * block_size;
    if (flags & generate_flags_prune_stage_two) {
        stage_two_size = ucd.pruned_stage_two_start * block_size;
    }
    size_t grapheme_break_transitions_size = 0;
    if (flags & generate_flags_grapheme_break) {
        grapheme_break_transitions_size = sizeof(grapheme_break_transitions) / sizeof(grapheme_break_transitions[0][0]);
    }

    auto ucd_entry_size = ucd.min_sizes.ucd_entry_size(flags);

    size_t overall_size = codepoints_count * (ucd.min_sizes.all_arrays.size / 8) + ucd.entries.size() * ucd_entry_size +
                          grapheme_break_transitions_size + stage_one_size * (ucd.min_sizes.stage_one_table.size / 8) +
                          stage_two_size * (ucd.min_sizes.stage_two.size / 8);
    fprintf(f,
            "/* This file was generated using tools/unicode_gen from\n"
            "   https://github.com/to-miz/tm. Do not modify by hand.\n"
            "   Around %zu bytes (%.2f kilobytes) of data for lookup tables\n"
            "   are generated. ",
            overall_size, overall_size / 1024.0);

    if (parsed.version.first < parsed.version.last) {
        fprintf(f, "It was generated using version %.*s of Unicode.", (int)(parsed.version.last - parsed.version.first),
                parsed.version.first);
    }

    fprintf(f,
            "*/\n\n"
            "#ifdef __cplusplus\n"
            "extern \"C\" {\n"
            "#endif\n\n");

    // Generate full uppercase, titlecase, lowercase, canonical and compatibility mapping array.
    auto print_runs = [](FILE* f, const char* name, const vector<codepoint_run>& runs, bool first) {
        if (runs.size() <= 1) return;

        fprintf(f, "%s    /* %s */", (first) ? "" : ",\n\n", name);
        for (size_t run_index = 0, runs_count = runs.size(); run_index < runs_count; run_index++) {
            auto& run = runs[run_index];
            int32_t count = run.count;
            if (count > 0) {
                fprintf(f, "\n    ");
                for (int32_t i = 0; i < count; i++) {
                    fprintf(f, "%d, ", run.codepoints[i]);
                }
                fprintf(f, "0%s", (run_index + 1 == runs_count) ? "" : ",");
            } else {
                fprintf(f, "\n    0%s", (run_index + 1 == runs_count) ? "" : ",");
            }
        }
    };

    auto stage_one_function_type = get_type_from_size(ucd.min_sizes.stage_one_function);
    auto stage_two_type = get_type_from_size(ucd.min_sizes.stage_two);

    if (ucd.min_sizes.all_arrays.size > 0) {
        fprintf(f, "/* Codepoint runs: %zu bytes. */\n", codepoints_count * (ucd.min_sizes.all_arrays.size / 8));

        if ((flags & generate_flags_full_case) || (flags & generate_flags_full_case_toggle)) {
            fprintf(f, "static const size_t %sfull_upper_offset = %zu;\n", prefix, full_upper_offset);
            fprintf(f, "static const size_t %sfull_lower_offset = %zu;\n", prefix, full_lower_offset);
            fprintf(f, "static const size_t %sfull_title_offset = %zu;\n", prefix, full_title_offset);
        }
        if (flags & generate_flags_full_case_fold) {
            fprintf(f, "static const size_t %sfull_case_fold_offset = %zu;\n", prefix, full_case_fold_offset);
        }
        if (flags & generate_flags_canonical) {
            fprintf(f, "static const size_t %scanonical_offset = %zu;\n", prefix, canonical_offset);
        }
        if (flags & generate_flags_compatibility) {
            fprintf(f, "static const size_t %scompatibility_offset = %zu;\n", prefix, compatibility_offset);
        }

        auto all_arrays_type = get_type_from_size(ucd.min_sizes.all_arrays);
        fprintf(f,
                "static const size_t %scodepoint_runs_size = %zu;\n"
                "static const %s %scodepoint_runs[%zu] = {\n",
                prefix, codepoints_count, all_arrays_type.data, prefix, codepoints_count);
        bool first = true;
        if ((flags & generate_flags_full_case) || (flags & generate_flags_full_case_toggle)) {
            print_runs(f, "Full uppercase entries.", ucd.full_upper, first);
            first = false;
            print_runs(f, "Full lowercase entries.", ucd.full_lower, false);
            print_runs(f, "Full titlecase entries.", ucd.full_title, false);
        }
        if (flags & generate_flags_full_case_fold) {
            print_runs(f, "Full case fold entries.", ucd.full_case_fold, first);
            first = false;
        }
        if (flags & generate_flags_canonical) {
            print_runs(f, "Canonical mapping entries.", ucd.canonical, first);
            first = false;
        }
        if (flags & generate_flags_compatibility) {
            print_runs(f, "Compatibility mapping entries.", ucd.compatibility, first);
            first = false;
        }
        fprintf(f, "\n};\n\n");
    }

    // Generate ucd entry struct.
    fprintf(f, "typedef struct {\n");
    if ((flags & generate_flags_category) || (flags & generate_flags_case_info)) {
        fprintf(f, "    uint8_t bits0;\n");
    }
    if ((flags & generate_flags_grapheme_break) || (flags & generate_flags_width)) {
        fprintf(f, "    uint8_t bits1;\n");
    }
    if (flags & generate_flags_full_case) {
        auto full_upper_index_type = get_type_from_size(ucd.min_sizes.full_upper_index);
        auto full_title_index_type = get_type_from_size(ucd.min_sizes.full_title_index);
        auto full_lower_index_type = get_type_from_size(ucd.min_sizes.full_lower_index);
        fprintf(f, "    %s full_upper_index;\n", full_upper_index_type.data);
        fprintf(f, "    %s full_title_index;\n", full_title_index_type.data);
        fprintf(f, "    %s full_lower_index;\n", full_lower_index_type.data);
    }
    if (flags & generate_flags_full_case_fold) {
        auto full_case_fold_index_type = get_type_from_size(ucd.min_sizes.full_case_fold_index);
        fprintf(f, "    %s full_case_fold_index;\n", full_case_fold_index_type.data);
    }
    if (flags & generate_flags_full_case_toggle) {
        auto full_case_toggle_index_type = get_type_from_size(ucd.min_sizes.full_case_toggle_index);
        fprintf(f, "    %s full_case_toggle_index;\n", full_case_toggle_index_type.data);
    }
    if ((flags & generate_flags_canonical) && ucd.min_sizes.canonical_index.size) {
        auto canonical_index_type = get_type_from_size(ucd.min_sizes.canonical_index);
        fprintf(f, "    %s canonical_index;\n", canonical_index_type.data);
    }
    if ((flags & generate_flags_compatibility) && ucd.min_sizes.compatibility_index.size) {
        auto compatibility_index_type = get_type_from_size(ucd.min_sizes.compatibility_index);
        fprintf(f, "    %s compatibility_index;\n", compatibility_index_type.data);
    }

    if (flags & generate_flags_simple_case) {
        auto simple_upper_offset_type = get_type_from_size(ucd.min_sizes.simple_upper_offset);
        auto simple_title_offset_type = get_type_from_size(ucd.min_sizes.simple_title_offset);
        auto simple_lower_offset_type = get_type_from_size(ucd.min_sizes.simple_lower_offset);
        fprintf(f, "    %s simple_upper_offset;\n", simple_upper_offset_type.data);
        fprintf(f, "    %s simple_title_offset;\n", simple_title_offset_type.data);
        fprintf(f, "    %s simple_lower_offset;\n", simple_lower_offset_type.data);
    }
    if (flags & generate_flags_simple_case_fold) {
        auto simple_case_fold_offset_type = get_type_from_size(ucd.min_sizes.simple_case_fold_offset);
        fprintf(f, "    %s simple_case_fold_offset;\n", simple_case_fold_offset_type.data);
    }
    if (flags & generate_flags_simple_case_toggle) {
        auto simple_case_toggle_offset_type = get_type_from_size(ucd.min_sizes.simple_case_toggle_offset);
        fprintf(f, "    %s simple_case_toggle_offset;\n", simple_case_toggle_offset_type.data);
    }
    if (flags & generate_flags_canonical) {
        auto simple_canonical_offset_type = get_type_from_size(ucd.min_sizes.simple_canonical_offset);
        fprintf(f, "    %s simple_canonical_offset;\n", simple_canonical_offset_type.data);
    }
    if (flags & generate_flags_compatibility) {
        auto simple_compatibility_offset_type = get_type_from_size(ucd.min_sizes.simple_compatibility_offset);
        fprintf(f, "    %s simple_compatibility_offset;\n", simple_compatibility_offset_type.data);
    }
    fprintf(f, "} %sucd_internal;\n\n", prefix);

    // Generate Unicode data entries.
    fprintf(f,
            "/* Unicode data entries: %zu bytes. */\n"
            "static const size_t %sucd_entries_size = %zu;\n"
            "static const %sucd_internal %sucd_entries[%zu] = {\n",
            ucd.entries.size() * ucd_entry_size, prefix, ucd.entries.size(), prefix, prefix, ucd.entries.size());
    const char* comma = ", ";
    const char* empty = "";
    for (size_t i = 0, count = ucd.entries.size(); i < count; i++) {
        auto& entry = ucd.entries[i];

        fprintf(f, "    {");
        bool first = true;

        if ((flags & generate_flags_category) || (flags & generate_flags_case_info)) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.flags);
            first = false;
        }
        if (flags & generate_flags_grapheme_break) {
            fprintf(f, "%s%d", (first) ? empty : comma, (int32_t)entry.data.grapheme_break);
            first = false;
        }
        if (flags & generate_flags_full_case) {
            fprintf(f, "%s%d, %d, %d", (first) ? empty : comma, entry.data.full_upper_index,
                    entry.data.full_title_index, entry.data.full_lower_index);
            first = false;
        }
        if (flags & generate_flags_full_case_fold) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.full_case_fold_index);
            first = false;
        }
        if (flags & generate_flags_full_case_toggle) {
            auto case_toggle_index = entry.data.full_case_toggle_index;
            if (case_toggle_index < 0) {
                // Negative value means index maps to lower case table.
                case_toggle_index = -case_toggle_index + (int32_t)full_lower_offset;
            }
            fprintf(f, "%s%d", (first) ? empty : comma, case_toggle_index);
            first = false;
        }

        if ((flags & generate_flags_canonical) && ucd.min_sizes.canonical_index.size) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.canonical_index);
            first = false;
        }
        if ((flags & generate_flags_compatibility) && ucd.min_sizes.compatibility_index.size) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.compatibility_index);
            first = false;
        }

        if (flags & generate_flags_simple_case) {
            fprintf(f, "%s%d, %d, %d", (first) ? empty : comma, entry.data.simple_upper_offset,
                    entry.data.simple_title_offset, entry.data.simple_lower_offset);
            first = false;
        }
        if (flags & generate_flags_simple_case_fold) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.simple_case_fold_offset);
            first = false;
        }
        if (flags & generate_flags_simple_case_toggle) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.simple_case_toggle_offset);
            first = false;
        }
        if (flags & generate_flags_canonical) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.simple_canonical_offset);
            first = false;
        }
        if (flags & generate_flags_compatibility) {
            fprintf(f, "%s%d", (first) ? empty : comma, entry.data.simple_compatibility_offset);
            first = false;
        }

        fprintf(f, "}%s\n", (i + 1 == count) ? empty : ",");
    }
    fprintf(f, "};\n\n");

    // NOTE: Binary search based code gen not implemented.
    /*if (flags & generate_flags_binary_search) {
        generate_binary_search_based(raw, ucd, f);
    } else*/
    { generate_multistage_table_based(ucd, pruned_stage_one_size, prefix, f); }

    // Generate grapheme break transition table.
    if (flags & generate_flags_grapheme_break) {
        assert(grapheme_break_transitions_size > 0);
        fprintf(f,
                "/*\nGrapheme cluster break transition table.\n"
                "The grapheme cluster break rules are embedded in a 16x16 state machine\n"
                "transition table, denoting whether we can break when going from one grapheme\n"
                "break type to another. The values are made up of a MUST_BREAK bit (highest bit)\n"
                "and the state to which the state machine transitions to in case no break is\n"
                "allowed. The state machine will consume codepoints, until a grapheme cluster\n"
                "break is found.\n"
                "See https://unicode.org/reports/tr29/#Grapheme_Cluster_Boundary_Rules\n"
                "for more information.\n*/\n");
        fprintf(f,
                "/* Unicode grapheme cluster break transition table: %zu bytes. */\n"
                "static const size_t %sgrapheme_break_transitions_size = %zu;\n"
                "static const uint8_t %sgrapheme_break_transitions[%zu] = {\n",
                sizeof(grapheme_break_transitions), prefix, grapheme_break_transitions_size, prefix,
                grapheme_break_transitions_size);
        for (int32_t from = 0; from < grapheme_entries; from++) {
            fprintf(f, "    ");
            for (int32_t to = 0; to < grapheme_entries - 1; to++) {
                fprintf(f, "%3u,", grapheme_break_transitions[from][to]);
            }
            fprintf(f, "%3u%s\n", grapheme_break_transitions[from][grapheme_entries - 1],
                    (from + 1 == grapheme_entries) ? "" : ", ");
        }
        fprintf(f, "};\n\n");
    }

    // Generate stage one accessor function.
    fprintf(f, "static %s %sget_stage_one_value_internal(uint32_t index) {\n", stage_one_function_type.data, prefix);
    bool all_are_default = true;
    for (size_t i = pruned_stage_one_size, count = ucd.stage_one.size(); i < count; i++) {
        if (ucd.stage_one[i] != default_block_pos) {
            all_are_default = false;
            break;
        }
    }
    if (flags & generate_flags_handle_invalid_codepoints) {
        if (!(flags & generate_flags_prune_stage_one) || !all_are_default) {
            fprintf(f, "    if (index >= %zu) return %d;\n", ucd.stage_one.size(), default_block_pos);
        }
    } else if (assert_name) {
        fprintf(f, "    %s(index < %zu);\n", assert_name, ucd.stage_one.size());
    }
    if (flags & generate_flags_prune_stage_one) {
        if (all_are_default) {
            fprintf(f,
                    "    if (index < %zu) return %sucd_stage_one[index];\n"
                    "    return %d;\n",
                    pruned_stage_one_size, prefix, default_block_pos);
        } else {
            struct range_map_entry {
                int32_t first;
                int32_t last;
                int32_t mapped;
            };
            struct outlier {
                int32_t index;
                int32_t mapped;
            };

            vector<range_map_entry> ranges;
            vector<outlier> outliers;

            int32_t current = (int32_t)pruned_stage_one_size;
            int32_t count = (int32_t)ucd.stage_one.size();
            while (current < count) {
                int32_t first = current;
                int32_t last = first + 1;
                int32_t mapped = ucd.stage_one[current];
                ++current;
                if (mapped == default_block_pos) continue;
                while (current < count && ucd.stage_one[current] == mapped) {
                    last = current + 1;
                    ++current;
                }

                int32_t width = last - first;
                if (width < 5) {
                    for (int32_t i = first; i < last; ++i) {
                        outliers.push_back({i, mapped});
                    }
                } else {
                    ranges.push_back({first, last, mapped});
                }
            }

            /* Add outliers back into ranges they split.
               Since we check outliers first before checking ranges,
               this doesn't break the range checks. */
            for (size_t i = 0; i + 1 < ranges.size();) {
                auto cur = &ranges[i];
                auto next = &ranges[i + 1];
                if (cur->mapped == next->mapped) {
                    bool gap_is_full_with_outliers = true;
                    for (auto gap = cur->last; gap < next->first; ++gap) {
                        bool found = false;
                        for (auto& entry : outliers) {
                            if (entry.index == gap) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            gap_is_full_with_outliers = false;
                            break;
                        }
                    }

                    if (gap_is_full_with_outliers) {
                        cur->last = next->last;
                        ranges.erase(ranges.begin() + i + 1);
                        continue;
                    }
                }
                ++i;
            }

            fprintf(f, "    if (index < %zu) return %sucd_stage_one[index];\n", pruned_stage_one_size, prefix);
            if (!outliers.empty()) {
                fprintf(f, "    switch (index) {\n");
                for (auto& entry : outliers) {
                    fprintf(f, "        case %d: return %d;\n", entry.index, entry.mapped);
                }
                fprintf(f, "    }\n");
            }

            for (auto& entry : ranges) {
                fprintf(f, "    if (index >= %d && index < %d) return %d;\n", entry.first, entry.last, entry.mapped);
            }

            fprintf(f, "    return %d;\n", default_block_pos);
        }
    } else {
        fprintf(f, "    return %sucd_stage_one[index];\n", prefix);
    }
    fprintf(f, "}\n\n");

    // Generate stage two accessor function.
    fprintf(f,
            "static %s %sget_stage_two_value_internal(uint32_t block_index,\n"
            "                uint32_t offset) {\n",
            stage_two_type.data, prefix);
    if (assert_name) {
        fprintf(f,
                "    %s(block_index < %sucd_stage_two_blocks_count);\n"
                "    %s(offset < %d);\n",
                assert_name, prefix, assert_name, block_size);
    }
    if (flags & generate_flags_prune_stage_two) {
        auto print_range = [](int32_t first, int32_t last, int32_t a, int32_t b, FILE* f) {
            if (last - first == 1) {
                fprintf(f, " return (offset == %d) ? %d : %d;\n", first, a, b);
            } else if (first == 0) {
                fprintf(f, " return (offset < %d) ? %d : %d;\n", last, a, b);
            } else if (last == block_size) {
                fprintf(f, " return (offset >= %d) ? %d : %d;\n", first, a, b);
            } else {
                fprintf(f, " return (offset >= %d && offset < %d) ? %d : %d;\n", first,
                        last, a, b);
            }
        };

        auto start = (int32_t)ucd.pruned_stage_two_start;
        fprintf(f,
                "    if(block_index < %d) {\n"
                "        return %sucd_stage_two[block_index * %d + offset];\n"
                "    }\n"
                "    switch (block_index - %d) {\n",
                start, prefix, block_size, start);
        for (size_t i = 0, count = ucd.stage_two.size(); i < count; i++) {
            auto& block = ucd.stage_two[i];
            if (block.redirect_index < start) continue;
            auto index = block.redirect_index - start;
            switch (block.ranges.count) {
                case 1: {
                    fprintf(f, "        case %d: return %d;\n", index, block.ranges.entries[0].value);
                    break;
                }
                case 2: {
                    fprintf(f, "        case %d:", index);
                    print_range(block.ranges.entries[0].first, block.ranges.entries[0].last,
                                block.ranges.entries[0].value, block.ranges.entries[1].value, f);
                    break;
                }
                case 3: {
                    fprintf(f, "        case %d:", index);
                    print_range(block.ranges.entries[1].first, block.ranges.entries[1].last,
                                block.ranges.entries[1].value, block.ranges.entries[0].value, f);
                    break;
                }
                case 4: {
                    int32_t first = 0;
                    int32_t second = 0;
                    int32_t a = 0;
                    int32_t b = 0;
                    if (block.ranges.entries[0].length() == 1) {
                        first = block.ranges.entries[0].first;
                        second = block.ranges.entries[2].first;
                        a = block.ranges.entries[0].value;
                        b = block.ranges.entries[1].value;
                    } else {
                        first = block.ranges.entries[1].first;
                        second = block.ranges.entries[3].first;
                        a = block.ranges.entries[1].value;
                        b = block.ranges.entries[0].value;
                    }
                    fprintf(f,
                            "        case %d: return (offset == %d || offset == %d) ? %d : %d;\n",
                            index, first, second, a, b);
                    break;
                }
                case 5: {
                    fprintf(f,
                            "        case %d: return (offset == %d || offset == %d) ? %d : %d;\n",
                            index, block.ranges.entries[1].first, block.ranges.entries[3].first,
                            block.ranges.entries[1].value, block.ranges.entries[0].value);
                    break;
                }
                default: {
                    FPRINTF(stderr, TEXT("Internal error."));
                    exit(-1);
                    break;
                }
            }
        }
        fprintf(f, "        default: return 0;\n    }\n");
    } else {
        if (assert_name) {
            fprintf(f,
                    "    %s(block_index < %sucd_stage_two_blocks_count);\n"
                    "    %s(offset < %sucd_block_size);\n",
                    assert_name, prefix, assert_name, prefix);
        }
        fprintf(f, "    return %sucd_stage_two[block_index * %d + offset];\n", prefix, block_size);
    }
    fprintf(f, "}\n\n");

    // Generate ucd_entry accessor.
    fprintf(f, "static const %sucd_internal* %sget_ucd_internal(uint32_t cp) {\n", prefix, prefix);
    if (assert_name) fprintf(f, "    %s(cp <= 0x10FFFF);\n", assert_name);
    fprintf(f,
            "    uint32_t stage_one_index = cp / %d;\n"
            "    uint32_t stage_two_index = cp %% %d;\n\n"
            "    %s block_index = %sget_stage_one_value_internal(stage_one_index);\n"
            "    %s entry_index =\n"
            "        %sget_stage_two_value_internal(block_index, stage_two_index);\n\n",
            block_size, block_size, stage_one_function_type.data, prefix, stage_two_type.data, prefix);

    if (assert_name) fprintf(f, "    %s(entry_index < %sucd_entries_size);\n", assert_name, prefix);
    fprintf(f, "    return &%sucd_entries[entry_index];\n}\n\n", prefix);

    // Generate unpacked ucd entry getter.
    fprintf(f,
            "%sUCD_DEF %sucd_entry %sucd_get_entry(uint32_t codepoint) {\n"
            "    const %sucd_internal* internal = %sget_ucd_internal(codepoint);\n"
            "    %sucd_entry result;\n",
            uppercase_prefix, prefix, prefix, prefix, prefix, prefix);
    if (flags & generate_flags_category) {
        fprintf(f, "    result.category = (%sucd_category_enum)((internal->bits0 >> %d) & %d);\n", prefix,
                ucd_category_shift, ucd_category_mask);
    }
    if (flags & generate_flags_case_info) {
        fprintf(f, "    result.case_info = (%sucd_case_info_enum)((internal->bits0 >> %d) & %d);\n", prefix,
                ucd_case_shift, ucd_case_mask);
    }
    if (flags & generate_flags_grapheme_break) {
        fprintf(f,
                "    result.grapheme_break =\n"
                "        (%sucd_grapheme_break_enum)((internal->bits1 >> %d) & %d);\n",
                prefix, grapheme_break_shift, grapheme_break_mask);
    }
    if (flags & generate_flags_width) {
        fprintf(f, "    result.width = (int8_t)((internal->bits1 >> %d) & %d);\n", width_shift, width_mask);
    }
    if (flags & generate_flags_simple_case) {
        fprintf(f,
                "    result.simple_upper = codepoint + internal->simple_upper_offset;\n"
                "    result.simple_title = codepoint + internal->simple_title_offset;\n"
                "    result.simple_lower = codepoint + internal->simple_lower_offset;\n");
    }
    if (flags & generate_flags_simple_case_fold) {
        fprintf(f, "    result.simple_case_fold = codepoint + internal->simple_case_fold_offset;\n");
    }
    if (flags & generate_flags_simple_case_toggle) {
        fprintf(f, "    result.simple_case_toggle = codepoint + internal->simple_case_toggle_offset;\n");
    }
    if (flags & generate_flags_canonical) {
        fprintf(f, "    result.simple_canonical = codepoint + internal->simple_canonical_offset;\n");
    }
    if (flags & generate_flags_compatibility) {
        fprintf(f, "    result.simple_compatibility = codepoint + internal->simple_compatibility_offset;\n");
    }

    if ((flags & generate_flags_canonical) && ucd.min_sizes.canonical_index.size) {
        fprintf(f,
                "    result.full_canonical = %scodepoint_runs + %scanonical_offset +\n"
                "                            internal->canonical_index;\n",
                prefix, prefix);
    }
    if ((flags & generate_flags_compatibility) && ucd.min_sizes.compatibility_index.size) {
        fprintf(f,
                "    result.full_compatibility = %scodepoint_runs + %scompatibility_offset +\n"
                "                                internal->compatibility_index;\n",
                prefix, prefix);
    }
    if (flags & generate_flags_full_case) {
        fprintf(f,
                "    result.full_upper = %scodepoint_runs + %sfull_upper_offset +\n"
                "                        internal->full_upper_index;\n"
                "    result.full_title = %scodepoint_runs + %sfull_title_offset +\n"
                "                        internal->full_title_index;\n"
                "    result.full_lower = %scodepoint_runs + %sfull_lower_offset +\n"
                "                        internal->full_lower_index;\n",
                prefix, prefix, prefix, prefix, prefix, prefix);
    }
    if (flags & generate_flags_full_case_fold) {
        fprintf(f,
                "    result.full_case_fold = %scodepoint_runs + %sfull_case_fold_offset +\n"
                "                            internal->full_case_fold_index;\n",
                prefix, prefix);
    }
    if (flags & generate_flags_full_case_toggle) {
        fprintf(f,
                "    result.full_case_toggle = %scodepoint_runs + internal->full_case_toggle_index;\n",
                prefix);
    }
    fprintf(f, "    return result;\n}\n\n");

    // Generate individual getters.
    if (flags & generate_flags_category) {
        fprintf(f,
                "%sUCD_DEF %sucd_category_enum %sucd_get_category(uint32_t codepoint) {\n"
                "    const %sucd_internal* internal = %sget_ucd_internal(codepoint);\n"
                "    return (%sucd_category_enum)((internal->bits0 >> %d) & %d);\n"
                "}\n\n",
                uppercase_prefix, prefix, prefix, prefix, prefix, prefix, ucd_category_shift, ucd_category_mask);

        fprintf(f,
                "%sUCD_DEF int %sucd_is_whitespace(uint32_t codepoint) {\n"
                "    const %sucd_internal* internal = %sget_ucd_internal(codepoint);\n"
                "    return (internal->bits0 & %d) != 0;\n"
                "}\n\n",
                uppercase_prefix, prefix, prefix, prefix, ucd_whitespace_flag);
    }
    if (flags & generate_flags_case_info) {
        fprintf(f,
                "%sUCD_DEF %sucd_case_info_enum %sucd_get_case_info(uint32_t codepoint) {\n"
                "    const %sucd_internal* internal = %sget_ucd_internal(codepoint);\n"
                "    return (%sucd_case_info_enum)((internal->bits0 >> %d) & %d);\n"
                "}\n\n",
                uppercase_prefix, prefix, prefix, prefix, prefix, prefix, ucd_case_shift, ucd_case_mask);
    }
    if (flags & generate_flags_width) {
        fprintf(f,
                "%sUCD_DEF int %sucd_get_width(uint32_t codepoint) {\n"
                "    const %sucd_internal* internal = %sget_ucd_internal(codepoint);\n"
                "    return (int)((internal->bits1 >> %d) & %d);\n"
                "}\n\n",
                uppercase_prefix, prefix, prefix, prefix, width_shift, width_mask);
    }

    fprintf(f,
            "#ifdef __cplusplus\n"
            "}\n"
            "#endif\n");
}

void generate_header_file(const ucd_min_sizes& min_sizes, uint32_t flags, const option_strings& strings, FILE* f) {
    fprintf(f,
            "/* This file was generated using tools/unicode_gen from\n"
            "   https://github.com/to-miz/tm. Do not modify by hand. */\n");

    auto uppercase_prefix = strings.uppercase_prefix;
    auto prefix = strings.prefix;

    if (!(flags & generate_flags_no_header_guard)) {
        fprintf(f,
                "#ifndef %sUCD_HEADER_INCLUDED_35E987C3_3323_4E66_9755_AE71CB1FE19E\n"
                "#define %sUCD_HEADER_INCLUDED_35E987C3_3323_4E66_9755_AE71CB1FE19E\n\n"
                "#ifndef %sUCD_DEF\n"
                "#define %sUCD_DEF extern\n"
                "#endif\n\n",
                uppercase_prefix, uppercase_prefix, uppercase_prefix, uppercase_prefix);
    }

    fprintf(f,
            "#ifdef __cplusplus\n"
            "extern \"C\" {\n"
            "#endif\n\n");

    if (flags & generate_flags_grapheme_break) {
        // Print grapheme break enum.
        fprintf(f, "typedef enum {\n");
        for (auto i = 0; i < grapheme_break_count; i++) {
            fprintf(f, "    %s%s,\n", prefix, to_string((grapheme_break_enum)i));
        }
        fprintf(f, "\n    %sgrapheme_break_count\n} %sucd_grapheme_break_enum;\n\n", prefix, prefix);
    }

    if (flags & generate_flags_case_info) {
        fprintf(f,
                "typedef enum {\n"
                "    %sucd_case_caseless,\n"
                "    %sucd_case_upper,\n"
                "    %sucd_case_lower,\n"
                "    %sucd_case_title\n"
                "} %sucd_case_info_enum; \n\n",
                prefix, prefix, prefix, prefix, prefix);
    }

    if (flags & generate_flags_category) {
        fprintf(f,
                "typedef enum {\n"
                "    %sucd_category_control,\n"
                "    %sucd_category_letter,\n"
                "    %sucd_category_mark,\n"
                "    %sucd_category_number,\n"
                "    %sucd_category_punctuation,\n"
                "    %sucd_category_symbol,\n"
                "    %sucd_category_separator\n"
                "} %sucd_category_enum;\n\n",
                prefix, prefix, prefix, prefix, prefix, prefix, prefix, prefix);
    }

    // Generate unpacked ucd entry.
    fprintf(f, "typedef struct {\n");
    if (flags & generate_flags_category) fprintf(f, "    %sucd_category_enum category;\n", prefix);
    if (flags & generate_flags_case_info) fprintf(f, "    %sucd_case_info_enum case_info;\n", prefix);
    if (flags & generate_flags_grapheme_break) fprintf(f, "    %sucd_grapheme_break_enum grapheme_break;\n", prefix);
    if (flags & generate_flags_width) fprintf(f, "    int8_t width;\n");
    if (flags & generate_flags_simple_case) {
        fprintf(f,
                "    uint32_t simple_upper;\n"
                "    uint32_t simple_title;\n"
                "    uint32_t simple_lower;\n");
    }
    if (flags & generate_flags_simple_case_fold) fprintf(f, "    uint32_t simple_case_fold;\n");
    if (flags & generate_flags_simple_case_toggle) fprintf(f, "    uint32_t simple_case_toggle;\n");
    if (flags & generate_flags_canonical) fprintf(f, "    uint32_t simple_canonical;\n");
    if (flags & generate_flags_compatibility) fprintf(f, "    uint32_t simple_compatibility;\n");
    if (min_sizes.all_arrays.size > 0) {
        auto type = get_type_from_size(min_sizes.all_arrays);
        if ((flags & generate_flags_canonical) && min_sizes.canonical_index.size) {
            fprintf(f, "    const %s* full_canonical;\n", type.data);
        }
        if ((flags & generate_flags_compatibility) && min_sizes.compatibility_index.size) {
            fprintf(f, "    const %s* full_compatibility;\n", type.data);
        }
        if (flags & generate_flags_full_case) {
            fprintf(f,
                    "    const %s* full_upper;\n"
                    "    const %s* full_title;\n"
                    "    const %s* full_lower;\n",
                    type.data, type.data, type.data);
        }
        if (flags & generate_flags_full_case_fold) fprintf(f, "    const %s* full_case_fold;\n", type.data);
        if (flags & generate_flags_full_case_toggle) fprintf(f, "    const %s* full_case_toggle;\n", type.data);
    }
    fprintf(f, "} %sucd_entry;\n\n", prefix);

    // Generate unpacked ucd entry getter.
    fprintf(f, "%sUCD_DEF %sucd_entry %sucd_get_entry(uint32_t codepoint);\n", uppercase_prefix, prefix, prefix);

    // Generate individual getters.
    if (flags & generate_flags_category) {
        fprintf(f, "%sUCD_DEF %sucd_category_enum %sucd_get_category(uint32_t codepoint);\n", uppercase_prefix, prefix,
                prefix);
        fprintf(f, "%sUCD_DEF int %sucd_is_whitespace(uint32_t codepoint);\n", uppercase_prefix, prefix);
    }
    if (flags & generate_flags_case_info) {
        fprintf(f, "%sUCD_DEF %sucd_case_info_enum %sucd_get_case_info(uint32_t codepoint);\n", uppercase_prefix,
                prefix, prefix);
    }
    if (flags & generate_flags_width) {
        fprintf(f, "%sUCD_DEF int %sucd_get_width(uint32_t codepoint);\n", uppercase_prefix, prefix);
    }

    // Generate feature query defines.
    fprintf(f,
            "\n"
            "#define %sUCD_HAS_CASE_INFO %d\n"
            "#define %sUCD_HAS_CATEGORY %d\n"
            "#define %sUCD_HAS_GRAPHEME_BREAK %d\n"
            "#define %sUCD_HAS_WIDTH %d\n"
            "#define %sUCD_HAS_CANONICAL %d\n"
            "#define %sUCD_HAS_COMPATIBILITY %d\n"
            "#define %sUCD_HAS_FULL_CASE %d\n"
            "#define %sUCD_HAS_FULL_CASE_FOLD %d\n"
            "#define %sUCD_HAS_FULL_CASE_TOGGLE %d\n"
            "#define %sUCD_HAS_SIMPLE_CASE %d\n"
            "#define %sUCD_HAS_SIMPLE_CASE_FOLD %d\n"
            "#define %sUCD_HAS_SIMPLE_CASE_TOGGLE %d\n",
            uppercase_prefix, (flags & generate_flags_case_info) != 0, uppercase_prefix,
            (flags & generate_flags_category) != 0, uppercase_prefix, (flags & generate_flags_grapheme_break) != 0,
            uppercase_prefix, (flags & generate_flags_width) != 0, uppercase_prefix,
            (flags & generate_flags_canonical) != 0, uppercase_prefix, (flags & generate_flags_compatibility) != 0,
            uppercase_prefix, (flags & generate_flags_full_case) != 0, uppercase_prefix,
            (flags & generate_flags_full_case_fold) != 0, uppercase_prefix,
            (flags & generate_flags_full_case_toggle) != 0, uppercase_prefix, (flags & generate_flags_simple_case) != 0,
            uppercase_prefix, (flags & generate_flags_simple_case_fold) != 0, uppercase_prefix,
            (flags & generate_flags_simple_case_toggle) != 0);

    fprintf(f,
            "\n#ifdef __cplusplus\n"
            "}\n"
            "#endif\n");

    if (!(flags & generate_flags_no_header_guard)) {
        fprintf(f, "\n#endif // %sUCD_HEADER_INCLUDED_35E987C3_3323_4E66_9755_AE71CB1FE19E\n", uppercase_prefix);
    }
}