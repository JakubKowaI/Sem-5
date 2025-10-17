#include <iostream>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <string>
#include <hpdf.h>
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>

using namespace std;

void draw_header(HPDF_Page page, HPDF_Font font, const string& filename) {
    HPDF_Page_SetFontAndSize(page, font, 14);
    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, 50, 820);
    string header = "File: " + filename;
    HPDF_Page_ShowText(page, header.c_str());
    HPDF_Page_EndText(page);
}

float draw_table(HPDF_Doc pdf, HPDF_Page& page, HPDF_Font font,
                 float startX, float startY,
                 const vector<tuple<string, long double, long double>>& data,
                 const string& title,
                 const long double entropy = -1)
{
    const float rowHeight = 20;
    const float colWidths[3] = {150, 100, 150};
    const float cellPadding = 5;
    const float bottomMargin = 50;

    HPDF_Page_SetFontAndSize(page, font, 10);
    float y = startY;

    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, startX, y);
    HPDF_Page_ShowText(page, title.c_str());
    HPDF_Page_EndText(page);
    y -= 20;

    HPDF_Page_BeginText(page);
    HPDF_Page_MoveTextPos(page, startX, y);
    HPDF_Page_ShowText(page, ("Entropy: "+to_string((double)entropy)).c_str());
    HPDF_Page_EndText(page);
    y -= 20;

    vector<string> headers = {"Character", "Frequency", "Information"};
    float x = startX;

    for (size_t i = 0; i < headers.size(); ++i) {
        HPDF_Page_Rectangle(page, x, y - rowHeight, colWidths[i], rowHeight);
        HPDF_Page_Stroke(page);
        HPDF_Page_BeginText(page);
        HPDF_Page_MoveTextPos(page, x + cellPadding, y - rowHeight + 5);
        HPDF_Page_ShowText(page, headers[i].c_str());
        HPDF_Page_EndText(page);
        x += colWidths[i];
    }
    y -= rowHeight;

    for (const auto& [ch, freq, ent] : data) {
        if (y - rowHeight < bottomMargin) {
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
            HPDF_Page_SetFontAndSize(page, font, 10);
            y = 800; 
        }

        x = startX;
        string freqStr = to_string((double)freq);
        string entStr = to_string((double)ent);
        vector<string> row = {ch, freqStr, entStr};

        for (size_t i = 0; i < row.size(); ++i) {
            HPDF_Page_Rectangle(page, x, y - rowHeight, colWidths[i], rowHeight);
            HPDF_Page_Stroke(page);

            HPDF_Page_BeginText(page);
            HPDF_Page_MoveTextPos(page, x + cellPadding, y - rowHeight + 5);
            HPDF_Page_ShowText(page, row[i].c_str());
            HPDF_Page_EndText(page);

            x += colWidths[i];
        }
        y -= rowHeight;
    }
    return y - 40; 
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Using: " << argv[0] << endl;
        return -1;
    }

    string filename = argv[1];
    ifstream input(filename, ios::binary);
    if (!input.is_open()) {
        cerr << "File not found: " << endl;
        return -1;
    }

    char c;
    int count = 0;
    bitset<8> prev = 0;
    unordered_map<bitset<8>, int> freq;
    unordered_map<bitset<16>, int> freqPlus;

    while (input.get(c)) {
        bitset<8> b(c);
        freq[b]++;
        count++;

        string p1 = prev.to_string();
        string p2 = b.to_string();
        string p = p1 + p2;
        bitset<16> bp(p);
        freqPlus[bp]++;
        prev = b;
    }
    input.close();

    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    if (!pdf) return -1;

    HPDF_Page page = HPDF_AddPage(pdf);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica", NULL);

    draw_header(page, font, filename);

    vector<tuple<string, long double, long double>> table1;
    vector<tuple<string, long double, long double>> table2;
    long double entropy1 = 0.0, entropy2 = 0.0;

    for (auto& it : freq) {
        long double p = (long double)it.second / count;
        long double info = -log2(p);
        table1.push_back({it.first.to_string(), p, info});
        entropy1 += p * info;
    }

    sort(table1.begin(), table1.end(),
    [](const auto& a, const auto& b) {
        return std::get<1>(a) > std::get<1>(b);
    });

    for (auto& itp : freqPlus) {
        long double pxy = (long double)itp.second / count;
        bitset<16> divider(0b0000000011111111); //mask for 8 bits
        auto rightKey = std::bitset<8>((itp.first & divider).to_ulong());
        auto leftKey = std::bitset<8>(((itp.first >> 8) & divider).to_ulong());

        long double px = 0.0;
        if (auto left = freq.find(leftKey); left != freq.end())
            px = (long double)left->second / count;

        long double pyx = px > 0 ? pxy / px : 0.0;
        long double info = pyx > 0 ? -log2(pyx) : 0.0;

        string text = leftKey.to_string() + " " + rightKey.to_string();
        table2.push_back({text, pxy, info});
        entropy2 += pxy * info;
    }

    sort(table2.begin(), table2.end(),
    [](const auto& a, const auto& b) {
        return std::get<1>(a) > std::get<1>(b);
    });

    // for (auto& itp : freqPlus) {
    //     long double p = (long double)itp.second / count;
    //     bitset<16> divider(0b000000000011111111111111);
    //     // cout<<"Full key: "<<itp.first.to_string()<<endl;
    //     auto rightKey = std::bitset<8> ( (itp.first & divider).to_ulong() );
    //     auto leftKey = std::bitset<8> ( ((itp.first >> 8) & divider).to_ulong() );
    //     // cout<<"Left: "<<leftKey.to_string()<<" Right: "<<rightKey.to_string()<<endl;
    //     string text = leftKey.to_string() + " " + rightKey.to_string();
    //     long double lp=0.0;
    //     long double rp=0.0;
    //     if (auto left = freq.find(leftKey); left != freq.end())
    //         lp = (long double)left->second / count;
    
    //     if (auto right = freq.find(rightKey); right != freq.end())
    //         rp = (long double)right->second / count;
        
    //     long double info = (-lp * log2(lp)) + (-rp * log2(rp));
    //     table2.push_back({text, p, info});
    //     entropy2 += p*info;
    // }

    float y = 780;
    y = draw_table(pdf, page, font, 50, y, table1, "Single Characters", entropy1);
    y = draw_table(pdf, page, font, 50, y, table2, "Character Pairs (Prev + Current)", entropy2);

    HPDF_SaveToFile(pdf, "output.pdf");
    HPDF_Free(pdf);

    cout << "PDF saved as output.pdf" << endl;
    cout << "Entropy diff: " << abs(entropy1 - entropy2) << endl;
    cout << "Entropy conditional: " << entropy2 << endl;
    return 0;
}
