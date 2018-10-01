#include "stdafx.h"

#include <boost/tokenizer.hpp>
#include <regex>
#include <list>

#include "arglist.h"


namespace {

typedef  boost::escaped_list_separator<char>    char_separator_t;
typedef  boost::tokenizer< char_separator_t >   char_tokenizer_t;

ArgsList  getArgsList(const std::string&  argsStr)
{
    char_tokenizer_t  token(argsStr, char_separator_t("", " \t", "\""));
    ArgsList  argsList;

    for (char_tokenizer_t::iterator it = token.begin(); it != token.end(); ++it)
    {
        if (*it != "")
            argsList.push_back(*it);
    }

    return argsList;
}

} // anonymous namespace


static const std::regex  versionRe("^-([2,3])(?:\\.(\\d+))?$");

Options::Options(const std::string& cmdline) :
    pyMajorVersion(-1),
    pyMinorVersion(-1),
    global(false),
    showHelp(false),
    runModule(false)
{
    args = getArgsList( cmdline );

    bool  globalByDefault = true;

    for (auto it = args.begin(); it != args.end();)
    {
        if (*it == "--global" || *it == "-g")
        {
            global = true;
            globalByDefault = false;
            it = args.erase(it);
            continue;
        }

        if (*it == "--local" || *it == "-l")
        {
            global = false;
            globalByDefault = false;
            it = args.erase(it);
            continue;
        }

        std::smatch  mres;
        if (std::regex_match(*it, mres, versionRe))
        {
            pyMajorVersion = atol(std::string(mres[1].first, mres[1].second).c_str());

            if (mres[2].matched)
            {
                pyMinorVersion = atol(std::string(mres[2].first, mres[2].second).c_str());
            }

            it = args.erase(it);
            continue;
        }

        break;
    }

    if ( args.empty() && globalByDefault)
    {
        global = true;
    }

    for (auto it = args.begin(); it != args.end();)
    {

        if (*it == "--help" || *it == "-h")
        {
            showHelp = true;
            it = args.erase(it);
            continue;
        }

        if (*it == "--module" || *it == "-m")
        {
            runModule = true;
            it = args.erase(it);
            continue;
        }

        break;
    }
}


