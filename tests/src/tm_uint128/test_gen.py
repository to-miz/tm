import random as random
random.seed(1)

import math
import struct

def next_up(x):
    # NaNs and positive infinity map to themselves.
    if math.isnan(x) or (math.isinf(x) and x > 0):
        return x

    # 0.0 and -0.0 both map to the smallest +ve float.
    if x == 0.0:
        x = 0.0

    n = struct.unpack('<q', struct.pack('<d', x))[0]
    if n >= 0:
        n += 1
    else:
        n -= 1
    return struct.unpack('<d', struct.pack('<q', n))[0]

def next_down(x):
    return -next_up(-x)

def nextafter(x, y):
    # If either argument is a NaN, return that argument.
    # This matches the implementation in decimal.Decimal
    if math.isnan(x):
        return x
    if math.isnan(y):
        return y

    if y == x:
        return y
    elif y > x:
        return next_up(x)
    else:
        return next_down(x)

def compare(a, b):
    if a < b: return -1
    if a > b: return 1
    return 0

def fls(v):
    if v == 0:
        return -1
    s = f"{v:0128b}"
    b = bytearray()
    b.extend(map(ord, s))
    result = 0
    for entry in b:
        if entry != ord('0'):
            break
        result += 1
    return 127 - result

def ffs(v):
    if v == 0:
        return -1
    s = f"{v:0128b}"
    b = bytearray()
    b.extend(map(ord, s))
    result = 0
    for entry in reversed(b):
        if entry != ord('0'):
            break
        result += 1
    return result

def popcount(v):
    s = f"{v:b}"
    b = bytearray()
    b.extend(map(ord, s))
    result = 0
    for entry in b:
        if entry != ord('0'): result += 1
    return result

def hex_byte_string(v):
    result = "{"
    for i in range(16):
        result += f'0x{v & 0xFF:x}'
        v >>= 8
        if i != 15: result += ', '
    result += '}'
    return result

def round_nearest(v):
    return float(v)

def round_up(v):
    result = float(v)
    if v <= int(result):
        return result
    return nextafter(result, float('+inf'))

def round_down(v):
    result = float(v)
    if v >= int(result):
        return result
    return nextafter(result, float('-inf'))

def round_zero(v):
    result = float(v)
    if v >= int(result):
        return result
    return nextafter(result, 0.0)

def generate(i, a, b):
    shift_amount = random.randint(0, 127)
    print(f'TEST_CASE("generated test {i}") {{')
    print(f'    tmi_uint128_t a = tmi_from_string("{a:X}", /*base=*/16);')
    print(f'    tmi_uint128_t b = tmi_from_string("{b:X}", /*base=*/16);')
    print(f'    tmi_uint128_t shift_amount = tmi_make_low(0x{shift_amount:X});')
    print(f'    CHECK(tml::to_string(a, 10) == "{a & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}");')
    print(f'    CHECK(tml::to_string(b, 10) == "{b & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF}");')
    print(f'    CHECK(tml::to_string(a, 2) == "{a & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:b}");')
    print(f'    CHECK(tml::to_string(b, 2) == "{b & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:b}");')
    print(f'    CHECK(tml::to_string(a, 16) == "{a & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(b, 16) == "{b & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_add(a, b), 16) == "{(a + b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_sub(a, b), 16) == "{(a - b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_mul(a, b), 16) == "{(a * b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    if b != 0:
        print(f'    CHECK(tml::to_string(tmi_div(a, b), 16) == "{(a // b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
        print(f'    CHECK(tml::to_string(tmi_mod(a, b), 16) == "{(a % b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
        print(f'    CHECK(tmi_fls(b) == {fls(b) & 0xFFFFFFFFFFFFFFFF}ull);')
        print(f'    CHECK(tmi_ffs(b) == {ffs(b) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_fls_s(b) == {fls(b) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_ffs_s(b) == {ffs(b) & 0xFFFFFFFFFFFFFFFF}ull);')
    if a != 0:
        print(f'    CHECK(tmi_fls(a) == {fls(a) & 0xFFFFFFFFFFFFFFFF}ull);')
        print(f'    CHECK(tmi_ffs(a) == {ffs(a) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_fls_s(a) == {fls(a) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_ffs_s(a) == {ffs(a) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tml::to_string(tmi_and(a, b), 16) == "{(a & b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_or(a, b), 16) == "{(a | b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_xor(a, b), 16) == "{(a ^ b) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_shl(a, shift_amount), 16) == "{(a << shift_amount) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_shr(a, shift_amount), 16) == "{(a >> shift_amount) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_shl(b, shift_amount), 16) == "{(b << shift_amount) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_shr(b, shift_amount), 16) == "{(b >> shift_amount) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_inc(a), 16) == "{(a + 1) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_inc(b), 16) == "{(b + 1) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_dec(a), 16) == "{(a - 1) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_dec(b), 16) == "{(b - 1) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tmi_lt(a, b) == {str(a < b).lower()});')
    print(f'    CHECK(tmi_lte(a, b) == {str(a <= b).lower()});')
    print(f'    CHECK(tmi_gt(a, b) == {str(a > b).lower()});')
    print(f'    CHECK(tmi_gte(a, b) == {str(a >= b).lower()});')
    print(f'    CHECK(tmi_eq(a, b) == {str(a == b).lower()});')
    print(f'    CHECK(tmi_cmp(a, b) == {str(compare(a, b)).lower()});')
    print(f'    CHECK(tmi_is_zero(a) == {str(a == 0).lower()});')
    print(f'    CHECK(tmi_is_zero(b) == {str(b == 0).lower()});')
    print(f'    CHECK(tmi_is_not_zero(a) == {str(a != 0).lower()});')
    print(f'    CHECK(tmi_is_not_zero(b) == {str(b != 0).lower()});')
    print(f'    CHECK(tmi_is_bit_set(a, tmi_low(shift_amount)) == {str((a & (1 << shift_amount)) > 0).lower()});')
    print(f'    CHECK(tmi_is_bit_set(b, tmi_low(shift_amount)) == {str((b & (1 << shift_amount)) > 0).lower()});')
    print(f'    CHECK(tml::to_string(tmi_set_bit(a, tmi_low(shift_amount)), 16) == "{(a | ((1 << shift_amount))) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_set_bit(b, tmi_low(shift_amount)), 16) == "{(b | ((1 << shift_amount))) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tmi_low(a) == 0x{a & 0xFFFFFFFFFFFFFFFF:016X}ull);')
    print(f'    CHECK(tmi_low(b) == 0x{b & 0xFFFFFFFFFFFFFFFF:016X}ull);')
    print(f'    CHECK(tmi_high(a) == 0x{(a >> 64) & 0xFFFFFFFFFFFFFFFF:016X}ull);')
    print(f'    CHECK(tmi_high(b) == 0x{(b >> 64) & 0xFFFFFFFFFFFFFFFF:016X}ull);')
    print(f'    CHECK(tmi_popcount(a) == {popcount(a) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_popcount(b) == {popcount(b) & 0xFFFFFFFFFFFFFFFF}ull);')
    print(f'    CHECK(tmi_to_bytes(a) == tmi_bytes{hex_byte_string(a)});')
    print(f'    CHECK(tmi_to_bytes(b) == tmi_bytes{hex_byte_string(b)});')
    print(f'    auto a_str = tml::to_string(a); INFO(a_str);')
    print(f'    CHECK(tmi_to_double_rm(a, TM_FE_TONEAREST) == {float.hex(round_nearest(a))});')
    print(f'    CHECK(tmi_to_double_rm(a, TM_FE_DOWNWARD) == {float.hex(round_down(a))});')
    print(f'    CHECK(tmi_to_double_rm(a, TM_FE_UPWARD) == {float.hex(round_up(a))});')
    print(f'    CHECK(tmi_to_double_rm(a, TM_FE_TOWARDZERO) == {float.hex(round_zero(a))});')
    print(f'    auto b_str = tml::to_string(b); INFO(b_str);')
    print(f'    CHECK(tmi_to_double_rm(b, TM_FE_TONEAREST) == {float.hex(round_nearest(b))});')
    print(f'    CHECK(tmi_to_double_rm(b, TM_FE_DOWNWARD) == {float.hex(round_down(b))});')
    print(f'    CHECK(tmi_to_double_rm(b, TM_FE_UPWARD) == {float.hex(round_up(b))});')
    print(f'    CHECK(tmi_to_double_rm(b, TM_FE_TOWARDZERO) == {float.hex(round_zero(b))});')
    print(f'    CHECK(tml::to_string(tmi_from_double(tmi_to_double_rm(a, TM_FE_TONEAREST)), 16) == "{int(round_nearest(a)) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print(f'    CHECK(tml::to_string(tmi_from_double(tmi_to_double_rm(b, TM_FE_TONEAREST)), 16) == "{int(round_nearest(b)) & 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF:X}");')
    print('}\n')

tests_count = 10

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    generate(i, a, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF0000000000000000
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF0000000000000000
    generate(i + tests_count * 1, a, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0x0000000000000000FFFFFFFFFFFFFFFF
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0x0000000000000000FFFFFFFFFFFFFFFF
    generate(i + tests_count * 2, a, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF0000000000000000
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0x0000000000000000FFFFFFFFFFFFFFFF
    generate(i + tests_count * 3, a, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0x0000000000000000FFFFFFFFFFFFFFFF
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & 0xFFFFFFFFFFFFFFFF0000000000000000
    generate(i + tests_count * 4, a, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    generate(i + tests_count * 5, a, 0)

for i in range(tests_count):
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    generate(i + tests_count * 6, 0, b)

for i in range(tests_count):
    a = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    b = random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF) & random.randint(0, 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)
    generate(i + tests_count * 7, a, b)