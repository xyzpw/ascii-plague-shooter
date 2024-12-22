#include <random>
#include "random_utils.h"

std::mt19937 mtRandGen(std::random_device{}());

std::uniform_int_distribution<int> uniformIntDist(
    0, std::numeric_limits<int>::max()
);

int randInt()
{
    return uniformIntDist(mtRandGen);
}

int randIntInRange(int min, int max)
{
    if (min > max){
        std::swap(min, max);
    }

    int range = max - min + 1;

    int nMax = std::numeric_limits<int>::max() / range;

    int result;
    do {
        result = randInt();
    } while (result >= nMax * range);
    return result % range + min;
}

double randNormalDist(double mean, double sd)
{
    std::normal_distribution<double> dist(mean, sd);
    return dist(mtRandGen);
}

int randBinomialDist(int value, double probability)
{
    std::binomial_distribution<int> dist(value, probability);
    return dist(mtRandGen);
}

/*
 Generates a random number from 0 to 1 and returns true if it is less than
 or equal to the specified probability.
*/
bool checkProbability(double p)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double rProb = dist(mtRandGen);
    return rProb <= p;
}