static bool asserts_enabled = true;
void enable_asserts(bool enabled) { asserts_enabled = enabled; }
void assert_throws(bool cond, const char* file, int line) {
    if (!cond && asserts_enabled) {
        throw std::runtime_error(std::string("testing assertion throw: ") + file + ":" + std::to_string(line));
    }
}