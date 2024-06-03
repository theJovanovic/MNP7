#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
using namespace std::chrono;

const int TABLE_SIZE = 256;

//void initialize_code_tables(vector<vector<pair<int, int>>>& tables) {
//    /*
//    Elements are in numerical order.
//    */
//    tables.resize(TABLE_SIZE, vector<pair<int, int>>(TABLE_SIZE));
//    for (int i = 0; i < TABLE_SIZE; ++i) {
//        for (int j = 0; j < TABLE_SIZE; ++j) {
//            tables[i][j] = { j, 0 };
//        }
//    }
//}

void initialize_code_tables(vector<vector<pair<int, int>>>& tables) {
    /*
    This comment suggests that the table prioritizes the order of characters:
    0 (likely meaning some special characters), lowercase letters, uppercase letters, and other characters.
    */

    tables.resize(TABLE_SIZE, vector<pair<int, int>>(TABLE_SIZE));

    for (int i = 0; i < TABLE_SIZE; ++i) {
        int pos = 0;

        tables[i][pos++] = { 0, 0 };

        for (int j = 97; j <= 122; ++j) {
            tables[i][pos++] = { j, 0 };
        }

        for (int j = 65; j <= 90; ++j) {
            tables[i][pos++] = { j, 0 };
        }

        for (int j = 0; j < TABLE_SIZE; ++j) {
            if ((j != 0) && (j < 65 || j > 90) && (j < 97 || j > 122)) {
                tables[i][pos++] = { j, 0 };
            }
        }
    }
}

string get_code(int position) {
    string code = "0";

    while (code.length() < position + 1) {
        code = "1" + code;
    }

    return code;
}

string rle_compression(const string& input) {
    string output;
    int i = 0;

    while (i < input.length()) {
        char current_char = input[i];
        int run_length = 1;

        while (i + run_length < input.length() && input[i + run_length] == current_char && run_length < 18) {
            run_length++;
        }

        if (run_length < 3) {
            for (int j = 0; j < run_length; ++j) {
                output += current_char;
            }
        }
        else {
            output += current_char;
            output += current_char;
            output += current_char;

            int remainingCount = run_length - 3;
            if (remainingCount > 15) {
                remainingCount = 15;
            }

            output += bitset<4>(remainingCount).to_string();
        }

        i += run_length;
    }

    return output;
}

string rle_decompression(const string& input) {
    stringstream decompressed;
    int n = input.length();

    for (int i = 0; i < n;) {
        char currentChar = input[i];

        if (i + 3 < n && input[i] == input[i + 1] && input[i + 1] == input[i + 2]) {
            int remainingCount = bitset<4>(input.substr(i + 3, 4)).to_ulong();

            decompressed << currentChar << currentChar << currentChar;

            for (int j = 0; j < remainingCount; j++) {
                decompressed << currentChar;
            }
            i += 7;
        }
        else {
            decompressed << currentChar;
            i++;
        }
    }

    return decompressed.str();
}

string mnp7_compression(string& input, bool verbose = false) {
    vector<vector<pair<int, int>>> tables;
    initialize_code_tables(tables);
    stringstream compressed;
    char prev_char = 0;
    int appearances = 0;

    for (int i = 0; i < input.length(); i++) {

        if (appearances == 3) {
            string nibble = input.substr(i, 4);
            int position = bitset<4>(nibble).to_ulong();
            string code = get_code(position);
            compressed << code;
            if (verbose) {
                cout << "Nibble: " << nibble << " | Code: " << code << " | Position: " << position << endl;
            }
            appearances = 0;
            i += 3;
            continue;
        }

        char curr_char = input[i];

        if (curr_char == prev_char) {
            appearances += 1;
        }
        else {
            appearances = 1;
        }

        auto& table = tables[prev_char];
        auto it = find_if(table.begin(), table.end(), [curr_char](const pair<int, int>& p) {
            return p.first == curr_char;
        });

        if (it != table.end()) {
            int position = distance(table.begin(), it);
            string code = get_code(position);
            if (verbose) {
                cout << "Characters: " << prev_char << curr_char << " | Code: " << code << " | Position: " << position << endl;
            }
            compressed << code;
            it->second++;
            sort(table.begin(), table.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;
            });
        }

        prev_char = curr_char;
    }

    return compressed.str();
}

string mnp7_decompression(const string& input) {
    vector<vector<pair<int, int>>> tables;
    initialize_code_tables(tables);
    stringstream decompressed;
    int i = 0;
    char prev_char = 0;
    int appearances = 0;

    while (i < input.length()) {

        int position = 0;
        while (input[i++] == '1') {
            position += 1;
        }

        if (appearances == 3) {
            bitset<4> nibble(position);
            decompressed << nibble;
            appearances = 0;
            continue;
        }

        char curr_char = tables[prev_char][position].first;

        if (curr_char == prev_char) {
            appearances += 1;
        }
        else {
            appearances = 1;
        }

        decompressed << curr_char;
        auto& table = tables[prev_char];

        auto it = find_if(table.begin(), table.end(), [curr_char](const pair<int, int>& p) {
            return p.first == curr_char;
            });

        if (it != table.end()) {
            it->second++;
            sort(table.begin(), table.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;
                });
        }
        prev_char = curr_char;
    }

    return decompressed.str();
}

string generateRandomString(int N) {
    std::string chars = "abcdefghij";
    std::string result;

    std::srand(std::time(nullptr));

    while (result.size() < N) {
        char ch = chars[std::rand() % chars.size()];

        for (int i = 0; i < 4 && result.size() < N; ++i) {
            result += ch;
        }
    }

    return result;
}

int main() {

    string stage1;
    string stage2;
    string stage2_decompression;
    string stage1_decompression;

    for (int n = 1; n <= 1000000; n *= 10) {
        string input = generateRandomString(n);
        double average_time = 0;
        int repetitions = 100.0;

        for (int _ = 0; _ < repetitions; _++) {
            
            auto start = high_resolution_clock::now();

            stage1 = rle_compression(input);
            stage2 = mnp7_compression(stage1);
            stage2_decompression = mnp7_decompression(stage2);
            stage1_decompression = rle_decompression(stage2_decompression);

            auto end = high_resolution_clock::now();
            duration<double, milli> duration = end - start;
            average_time += duration.count();

            if (input != stage1_decompression) {
                cout << "Compression/decompression unsuccessful!" << endl;
            }
        }

        average_time /= repetitions;

        //cout << "Input: " << input << endl;
        cout << "Input size (bits): " << input.length() * 8 << " | Stage 2 compression size (bits): " << stage2.length() << endl;
        cout << "Compression factor (input/compression): ~" << input.length() * 8.0f / stage2.length() << endl;
        cout << "Elapsed time: " << average_time << " ms" << endl;
        cout << endl << endl;
    }

    return 0;
}