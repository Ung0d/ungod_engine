#include "ungod/base/Utility.h"
#include <time.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "ungod/base/Entity.h"
#include "ungod/base/Transform.h"

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


    bool isBelow(Entity l, Entity r)
    {
        //TRUE == "left is rendered FIRST, right after"

        const TransformComponent& left = l.get<TransformComponent>();
        const TransformComponent& right = r.get<TransformComponent>();

        //test if right.rightAnchor is RIGHT of the line between the two anchors of left
        sf::Vector2 lla = left.getLeftAnchor();
        sf::Vector2 lra = left.getRightAnchor();
        sf::Vector2 rla = right.getLeftAnchor();
        sf::Vector2 rra = right.getRightAnchor();

        /*if (sign(lra, rra ,lla) > 0 && sign(lra, rla ,lla) > 0)
            return true;

        if (sign(rra, lra ,rla) >= 0 && sign(rra, lla ,rla) >= 0)
            return false;*/

        return std::max(rra.y, rla.y) > std::max(lla.y, lra.y);
    };
}
