#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/**
 * Print instructions for using the program.
 */
void printMenu();

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext);

int main() {
  Random::seed(time(NULL));
  string command;

  // Load the dictionary from file
  vector<string> dictionary;
  ifstream dict("dictionary.txt");
  string word;
  while (getline(dict, word)) {
    dictionary.push_back(word);
  }
  dict.close();

  vector<string> quadgrams;
  vector<int> counts;

  ifstream quadFile("english_quadgrams.txt");  // read quad scores file

  string quad;  // read each quad
  while (getline(quadFile, quad)) {
    size_t comma = quad.find(',');
    if (comma != string::npos) {
      quadgrams.push_back(
          quad.substr(0, comma));  // extract the string, which is the beginning
                                   // of the string till the comma
      counts.push_back(stoi(
          quad.substr(comma + 1)));  // get the count that's after the comma
    }
  }
  quadFile.close();

  QuadgramScorer scorer(quadgrams, counts);

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }

    if (command == "C" || command == "c") {
      caesarEncryptCommand();
    }

    if (command == "D" || command == "d") {
      caesarDecryptCommand(dictionary);
    }

    if (command == "E" || command == "e") {
      computeEnglishnessCommand(scorer);
    }

    if (command == "S" || command == "s") {
      decryptSubstCipherCommand(scorer);
    }

    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    if (command == "F" || command == "f") {
      string inputFile;
      cout << "Enter input file name: ";
      getline(cin, inputFile);
      ifstream inFile(inputFile);

      string outputFile;
      cout << "Enter output file name: ";
      getline(cin, outputFile);

      // Read and store each line of the file:
      string ciphertext;
      string line;
      while (getline(inFile, line)) {
        ciphertext += line + "\n";  // Keep line breaks for formatting
      }
      inFile.close();

      // Decrypt the inputted text from the file:
      vector<char> bestKey = decryptSubstCipher(scorer, ciphertext);
      string decryptedText = applySubstCipher(bestKey, ciphertext);

      // Lastly, write the decrypted text into the output file:
      ofstream outFile(outputFile);
      outFile << decryptedText;
      outFile.close();
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  // TODO: student
  if (!isalpha(c)) {
    return c;  // For chars that arent letters, do nothing
  }

  int num = ALPHABET.find(
      toupper(c));  // Find the corresponding index to the input char

  if (num != string::npos) {           // If valid input
    num = (num + (amount % 26)) % 26;  // rotate + ensure it wraps around
    if (num < 0) {                     // If num is negative
      num += 26;                       // Turn it into a positive num
    }
    return ALPHABET[num];
  }

  return c;  // return original char if not found in ALPHABET
}

string rot(const string& line, int amount) {
  // TODO: student
  string result = "";

  for (size_t i = 0; i < line.size(); i++) {
    if (isalpha(line[i])) {  // if element is a letter...
      char upperChar = toupper(line[i]);
      char rotatedChar = rot(upperChar, amount);
      result += rotatedChar;  // Call the above rot function to do the rotation
    } else if (isspace(line[i])) {  // If there's a space, keep the space
      result += ' ';
    }
    // For chars that aren't letters or spaces (like punctuation), then they are
    // skipped and not added to the resulting encrypted string
  }
  return result;
}

void caesarEncryptCommand() {  // Helper function for the "c" command
  // TODO: student
  string text;
  cout << "Enter text to encrypt: ";
  getline(cin, text);  // Get the string to encrypt from user

  string shiftString;
  cout << "Enter shift amount: ";  // Get the shift amount:
  getline(cin, shiftString);
  int shift = stoi(shiftString);  // Convert shift num to int

  string encryptedText = rot(text, shift);  // Rotate text to encrypt
  cout << "Encrypted Text: " << encryptedText << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& strings, int amount) {
  // Loop through each string in the vector:
  for (auto& i : strings) {
    if (!i.empty()) {
      i = ::rot(i, amount);  // Rotate the current string
    }
  }
}

string clean(const string& s) {
  string result;  // Store the cleaned out string

  for (char c : s) {
    if (isalpha(c)) {
      result += toupper(c);  // Convert to uppercase if it's a letter
    }
    // Else: other characters like punctuation, numbers, spaces are ignored
  }

  return result;
}

// Splits the string into seperate words and store in a vector:
vector<string> splitBySpaces(const string& s) {
  vector<string> words;  // Vector to store each word

  stringstream ss(s);
  string word;
  while (ss >> word) {      // Go through entire string
    words.push_back(word);  // push back each word to vector
  }

  return words;
}

// Takes the vector of individual words and creates one string
string joinWithSpaces(const vector<string>& words) {
  string result;

  for (size_t i = 0; i < words.size(); i++) {
    result += words[i];
    if (i < words.size() - 1) {  // For words that aren't the last word...
      result += " ";             // Include a space after them
    }
  }

  return result;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  int dictWords = 0;  // Stores how many words were found in the dict

  for (const auto& word : words) {       // Iterates through each word
    for (const auto& dictWord : dict) {  // Iterates through each dictionary
                                         // word
      if (word == dictWord) {  // if the word exists in the dictionary...
        dictWords++;           // increment the counter
      }
    }
  }

  return dictWords;
}

void caesarDecryptCommand(const vector<string>& dict) {
  string text;
  cout << "Enter text to decrypt: ";
  getline(cin, text);  // Get encrypted text from user

  // Split the original text by spaces
  vector<string> words = splitBySpaces(text);

  // Clean each word and store only non-empty cleaned words in a new vector
  vector<string> cleanedWords;
  for (auto& word : words) {
    string cleaned = clean(word);
    if (!cleaned.empty()) {
      cleanedWords.push_back(cleaned);
    }
  }

  if (cleanedWords.empty()) {
    cout << "No good decryptions found" << endl;
    return;
  }

  bool found = false;

  // Try each shift from 0 to 25
  for (int i = 0; i < 26; i++) {
    vector<string> rotatedWords = cleanedWords;
    rot(rotatedWords, i);  // Rotate each word

    int wordCount = numWordsIn(rotatedWords, dict);

    // Check if more than half of the words are existing in the dict
    if (wordCount > static_cast<int>(rotatedWords.size()) / 2) {
      cout << joinWithSpaces(rotatedWords) << endl;
      found = true;
    }
  }

  if (!found) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string result;  // Will store the transformed chars

  for (char i : s) {              // loop through string
    bool origLower = islower(i);  // Track if the char was originally lower case

    if (isalpha(i)) {  // if its a letter..
      char upperChar = toupper(i);
      char matching =
          cipher[upperChar - 'A'];  // Find the char in 'cipher' at that index

      result += matching;

    } else {
      result += i;  // Non-letters are left the same
    }
  }

  return result;
}

void applyRandSubstCipherCommand() {
  cout << "Enter text to encrypt: ";
  string input;
  getline(cin, input);

  vector<char> cipher = genRandomSubstCipher();  // Generate rand sub cipher
  string encrypted = applySubstCipher(cipher, input);  // Encrypt text

  cout << "Encrypted text: " << encrypted << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  double totalScore = 0.0;

  // Loop for breaking up string into quadgrams:
  for (size_t i = 0; i <= s.size() - 4;
       i++) {  // stop when there are fewer than 4 characters left to form a
               // complete quadgram
    string quad = s.substr(i, 4);

    totalScore += scorer.getScore(quad);
  }

  return totalScore;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  cout << "Enter text to compute Englishness: ";
  string input;
  getline(cin, input);

  input = clean(input);  // clean the string

  if (input.size() < 4) {
    cout << "Error: Too small of an input" << endl;
    return;
  }

  double score = scoreString(scorer, input);  // get englishness score

  cout << "The computed Englishness is: " << score << endl;
}

// Helper function for decryptSubstCipher:
vector<char> findBestScore(const QuadgramScorer& scorer,
                           const string& ciphertext) {
  vector<char> key = genRandomSubstCipher();  // generate random key initally
  string decText =
      applySubstCipher(key, ciphertext);  // decrypt text with current key
  double bestScore = scoreString(scorer, decText);  // current best score

  int failedSwaps = 0;  // Count consecutive failed swaps

  while (failedSwaps < 1000) {  // max 1000 swaps
    // Get random indices (which represent letters)
    int letter1 = Random::randInt(25);
    int letter2;
    do {  // Get a completely different random index for letter2:
      letter2 = Random::randInt(25);
    } while (letter2 == letter1);

    // Swap letters
    swap(key[letter1], key[letter2]);
    string newDec = applySubstCipher(key, ciphertext);  // decrypt w/ new key
    double newScore = scoreString(scorer, newDec);

    // Keep the change only if the score improves
    if (newScore > bestScore) {
      bestScore = newScore;
      failedSwaps = 0;  // Reset failure count if we improve
    } else {
      swap(key[letter1], key[letter2]);  // Undo swap if it didn't help
      failedSwaps++;                     // Count failed swaps
    }
  }

  return key;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  vector<char> bestSub;
  double bestOverall = -1e9;  // best score will start low

  string cleanedText = clean(ciphertext);

  for (size_t i = 0; i < 25; i++) {  //"run hill" algorithm 25 times
    vector<char> maybeKey = findBestScore(scorer, cleanedText);  // Run the 1000 swaps to find the best possible sub key
    string decrypted = applySubstCipher(maybeKey, cleanedText);  // Apply key to convert encrypted text back to English
    double maybeScore = scoreString(scorer, decrypted);  // Compute Englishness score of new decrypted text

    if (maybeScore > bestOverall) {  // If potential score is better than the
                                     // current best score
      bestOverall = maybeScore;      // then it will be the new best score
      bestSub = maybeKey;            // Keep the best key found
    }
  }

  return bestSub;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  cout << "Enter text to decrypt: ";
  string input;
  getline(cin, input);

  // Decrypt the text using the best key
  vector<char> bestKey = decryptSubstCipher(scorer, input);

  // Apply the best key to actually decrypt the text
  string decryptedText = applySubstCipher(bestKey, input);

  cout << "Decrypted text: " << decryptedText << endl;
}

#pragma endregion SubstDec
