#include <components/esm3/loaddial.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"

#include "../mwworld/esmstore.hpp"
#include "../mwworld/store.hpp"

#include "keywordsearch.hpp"

#include "hypertextparser.hpp"

namespace MWDialogue
{
    namespace HyperTextParser
    {
        std::vector<Token> parseHyperText(const std::string& text)
        {
            std::vector<Token> result;
            size_t pos_end = std::string::npos, iteration_pos = 0;
            for (;;)
            {
                size_t pos_begin = text.find('@', iteration_pos);
                if (pos_begin != std::string::npos)
                    pos_end = text.find('#', pos_begin);

                if (pos_begin != std::string::npos && pos_end != std::string::npos)
                {
                    if (pos_begin != iteration_pos)
                        tokenizeKeywords(text.substr(iteration_pos, pos_begin - iteration_pos), result);

                    std::string link = text.substr(pos_begin + 1, pos_end - pos_begin - 1);
                    result.emplace_back(link, Token::ExplicitLink);

                    iteration_pos = pos_end + 1;
                }
                else
                {
                    if (iteration_pos != text.size())
                        tokenizeKeywords(text.substr(iteration_pos), result);
                    break;
                }
            }

            return result;
        }

        void tokenizeKeywords(const std::string& text, std::vector<Token>& tokens)
        {
            const auto& keywordSearch
                = MWBase::Environment::get().getESMStore()->get<ESM::Dialogue>().getDialogIdKeywordSearch();

            std::vector<KeywordSearch<int /*unused*/>::Match> matches;
            keywordSearch.highlightKeywords(text.begin(), text.end(), matches);

            for (const auto& match : matches)
            {
                tokens.emplace_back(std::string(match.mBeg, match.mEnd), Token::ImplicitKeyword);
            }
        }

        size_t removePseudoAsterisks(std::string& phrase)
        {
            size_t pseudoAsterisksCount = 0;

            if (!phrase.empty())
            {
                std::string::reverse_iterator rit = phrase.rbegin();

                const char specialPseudoAsteriskCharacter = 127;
                while (rit != phrase.rend() && *rit == specialPseudoAsteriskCharacter)
                {
                    pseudoAsterisksCount++;
                    ++rit;
                }
            }

            phrase = phrase.substr(0, phrase.length() - pseudoAsterisksCount);

            return pseudoAsterisksCount;
        }
    }
}
