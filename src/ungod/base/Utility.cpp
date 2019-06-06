#include "ungod/base/Utility.h"
#include <time.h>

namespace ungod
{
    std::mt19937 NumberGenerator::gen(time(0));

    int NumberGenerator::getRandBetw(const int a, const int b)
    {
        std::uniform_int_distribution<> dist(a,b);
        return dist(gen);
    }

    float NumberGenerator::getFloatRandBetw(const float a, const float b)
    {
        std::uniform_real_distribution<> dist(a,b);
        return dist(gen);
    }

    float NumberGenerator::getNormRand(const float mu, const float rho)
    {
        std::normal_distribution<float> dist(mu, rho);
        return dist(gen);
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
}
