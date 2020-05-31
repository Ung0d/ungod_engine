#include "ungod/base/Utility.h"
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

namespace ungod
{
    std::mt19937 NumberGenerator::gen((int)time(0));

    int NumberGenerator::getRandBetw(const int a, const int b)
    {
        std::uniform_int_distribution<> dist(a,b);
        return dist(gen);
    }

    float NumberGenerator::getFloatRandBetw(const float a, const float b)
    {
        std::uniform_real_distribution<> dist(a,b);
        return (float)dist(gen);
    }

    float NumberGenerator::getNormRand(const float mu, const float rho)
    {
        std::normal_distribution<float> dist(mu, rho);
        return (float)dist(gen);
    }

    bool NumberGenerator::getRandBool()
    {
        return rand() % 2;
    }

    bool NumberGenerator::getRandBool(const float true_prob)
    {
        std::uniform_int_distribution<> dist(0,100);
        return dist(gen) < 100*true_prob;
    }


    std::vector<std::string> enumerateDirectory(const std::string& pathToDir)
    {
        boost::filesystem::path p(pathToDir);
        std::vector<std::string> enumFiles;

        if (boost::filesystem::is_directory(p)) 
        {
            for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
                if (boost::filesystem::is_regular_file(entry.path())) 
                    enumFiles.emplace_back(entry.path().filename().string());
        }
        else
        {
            ungod::Logger::warning(pathToDir, "is no directory. Can not enumerate.");
        }

        return enumFiles;
    }
}
