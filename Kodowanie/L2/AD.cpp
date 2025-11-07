// arith_decode.cpp
// Dekoder arytmetyczny z skalowaniem (E1/E2/E3).
// Założenia:
//  - Format pliku: [uint32: alphabet_size] { [uint8: symbol] [uint32: weight] }*alphabet_size
//                   [uint32: original_length]
//                   [rest bits: binary fractional representation of code z]
//  - 32-bit integers may be little- or big-endian; program will try to autodetect.
//  - Weights are non-negative uint32; their sum > 0.
//  - Output: decoded byte sequence to stdout.
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i64 = int64_t;

static uint32_t read_u32_be(const vector<uint8_t>& buf, size_t off) {
    return (uint32_t(buf[off])<<24) | (uint32_t(buf[off+1])<<16) | (uint32_t(buf[off+2])<<8) | uint32_t(buf[off+3]);
}
static uint32_t read_u32_le(const vector<uint8_t>& buf, size_t off) {
    return (uint32_t(buf[off+3])<<24) | (uint32_t(buf[off+2])<<16) | (uint32_t(buf[off+1])<<8) | uint32_t(buf[off]);
}

// helper: read whole file into vector<uint8_t>
vector<uint8_t> read_file(const string& path) {
    ifstream f(path, ios::binary);
    if(!f) {
        cerr << "Cannot open file: " << path << "\n";
        exit(2);
    }
    f.seekg(0, ios::end);
    size_t sz = (size_t)f.tellg();
    f.seekg(0, ios::beg);
    vector<uint8_t> b(sz);
    if(sz) f.read((char*)b.data(), (streamsize)sz);
    return b;
}

// Bit reader for the tail bits (MSB-first)
struct BitReader {
    const vector<uint8_t>& buf;
    size_t byte_pos;
    int bit_pos; // 0..7, next bit to read is (7 - bit_pos) if MSB-first
    BitReader(const vector<uint8_t>& b, size_t start_byte=0) : buf(b), byte_pos(start_byte), bit_pos(0) {}
    // read one bit, return 0 or 1. If out of bits, return 0 (pad with zeros).
    int read_bit() {
        if(byte_pos >= buf.size()) return 0;
        int bit = (buf[byte_pos] >> (7 - bit_pos)) & 1;
        bit_pos++;
        if(bit_pos == 8) { bit_pos = 0; byte_pos++; }
        return bit;
    }
    // read upto n bits and return as uint32 (left-aligned from MSB). But we'll typically read single bits.
    uint32_t read_bits_int(int n) {
        uint32_t v = 0;
        for(int i=0;i<n;i++){
            v = (v<<1) | (uint32_t)read_bit();
        }
        return v;
    }
};

// Find symbol index given scaled_value using binary search on cumulative array.
int find_symbol(const vector<u64>& cum, u64 scaled_value) {
    // cum: length m+1, cum[0]=0, cum[m]=total
    // find largest s such that cum[s] <= scaled_value (we want s in 1..m)
    int lo=0, hi=(int)cum.size()-1;
    while(lo+1<hi){
        int mid=(lo+hi)/2;
        if(cum[mid] <= scaled_value) lo=mid;
        else hi=mid;
    }
    // lo is index where cum[lo] <= scaled_value < cum[lo+1]
    return lo+1; // symbol index 1..m used in algorithm (corresponds to cum[lo]..cum[lo+1])
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if(argc != 2){
        cerr << "Użycie: " << argv[0] << " compressed_file\n";
        return 1;
    }
    string path = argv[1];
    auto data = read_file(path);
    if(data.size() < 4) {
        cerr << "Plik zbyt krótki.\n";
        return 2;
    }

    // Try read alphabet_size using both endian interpretations and pick plausible one (<= 256 and >0).
    u32 alph_le = read_u32_le(data, 0);
    u32 alph_be = read_u32_be(data, 0);
    bool use_le = false;
    if(alph_le > 0 && alph_le <= 256 && !(alph_be > 0 && alph_be <= 256)) use_le = true;
    else if(alph_be > 0 && alph_be <= 256 && !(alph_le > 0 && alph_le <= 256)) use_le = false;
    else {
        // both plausible or both implausible: prefer little-endian as default
        use_le = true;
    }

    function<u32(size_t)> read_u32 = [&](size_t off)->u32 {
        if(off+4 > data.size()) { cerr << "Nieoczekiwany koniec pliku podczas odczytu 32-bit.\n"; exit(3); }
        return use_le ? read_u32_le(data, off) : read_u32_be(data, off);
    };

    size_t offset = 0;
    u32 alphabet_size = read_u32(offset);
    offset += 4;

    if(alphabet_size == 0) {
        cerr << "Alfabet ma rozmiar 0 — nic do zrobienia.\n";
        return 0;
    }
    // ensure there's enough bytes for (symbol + weight) * alphabet_size
    size_t needed = offset + (size_t)alphabet_size * (1 + 4);
    if(needed + 4 > data.size()) {
        // maybe wrong endianness — try other endianness once
        // (Re-try switching endianness if plausible)
        // toggle endianness flag; the read_u32 closure consults use_le, so no need to reassign it
        use_le = !use_le;
        offset = 0;
        alphabet_size = read_u32(offset); offset += 4;
        needed = offset + (size_t)alphabet_size * (1 + 4);
    }

    // Read symbols and weights
    vector<uint8_t> symbols;
    vector<u64> weights; // use 64-bit to sum safely
    symbols.reserve(alphabet_size);
    weights.reserve(alphabet_size);
    for(u32 i=0;i<alphabet_size;i++){
        if(offset + 1 + 4 > data.size()) { cerr << "Plik zbyt krótki podczas odczytu alfabetu.\n"; return 4; }
        uint8_t sym = data[offset];
        uint32_t w = read_u32(offset+1);
        offset += 1+4;
        symbols.push_back(sym);
        weights.push_back((u64)w);
    }

    if(offset + 4 > data.size()) {
        cerr << "Brak pola z długością oryginalnego tekstu.\n";
        return 5;
    }
    u32 original_len = read_u32(offset);
    offset += 4;

    // remaining bytes from offset are the bitstream for the fractional code
    vector<uint8_t> tail;
    if(offset <= data.size()) tail.insert(tail.end(), data.begin()+offset, data.end());
    BitReader br(tail, 0);

    // Build cumulative frequency table
    u64 total = 0;
    for(u64 w : weights) total += w;
    if(total == 0) { cerr << "Suma wag = 0 — nie da się zdekodować.\n"; return 6; }

    // Make cumulative array cum[0]=0, cum[1]=w0, ..., cum[m]=total
    vector<u64> cum; cum.reserve(weights.size()+1);
    cum.push_back(0);
    for(size_t i=0;i<weights.size();++i) cum.push_back(cum.back() + weights[i]);

    // Map symbol indices: we'll use indices 1..m consistent with cum
    // State bits
    const int STATE_BITS = 32;
    const u64 FULL = (1ULL<<STATE_BITS);
    const u64 MAX_RANGE = FULL - 1;
    const u64 HALF = (1ULL << (STATE_BITS - 1));
    const u64 QUARTER = (HALF >> 1);
    const u64 THREE_QUARTERS = HALF + QUARTER;

    // Initialize decoder registers
    u64 low = 0;
    u64 high = MAX_RANGE;
    // fill code with first STATE_BITS bits from bitstream
    u64 code = 0;
    for(int i=0;i<STATE_BITS;i++){
        code = (code << 1) | (u64)br.read_bit();
    }

    // Output to stdout (binary)
    std::ostream &out = cout;
    // Decode original_len symbols
    for(u32 decoded = 0; decoded < original_len; ++decoded) {
        u64 range = high - low + 1;
        // scaled_value in [0, total-1]
        // scaled = floor( ( (code - low + 1) * total - 1 ) / range )
        u64 tmp1 = (code - low + 1);
        // multiply tmp1 * total could overflow 64-bit if total large * tmp1 ~ 2^64.
        // But total is sum of uint32s <= 2^32-1 * 256 < 2^40, tmp1 < 2^32, product < 2^72 -> may overflow.
        // We'll use built-in unsigned __int128 to be safe.
        __uint128_t prod = (__uint128_t)tmp1 * (__uint128_t)total;
        u64 scaled = (u64)((prod - 1) / range);

        // Find symbol whose cumulative interval contains scaled
        // cum[k-1] <= scaled < cum[k]
        int k = find_symbol(cum, scaled); // returns 1..m
        // update interval to symbol's subinterval
        u64 sym_low = cum[k-1];
        u64 sym_high = cum[k];
        // Compute new high/low:
        __uint128_t numer_low = (__uint128_t)range * (__uint128_t)sym_low;
        __uint128_t numer_high = (__uint128_t)range * (__uint128_t)sym_high;
        u64 new_low = low + (u64)(numer_low / total);
        u64 new_high = low + (u64)(numer_high / total) - 1;
        low = new_low;
        high = new_high;

        // renormalization with E1/E2/E3
        while(true){
            if(high < HALF) {
                // do nothing, bit 0
            } else if(low >= HALF) {
                // subtract half from all
                code -= HALF;
                low -= HALF;
                high -= HALF;
            } else if(low >= QUARTER && high < THREE_QUARTERS) {
                // underflow case
                code -= QUARTER;
                low -= QUARTER;
                high -= QUARTER;
            } else {
                break;
            }
            // shift left one bit
            low = (low << 1) & MAX_RANGE;
            high = ((high << 1) & MAX_RANGE) | 1;
            int nextb = br.read_bit();
            code = ((code << 1) & MAX_RANGE) | (u64)nextb;
        }

        // output symbol
        uint8_t outb = symbols[k-1];
        out.put((char)outb);
    }

    // flush
    out.flush();
    return 0;
}
