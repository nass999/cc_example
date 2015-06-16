#include <iostream>
#include <fstream>
#include <list>
#include <stdio.h> // for EOF
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cstdint>
#include <cctype>

class Scanner final
{
private:
    std::list<std::ifstream> streams; // list of input file streams
    int getInternal() // internal get next character function
    {
        while(!streams.empty()) // if there are any streams left then we can get a character
        {
            int retval = streams.front().get(); // get next character from current stream
            if(retval != EOF) // if the stream is not exausted
                return retval;
            streams.pop_front(); // go to next stream because the current one is exausted
        }
        return EOF;
    }
    int peekedChar = EOF; // peeked character cache (EOF means the cache is empty)
    int peek() // exactly like istream::peak()
    {
        if(peekedChar == EOF)
            peekedChar = getInternal();
        return peekedChar;
    }
    int get() // exactly like istream::get()
    {
        if(peekedChar == EOF)
            peekedChar = getInternal();
        int retval = peekedChar;
        peekedChar = EOF;
        return retval;
    }
    std::vector<std::uint64_t> lineWordCounts; // should replace vector with rope
    std::map<std::string, std::uint64_t> wordMap; // word map : a map keeps keys in sorted order automatically
    void insertLineWordCount(std::uint64_t lineWordCount, std::ostream &runningMedianOStream)
    {
        std::size_t insertPos;
        for(insertPos = 0; insertPos < lineWordCounts.size(); insertPos++) // find spot to insert : could change to binary search
        {
            if(lineWordCounts[insertPos] > lineWordCount)
                break;
        }
        lineWordCounts.insert(lineWordCounts.begin() + insertPos, lineWordCount);
        std::uint64_t medianLineCountTimesTwo; // remove float round and outputing issues by using std::uint64_t
        if(lineWordCounts.size() % 2 == 0)
        {
            medianLineCountTimesTwo = lineWordCounts[(lineWordCounts.size() - 1) / 2] + lineWordCounts[1 + (lineWordCounts.size() - 1) / 2];
        }
        else
        {
            medianLineCountTimesTwo = 2 * lineWordCounts[lineWordCounts.size() / 2];
        }
        runningMedianOStream << medianLineCountTimesTwo / 2 << (medianLineCountTimesTwo % 2 == 0 ? ".0\n" : ".5\n");
    }
public:
    Scanner(std::list<std::ifstream> streams)
        : streams(std::move(streams))
    {
    }
    void run(std::ostream &runningMedianOStream)
    {
        std::string currentWord = "";
        std::uint64_t lineWordCount = 0;
        for(;;)
        {
            if(peek() == '\'' || peek() == '-')
            {
                get();
                continue;
            }
            if(peek() != EOF && std::isalpha(peek()))
            {
                char ch = (char)std::tolower(get());
                currentWord += ch;
            }
            else
            {
                if(!currentWord.empty())
                {
                    wordMap[currentWord]++;
                    currentWord.clear();
                    lineWordCount++;
                }
                if(peek() == '\r' || peek() == '\n' || peek() == '\f' || peek() == EOF) // if at end of line/file
                {
                    if(lineWordCount != 0 || peek() != EOF)
                        insertLineWordCount(lineWordCount, runningMedianOStream);
                    lineWordCount = 0;
                }
                if(get() == EOF)
                    break;
            }
        }
    }
    void dumpWordCount(std::ostream &os)
    {
        for(auto &p : wordMap) // word map is stored in sorted order
        {
            os << std::get<0>(p) << " " << std::get<1>(p) << "\n";
        }
    }
};

int main(int argc, char **argv)
{
    std::set<std::string> fileNames; // set automatically sorts
    for(int i = 1; i < argc; i++)
    {
        fileNames.insert(argv[i]);
    }
    std::list<std::ifstream> streams;
    for(const std::string &fileName : fileNames) // set automatically sorts
    {
        streams.emplace_back(fileName); // construct in-place
        if(!streams.back())
        {
            std::cerr << "can't open " << fileName << std::endl;
            return 1;
        }
    }
    std::ofstream wordCountOStream("./wc_output/wc_result.txt");
    if(!wordCountOStream)
    {
        std::cerr << "can't open " << "./wc_output/wc_result.txt" << std::endl;
        return 1;
    }
    std::ofstream runningMedianOStream("./wc_output/med_result.txt");
    if(!runningMedianOStream)
    {
        std::cerr << "can't open " << "./wc_output/med_result.txt" << std::endl;
        return 1;
    }
    Scanner scanner(std::move(streams));
    scanner.run(runningMedianOStream);
    scanner.dumpWordCount(wordCountOStream);
    return 0;
}
