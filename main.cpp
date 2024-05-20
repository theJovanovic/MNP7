#include <algorithm>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

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

// Defines a function `initialize_code_tables` that takes a reference to a 2D vector of integer pairs.
void initialize_code_tables(vector<vector<pair<int, int>>>& tables) {
    /*
    This comment suggests that the table prioritizes the order of characters:
    0 (likely meaning some special characters), lowercase letters, uppercase letters, and other characters.
    */

    // Resize the 'tables' vector to have TABLE_SIZE number of rows, each row containing TABLE_SIZE pairs of {0, 0}.
    tables.resize(TABLE_SIZE, vector<pair<int, int>>(TABLE_SIZE));

    // Loops through each row of the tables.
    for (int i = 0; i < TABLE_SIZE; ++i) {
        // Initializes a position counter used to place pairs in the row.
        int pos = 0;

        // Sets the first pair in the current row to {0, 0}.
        tables[i][pos++] = { 0, 0 };

        // Loops through ASCII values of lowercase letters (from 'a' to 'z').
        for (int j = 97; j <= 122; ++j) {
            // Assigns each character a pair, starting at the current position, then increments the position.
            tables[i][pos++] = { j, 0 };
        }

        // Loops through ASCII values of uppercase letters (from 'A' to 'Z').
        for (int j = 65; j <= 90; ++j) {
            // Similar to lowercase letters, assigns each uppercase character a pair and increments the position.
            tables[i][pos++] = { j, 0 };
        }

        // Loops to potentially fill up remaining positions in the row based on TABLE_SIZE.
        for (int j = 0; j < TABLE_SIZE; ++j) {
            // Includes characters that are not '0', not uppercase ('A'-'Z'), and not lowercase ('a'-'z') ASCII values.
            if ((j != 0) && (j < 65 || j > 90) && (j < 97 || j > 122)) {
                tables[i][pos++] = { j, 0 };
            }
        }
    }
}

// Defines a function named `get_code` that takes an integer `position` and returns a string.
string get_code(int position) {
    // Initializes a string variable `code` with the value "0".
    string code = "0";

    // Enters a loop that continues as long as the length of `code` is less than `position + 1`.
    while (code.length() < position + 1) {
        // Prepends "1" to the current value of `code` at each iteration.
        code = "1" + code;
    }

    // Returns the modified `code` string after exiting the loop.
    return code;
}

// Defines a function `rle_compression` that performs run-length encoding on a given input string.
string rle_compression(const string& input) {
    // Initializes an empty string `output` to hold the compressed result.
    string output;
    // Initializes an index `i` to keep track of the current position in the input string.
    int i = 0;

    // Continues looping as long as `i` is less than the length of the input string.
    while (i < input.length()) {
        // Stores the current character from the input string.
        char current_char = input[i];
        // Initializes a counter `run_length` to track the number of consecutive occurrences of `current_char`.
        int run_length = 1;

        // Checks if the next character in the input string matches `current_char` and ensures run_length does not exceed 18.
        while (i + run_length < input.length() && input[i + run_length] == current_char && run_length < 18) {
            run_length++;  // Increment the run length counter if conditions are met.
        }

        // If the `run_length` is less than 3, add the character `current_char` to the output for each occurrence.
        if (run_length < 3) {
            for (int j = 0; j < run_length; ++j) {
                output += current_char;
            }
        }
        else {
            // If `run_length` is 3 or more, always add three occurrences of `current_char` to the output.
            output += current_char;
            output += current_char;
            output += current_char;

            // Calculate the number of additional occurrences after the initial three.
            int remainingCount = run_length - 3;
            // Limit the remaining count to a maximum of 15 (presumably for encoding purposes).
            if (remainingCount > 15) {
                remainingCount = 15;
            }

            // Convert the remaining count to a binary string using a bitset of 4 bits and append it to the output.
            output += bitset<4>(remainingCount).to_string();
        }

        // Increment `i` by `run_length` to move to the next group of characters.
        i += run_length;
    }

    // Return the compressed output string.
    return output;
}

// Defines a function `rle_decompression` that decompresses an input string encoded using a specific RLE scheme.
string rle_decompression(const string& input) {
    // Uses a stringstream to efficiently build the decompressed output.
    stringstream decompressed;
    // Retrieves the length of the input string.
    int n = input.length();

    // Loops through the input string using index `i`.
    for (int i = 0; i < n;) {
        // Stores the current character from the input.
        char currentChar = input[i];

        // Checks if there is a run-length encoding pattern starting at this position.
        // This pattern is identified by three consecutive characters followed by a 4-bit binary sequence.
        if (i + 3 < n && input[i] == input[i + 1] && input[i + 1] == input[i + 2]) {
            // Converts the next 4 characters, which represent the binary number, from binary to an unsigned long.
            int remainingCount = bitset<4>(input.substr(i + 3, 4)).to_ulong();

            // Writes the current character three times to the decompressed output, as per the encoding scheme.
            decompressed << currentChar << currentChar << currentChar;

            // Appends the remaining occurrences of the current character to the decompressed output.
            for (int j = 0; j < remainingCount; j++) {
                decompressed << currentChar;
            }
            // Moves the index `i` forward by 7 positions to skip the current encoded sequence.
            i += 7;
        }
        else {
            // If there's no encoding pattern, simply append the current character to the output.
            decompressed << currentChar;
            // Increment the index `i` by 1 to continue to the next character.
            i++;
        }
    }

    // Converts the contents of the stringstream to a string and returns it.
    return decompressed.str();
}

// Defines a function `mnp7_compression` that performs custom compression on a given input string.
string mnp7_compression(string& input) {
    // Initializes a 2D vector to store pairs of integers for compression look-up tables.
    vector<vector<pair<int, int>>> tables;
    // Calls `initialize_code_tables` to set up the initial state of `tables`.
    initialize_code_tables(tables);
    // Uses a stringstream to efficiently build the compressed output.
    stringstream compressed;
    // Declares variables to track the current and previous characters processed.
    char prev_char = 0;
    // Initializes a counter for the appearances of consecutive characters.
    int appearances = 0;

    // Iterates over each character in the input string.
    for (int i = 0; i < input.length(); i++) {

        // Checks if there have been three consecutive appearances to process a nibble.
        if (appearances == 3) {
            // Extracts a 4-character substring starting from the current index.
            string nibble = input.substr(i, 4);
            // Converts the substring to a position index using a bitset.
            int position = bitset<4>(nibble).to_ulong();
            // Retrieves a corresponding code from the compression tables.
            string code = get_code(position);
            // Appends the retrieved code to the compressed output.
            compressed << code;
            // Prints debug information about the current nibble and its encoding.
            cout << "Nibble: " << nibble << " | Code: " << code << " | Position: " << position << endl;
            // Resets the appearances counter and increments the index to skip the processed nibble.
            appearances = 0;
            i += 3;
            continue;
        }

        // Updates `curr_char` to the current character in the input string.
        char curr_char = input[i];

        // Checks if the current character matches the previous one to count appearances.
        if (curr_char == prev_char) {
            appearances += 1;
        }
        else {
            // Resets appearances count if the current character differs from the previous one.
            appearances = 1;
        }

        // References the table corresponding to the current character for lookup.
        auto& table = tables[prev_char];
        // Finds the previous character's position in the current character's table.
        auto it = find_if(table.begin(), table.end(), [curr_char](const pair<int, int>& p) {
            return p.first == curr_char;
        });

        // Checks if the previous character was found in the table.
        if (it != table.end()) {
            // Calculates the position of the found character pair in the table.
            int position = distance(table.begin(), it);
            // Retrieves the compression code for this position.
            string code = get_code(position);
            // Prints debug information about the compression step.
            cout << "Characters: " << prev_char << curr_char << " | Code: " << code << " | Position: " << position << endl;
            // Appends the retrieved code to the compressed output.
            compressed << code;
            // Increments the use-count of this character pair in the table.
            it->second++;
            // Re-sorts the table based on the most frequently used pairs.
            sort(table.begin(), table.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;
            });
        }

        // Updates `prev_char` to the current character for the next iteration.
        prev_char = curr_char;
    }

    // Converts the contents of the stringstream to a string and returns it.
    return compressed.str();
}

// Defines a function for decompressing a string based on the MNP7 custom encoding scheme.
string mnp7_decompression(const string& input) {
    // Initializes a 2D vector of integer pairs as tables for decompression look-up.
    vector<vector<pair<int, int>>> tables;
    // Populates the tables with initial values for each character.
    initialize_code_tables(tables);
    // Uses a stringstream to build the decompressed string efficiently.
    stringstream decompressed;
    // Index to track the current position in the input string.
    int i = 0;
    // Variable to hold the previously processed character.
    char prev_char = 0;
    // Counter for consecutive appearances of the same character.
    int appearances = 0;

    // Processes the input string until the end is reached.
    while (i < input.length()) {

        // Resets position index for each new character sequence.
        int position = 0;
        // Counts '1's to determine the position in the decompression table.
        while (input[i++] == '1') {
            position += 1;
        }

        // Checks if the sequence appears for the third time consecutively.
        if (appearances == 3) {
            // Creates a 4-bit binary number (nibble) based on the count of '1's.
            bitset<4> nibble(position);
            // Appends the binary string of the nibble to the decompressed output.
            decompressed << nibble;
            // Resets the appearance counter and continues to the next loop iteration.
            appearances = 0;
            continue;
        }

        // Retrieves the character from the table based on the previous character and the calculated position.
        char curr_char = tables[prev_char][position].first;

        // Increments or resets the appearance counter based on whether the current and previous characters match.
        if (curr_char == prev_char) {
            appearances += 1;
        }
        else {
            appearances = 1;
        }

        // Appends the current character to the decompressed string.
        decompressed << curr_char;
        // Retrieves the table specific to the previous character.
        auto& table = tables[prev_char];

        // Finds the current character in the table to update its frequency.
        auto it = find_if(table.begin(), table.end(), [curr_char](const pair<int, int>& p) {
            return p.first == curr_char;
            });

        // If the character is found, increments its frequency.
        if (it != table.end()) {
            it->second++;
            // Sorts the table by frequency to prioritize commonly used characters.
            sort(table.begin(), table.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
                return a.second > b.second;
                });
        }
        // Updates the previous character to the current one for the next loop iteration.
        prev_char = curr_char;
    }

    // Returns the fully decompressed string.
    return decompressed.str();
}

int main() {
    string input = "aaaabbbbbccccddddeeeeffffgggghhhhiiii";
    string stage1 = rle_compression(input);
    string stage2 = mnp7_compression(stage1);
    string stage2_decompression = mnp7_decompression(stage2);
    string stage1_decompression = rle_decompression(stage2_decompression);

    cout << endl << endl;
    if (input == stage1_decompression) {
        cout << "Compression/decompression successful!" << endl;
    }
    else {
        cout << "Compression/decompression unsuccessful!" << endl;
    }
    cout << "Input: " << input << endl;
    cout << "Output: " << stage1_decompression << endl;
    cout << "Stage 2 compression size (bits): " << stage2.length() << " | RLE compression size (~bits): " << stage1.length() * 8 << endl;
    cout << "Compression factor: ~" << stage1.length() * 8 / stage2.length() << endl;

    return 0;
}