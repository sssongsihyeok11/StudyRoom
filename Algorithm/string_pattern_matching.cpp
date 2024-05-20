#include <iostream>
#include <string>
#include <fstream>
#include <vector>

std::vector<int> mem_for_boyer_moore; //for push_back indexlist in boyer-moore

void find_table(std::string str, int size, int *table) {
    int i;

    for (i = 0; i < 256; i++) // 256: slot in badchar
        table[i] = -1;

    for (i = 0; i < size; i++)
        table[(int)str[i]] = i; // string to int -> put letter in index
}

void boyer_moore(std::string text, std::string pattern) { // implement boyer-moore algorithm using badCharHeuristic table
    int m = pattern.size();
    int n = text.size();

    int table[256];

    find_table(pattern, m, table);

    int start = 0; 
    while (start <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[start + j]) // if text letter matches pattern
            j--; // move left position

        if (j < 0) { // j is in LSB -> pattern found
            std:: cout << "Pattern found at index " << start <<std:: endl;
            mem_for_boyer_moore.push_back(start); // store the index of the pattern occurrence.
            start += (start + m < n) ? m - table[text[start + m]] : 1; // Initializes the next search position in the text index 
                                                                         // after finding the pattern.

        }
        else {
            start +=std:: max(1, j - table[text[start + j]]); // determines the shift distance when a mismatch occurs in the pattern
        }
    }
}

std::vector<int> mem_for_kmp; //for push_back indexlist in kmp

std::vector<int> Make_Table(const std::string& pattern) {
    int patternSize = pattern.size();
    std::vector<int> table(patternSize, 0);
    int j = 0;
    for (int i = 1; i < patternSize; i++) {  // iterate through the pattern to fill the table.
        while (j > 0 && pattern[i] != pattern[j]) {
            j = table[j - 1]; // update j to the length of the matching prefix suffix of the previous character.
        }
        if (pattern[i] == pattern[j]) {
            table[i] = ++j; // set the table entry for position i to the incremented value of j.
        }
    }
    return table;
}

void KMP(const std::string& text, const std::string& pattern) {
    std::vector<int> table = Make_Table(pattern); // create a table using the Make_Table function
    int textSize = text.size();
    int patternSize = pattern.size();
    int i = 0, j = 0;
    while (i < textSize) {
        if (pattern[j] == text[i]) {  // if the characters at positions i and j match, move to the next positions.
            i++;
            j++;
            if (j == patternSize) {
                mem_for_kmp.push_back(i-j); // store the index of the pattern occurrence.
                std::cout << "Pattern found at index " << i - j << std::endl;
                j = table[j - 1]; // update j using the table entry for the previous position.
            }
        } else {
            if (j != 0) {
                j = table[j - 1]; // update j to the length of the matching prefix suffix of the previous character.
            } else {
                i++;
            }
        }
    }
}

long long Q = 17284859009917; // large prime number used for modular arithmetic.
int base = 10; // base value for converting characters to integers.

int modular(const std::string& pattern) {
    int result = 0;
    for (char c : pattern) {
        result = (result * base + (int)c) % Q;  // update the hash value by multiplying the current value by the base,
                                                // adding the integer value of the character, and taking the modulus with Q.
    }
    return result;
}

std::vector<int> mem_for_Rabin_Karp; //for push_back indexlist in rabin_karp

void Rabin_Karp(const std::string& text, const std::string& pattern) {
    int pattern_hash = modular(pattern);
    int pattern_length = pattern.size();
    int text_length = text.size();

    for (int start = 0; start <= text_length - pattern_length; start++) { // iterate through the text to search for occurrences of the pattern.
        std::string temp = text.substr(start, pattern_length);
        int mod = modular(temp); // calculate the hash value of the substring.

        // if the hash values of the substring and the pattern match, and the substrings are equal, a potential match is found.
        if (mod == pattern_hash && temp == pattern) {
            std::cout << "Pattern found at index " << start << std::endl;
            mem_for_Rabin_Karp.push_back(start); // store the index of the pattern occurrence.
        }
    }
}

int main(){
    std::string text; // text
    std::ifstream file("RFC2616_modified.txt"); // open text file
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            text += line + "\n";
        }
        file.close();
    }
    else {
        std :: cout << "file is not open";
        return 1;
    }

    
    std::string PATTERN;
    std::cin >> PATTERN;
   
    std::cout << "start kmp algorithm\n\n";
    KMP(text,PATTERN); // find PATTERN in text using KMP algorithm
    std::cout << "\nindex in pattern \n";
    if (!mem_for_kmp.empty()) std::cout << mem_for_kmp[0];
    for (size_t i = 1; i < mem_for_kmp.size(); i++) {
        std::cout << ", " << mem_for_kmp[i]; // print the index of pattern occurrence
    }
    std::cout << "\nnumber of pattern: " << mem_for_kmp.size() << " nums\n";
    std::cout << "\nend kmp algorithm\n\nstart boyer-moore algorithm\n\n";
    boyer_moore(text, PATTERN); // find PATTERN in text using boyer_moore algorithm
    std::cout << "\n\nindex in pattern \n";
    if (!mem_for_boyer_moore.empty()) std::cout << mem_for_boyer_moore[0];
    for (size_t i = 1; i < mem_for_boyer_moore.size(); i++) {
        std::cout << ", " << mem_for_boyer_moore[i]; // print the index of pattern occurrence
    }
    std::cout << "\nnumber of pattern: " << mem_for_boyer_moore.size() << " nums\n";


    std::cout << "\nstart rabin-karp algorithm\n\n";
    Rabin_Karp(text,PATTERN); // find PATTERN in text using rabin_karp algorithm
    std::cout << "\nindex in pattern \n";
    if (!mem_for_Rabin_Karp.empty()) std::cout << mem_for_kmp[0];
    for (size_t i = 1; i < mem_for_Rabin_Karp.size(); i++) {
        std::cout << ", " << mem_for_Rabin_Karp[i]; // print the index of pattern occurrence
    }
    std::cout << "\nnumber of pattern: " << mem_for_Rabin_Karp.size() << " nums\n";

}