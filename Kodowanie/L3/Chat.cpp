#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <stdexcept>

using namespace std;

/* ===========================================================
   ELIAS GAMMA
   =========================================================== */

u_int64_t eliasGammaDecode(string &s) {
    size_t zeros = 0;
    while (zeros < s.size() && s[zeros] == '0') zeros++;

    size_t len = zeros + 1;
    if (s.size() < zeros + len) throw runtime_error("Incomplete Elias Gamma");

    string bin = s.substr(zeros, len);
    s.erase(0, zeros + len);
    return stoull(bin, nullptr, 2);
}

/* ===========================================================
   ELIAS DELTA
   =========================================================== */

u_int64_t eliasDeltaDecode(string &s) {
    u_int64_t len = eliasGammaDecode(s);
    if (len == 0) throw runtime_error("Invalid Elias Delta length");

    if (s.size() < len - 1) throw runtime_error("Incomplete Elias Delta");
    string rest = s.substr(0, len - 1);
    s.erase(0, len - 1);

    string bin = "1" + rest;
    return stoull(bin, nullptr, 2);
}

/* ===========================================================
   ELIAS OMEGA
   =========================================================== */

u_int64_t eliasOmegaDecode(string &s) {
    u_int64_t n = 1;
    size_t pos = 0;

    while (true) {
        if (pos >= s.size()) throw runtime_error("Incomplete Elias Omega");

        if (s[pos] == '0') {
            s.erase(0, pos + 1);
            return n;
        }

        u_int64_t chunk = n + 1;
        if (pos + chunk > s.size()) throw runtime_error("Incomplete Elias Omega");

        string bin = s.substr(pos, chunk);
        n = stoull(bin, nullptr, 2);
        pos += chunk;
    }
}

/* ===========================================================
   FIBONACCI CODING
   =========================================================== */

u_int64_t fibonacciDecode(string &s) {
    size_t end = s.find("11");
    if (end == string::npos) throw runtime_error("Incomplete Fibonacci");

    string block = s.substr(0, end + 2);
    s.erase(0, end + 2);

    vector<u_int64_t> F = {1, 2};
    while (F.size() < block.size())
        F.push_back(F[F.size() - 1] + F[F.size() - 2]);

    u_int64_t value = 0;
    for (size_t i = 0; i < block.size(); i++)
        if (block[i] == '1') value += F[i];

    return value - F.back();
}

/* ===========================================================
   PICK DECODER
   =========================================================== */

using DecodeFn = u_int64_t (*)(string &);

DecodeFn pickDecode(char mode) {
    switch (mode) {
        case 'w': return eliasOmegaDecode;
        case 'y': return eliasGammaDecode;
        case 'o': return eliasDeltaDecode;
        case 'f': return fibonacciDecode;
        default:
            cerr << "Unknown mode, using Elias Omega\n";
            return eliasOmegaDecode;
    }
}

/* ===========================================================
   LZW DECODE
   =========================================================== */

void lzwDecode(ifstream &in, ofstream &out, char mode) {
    DecodeFn decode = pickDecode(mode);

    vector<string> dict;
    dict.reserve(4096);
    for (int i = 0; i < 256; i++)
        dict.push_back(string(1, char(i)));

    string bitbuf;

    auto feedByte = [&]() -> bool {
        int c = in.get();
        if (c == EOF) return false;
        bitbuf += bitset<8>((unsigned char)c).to_string();
        return true;
    };

    // --- read first code ---
    u_int64_t prev;
    while (true) {
        try {
            prev = decode(bitbuf);
            break;
        } catch (...) {
            if (!feedByte()) return;
        }
    }

    string w = dict[prev];
    out << w;

    // --- main loop ---
    while (true) {
        u_int64_t code;
        bool ready = false;

        while (!ready) {
            try {
                code = decode(bitbuf);
                ready = true;
            } catch (...) {
                if (!feedByte()) return;
            }
        }

        string entry;
        if (code < dict.size()) {
            entry = dict[code];
        } else if (code == dict.size()) {
            entry = w + w[0];
        } else {
            throw runtime_error("Invalid LZW index");
        }

        out << entry;
        dict.push_back(w + entry[0]);
        w = entry;
    }
}

/* ===========================================================
   MAIN
   =========================================================== */

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: ./lzw_decode file\n";
        return 1;
    }

    ifstream in(argv[1], ios::binary);
    if (!in) {
        cerr << "Cannot open input file\n";
        return 1;
    }

    ofstream out("LZWDecoded.txt", ios::binary);
    if (!out) {
        cerr << "Cannot open output file\n";
        return 1;
    }

    int m = in.get();
    if (m == EOF) {
        cerr << "Empty file\n";
        return 1;
    }

    char mode = char(m);

    try {
        lzwDecode(in, out, mode);
    } catch (exception &e) {
        cerr << "Decode error: " << e.what() << "\n";
        return 2;
    }

    return 0;
}
