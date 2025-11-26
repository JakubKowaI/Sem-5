// arithmetic_coding.cpp
// Prosty program do kodowania i dekodowania arytmetycznego (wariant Witten-Neal-Cleary)
// z renormalizacją (skalowaniem) i obsługą podziału podprzedziałów (E3 underflow).
// Tryb: encode lub decode
// Kompilacja: g++ -std=c++17 -O2 arithmetic_coding.cpp -o arith
// Użycie:
//  ./arith encode input.bin output.bin
//  ./arith decode input.bin output.bin

#include <bits/stdc++.h>
using namespace std;
using u32 = uint32_t;
using u64 = uint64_t;

// Parametry arytmetyki
static const unsigned CODE_BITS = 32;
static const u32 TOP        = 0xFFFFFFFFu;
static const u32 HALF       = 0x80000000u; // 1 << 31
static const u32 QUARTER    = 0x40000000u; // 1 << 30
static const u32 THREE_QUART = HALF + QUARTER;

// Liczba symboli: bajty (0..255) + EOF symbol
static const int SYMBOLS = 257;
static const int EOF_SYMBOL = 256;

// Bit output
struct BitOutput {
    FILE* f;
    int buffer = 0;
    int bits_filled = 0;
    BitOutput(FILE* f_): f(f_) {}
    void put_bit(int b){
        buffer = (buffer << 1) | (b & 1);
        bits_filled++;
        if(bits_filled == 8){
            fputc(buffer, f);
            bits_filled = 0;
            buffer = 0;
        }
    }
    void flush(){
        if(bits_filled > 0){
            buffer <<= (8 - bits_filled);
            fputc(buffer, f);
            bits_filled = 0;
            buffer = 0;
        }
    }
};

// Bit input
struct BitInput {
    FILE* f;
    int buffer = 0;
    int bits_left = 0;
    BitInput(FILE* f_): f(f_) {}
    int read_bit(){
        if(bits_left == 0){
            int c = fgetc(f);
            if(c == EOF) return -1;
            buffer = c;
            bits_left = 8;
        }
        int res = (buffer >> (bits_left - 1)) & 1;
        bits_left--;
        return res;
    }
};

// Frequencies i kumulatywy
struct FrequencyTable{
    vector<u32> freq; // size SYMBOLS
    vector<u32> cum;  // cum[i] = sum_{j=0}^{i-1} freq[j]
    u32 total = 0;
    FrequencyTable(){
        freq.assign(SYMBOLS, 1); // minimalnie 1, unikanie zer
        build_cum();
    }
    void build_cum(){
        cum.assign(SYMBOLS + 1, 0);
        total = 0;
        for(int i=0;i<SYMBOLS;i++){
            cum[i] = total;
            total += freq[i];
        }
        cum[SYMBOLS] = total;
    }
    void set_from_vector(const vector<u32>& v){
        if((int)v.size() != SYMBOLS) throw runtime_error("bad freq size");
        freq = v;
        // ensure no zero
        for(int i=0;i<SYMBOLS;i++) if(freq[i] == 0) freq[i] = 1;
        build_cum();
    }
};

// Pomoc: zapis/odczyt nagłówka (częstotliwości) - zapisujemy 257 uint32 (big-endian)
void write_header(FILE* out, const FrequencyTable &ft){
    for(int i=0;i<SYMBOLS;i++){
        u32 v = ft.freq[i];
        // big-endian
        fputc((v >> 24) & 0xFF, out);
        fputc((v >> 16) & 0xFF, out);
        fputc((v >> 8) & 0xFF, out);
        fputc((v) & 0xFF, out);
    }
}

FrequencyTable read_header(FILE* in){
    FrequencyTable ft;
    vector<u32> v(SYMBOLS);
    for(int i=0;i<SYMBOLS;i++){
        int b0 = fgetc(in);
        int b1 = fgetc(in);
        int b2 = fgetc(in);
        int b3 = fgetc(in);
        if(b3 == EOF) throw runtime_error("Unexpected EOF in header");
        u32 val = ((u32)b0<<24) | ((u32)b1<<16) | ((u32)b2<<8) | (u32)b3;
        if(val == 0) val = 1; // safety
        v[i] = val;
    }
    ft.set_from_vector(v);
    return ft;
}

// ENCODER
void encode_file(FILE* in, FILE* out){
    // read entire input to compute static frequencies
    vector<unsigned char> data;
    int c;
    while((c = fgetc(in)) != EOF) data.push_back((unsigned char)c);

    FrequencyTable ft;
    // zero then count
    for(int i=0;i<SYMBOLS;i++) ft.freq[i] = 0;
    for(unsigned char b: data) ft.freq[b]++;
    ft.freq[EOF_SYMBOL] = 1; // EOF symbol
    // ensure no zero
    for(int i=0;i<SYMBOLS;i++) if(ft.freq[i]==0) ft.freq[i]=1;
    ft.build_cum();

    // write header
    write_header(out, ft);

    BitOutput bout(out);

    u32 low = 0;
    u32 high = TOP;
    u64 underflow = 0;

    auto output_bit_plus_underflow = [&](int bit){
        bout.put_bit(bit);
        while(underflow > 0){
            bout.put_bit(!bit);
            underflow--;
        }
    };

    auto encode_symbol = [&](int sym){
        u64 range = (u64)high - (u64)low + 1ULL;
        u64 total = ft.total;
        u64 sym_low = ft.cum[sym];
        u64 sym_high = ft.cum[sym+1];
        u64 new_low = low + (u32)((range * sym_low) / total);
        u64 new_high = low + (u32)((range * sym_high) / total) - 1;
        low = (u32)new_low;
        high = (u32)new_high;

        for(;;){
            if((low & HALF) == (high & HALF)){
                int bit = (low & HALF) ? 1 : 0;
                output_bit_plus_underflow(bit);
                low = (u32)((low << 1) & TOP);
                high = (u32)(((high << 1) & TOP) | 1);
            } else if((low & ~high & QUARTER) != 0){
                // underflow condition
                underflow++;
                low = (u32)((low << 1) & (TOP >> 1));
                high = (u32)((((high ^ HALF) << 1) & TOP) | HALF | 1);
            } else break;
        }
    };

    // encode data symbols
    for(unsigned char b: data) encode_symbol((int)b);
    // EOF
    encode_symbol(EOF_SYMBOL);

    // finish: output one 1 bit then necessary underflow complement (per WNC)
    underflow++;
    if(low < QUARTER) output_bit_plus_underflow(0);
    else output_bit_plus_underflow(1);

    bout.flush();
}

// DECODER
void decode_file(FILE* in, FILE* out){
    FrequencyTable ft = read_header(in);
    BitInput bin(in);

    u32 low = 0;
    u32 high = TOP;
    u32 code = 0;
    // read initial CODE_BITS bits
    for(unsigned i=0;i<CODE_BITS;i++){
        int b = bin.read_bit();
        code = (code << 1) | (b < 0 ? 0 : (u32)b);
    }

    while(true){
        u64 range = (u64)high - (u64)low + 1ULL;
        u64 total = ft.total;
        u64 value = ((u64)(code - low) + 0ULL) * total / range;
        // find symbol s with cum[s] <= value < cum[s+1]
        int symbol = int(upper_bound(ft.cum.begin(), ft.cum.end(), (u32)value) - ft.cum.begin() - 1);
        if(symbol < 0) symbol = 0;
        if(symbol >= SYMBOLS) symbol = SYMBOLS-1;

        if(symbol == EOF_SYMBOL) break;

        // output symbol
        fputc((unsigned char)symbol, out);

        // update interval
        u64 sym_low = ft.cum[symbol];
        u64 sym_high = ft.cum[symbol+1];
        u32 new_low = low + (u32)((range * sym_low) / total);
        u32 new_high = low + (u32)((range * sym_high) / total) - 1;
        low = new_low; high = new_high;

        // renormalize
        for(;;){
            if((low & HALF) == (high & HALF)){
                low = (u32)((low << 1) & TOP);
                high = (u32)(((high << 1) & TOP) | 1);
                int b = bin.read_bit();
                code = (u32)(((code << 1) & TOP) | (b < 0 ? 0 : (u32)b));
            } else if((low & ~high & QUARTER) != 0){
                low = (u32)((low << 1) & (TOP >> 1));
                high = (u32)((((high ^ HALF) << 1) & TOP) | HALF | 1);
                int b = bin.read_bit();
                code = (u32)(((code ^ HALF) << 1) & TOP);
                code |= (b < 0 ? 0 : (u32)b);
            } else break;
        }
    }
}

int main(int argc, char** argv){
    if(argc < 4){
        fprintf(stderr, "Usage: %s encode|decode input output\n", argv[0]);
        return 1;
    }
    string mode = argv[1];
    FILE* in = fopen(argv[2], "rb");
    if(!in){ perror("fopen input"); return 1; }
    FILE* out = fopen(argv[3], "wb");
    if(!out){ perror("fopen output"); fclose(in); return 1; }

    try{
        if(mode == "encode") encode_file(in, out);
        else if(mode == "decode") decode_file(in, out);
        else{ fprintf(stderr, "Unknown mode\n"); }
    }catch(exception &e){
        fprintf(stderr, "Error: %s\n", e.what());
    }

    fclose(in); fclose(out);
    return 0;
}
