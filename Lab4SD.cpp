#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

struct DictWord
{
    std::string word;
    std::array<unsigned char, 33> freq{};
    int lettersCount = 0;
};

int getRussianLetterIndex(unsigned char b1, unsigned char b2)
{
    // а-п: 0xD0 0xB0..0xBF
    if (b1 == 0xD0 && b2 >= 0xB0 && b2 <= 0xBF)
    {
        int idx = b2 - 0xB0;      // 0..15
        if (idx >= 5) idx++;      // после 'е' (5) вставляем 'ё' (6)
        return idx;               // 'а'=0, 'б'=1, ..., 'е'=5, 'ё'=6, 'ж'=7, ..., 'п'=16
    }
    // р-я: 0xD1 0x80..0x8F
    if (b1 == 0xD1 && b2 >= 0x80 && b2 <= 0x8F)
    {
        return 17 + (b2 - 0x80);  // 'р'=17, 'с'=18, ..., 'я'=32
    }
    // ё: 0xD1 0x91 
    if (b1 == 0xD1 && b2 == 0x91)
        return 6;
    return -1;
}

std::array<unsigned char, 33> makeFreq(const std::string& word , int& lettersCount)
{
    std::array<unsigned char, 33> freq{};
    freq.fill(0);

    lettersCount = 0;

    for (size_t i = 0; i + 1 < word.size();)
    {
        unsigned char b1 = static_cast<unsigned char>(word[i]);
        unsigned char b2 = static_cast<unsigned char>(word[i + 1]);

        int idx = getRussianLetterIndex(b1, b2);

        if (idx >= 0)
        {
            freq[idx]++;
            lettersCount++;
            i += 2;
        }
        else
        {
            i++;
        }
    }

    return freq;
}

bool canBuild(const std::array<unsigned char, 33>& wordFreq , const std::array<unsigned char, 33>& sourceFreq)
{
    for (int i = 0; i < 33; i++)
    {
        if (wordFreq[i] > sourceFreq[i])
            return false;
    }

    return true;
}

int main()
{
    std::ifstream fin("nouns.txt");

    if (!fin)
    {
        std::cout << "Cannot open nouns.txt\n";
        return 1;
    }

    std::vector<DictWord> dictionary;

    std::string line;

    while (std::getline(fin, line))
    {
        if (line.empty())
            continue;

        DictWord w;

        w.word = line;
        w.freq = makeFreq(line, w.lettersCount);

        dictionary.push_back(std::move(w));
    }

    // Сортируем
    std::sort(dictionary.begin() , dictionary.end() , [](const DictWord& a, const DictWord& b)
        {
            if (a.lettersCount != b.lettersCount)
                return a.lettersCount > b.lettersCount;

            return a.word < b.word;
        });

    std::cout << "Dictionary loaded: "
              << dictionary.size()
              << " words\n";

    while (true)
    {
        std::cout << "\nInput word (exit): ";

        std::string source;

        std::getline(std::cin, source);

        if (source == "exit")
            break;

        int sourceLength;

        auto sourceFreq =
            makeFreq(source, sourceLength);

        size_t found = 0;

        for (const auto& word : dictionary)
        {
            if (word.lettersCount > sourceLength)
                continue;

            if (canBuild(word.freq, sourceFreq))
            {
                std::cout << word.word << '\n';
                found++;
            }
        }

        std::cout << "\nFound: "
                  << found
                  << '\n';
    }

    return 0;
}