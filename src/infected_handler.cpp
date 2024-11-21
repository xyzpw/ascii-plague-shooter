#include <iostream>
#include <utility>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <thread>
#include "common.h"
#include "math_utils.h"
#include "gameUtilities.h"
#include "infected_handler.h"

bool checkBulletWasFatal(HIT_LOCATION location, int joules);
bool checkClaymoreIsFacingPos(Explosive claymore, std::pair<int, int> coord);
int getDelayedDeathTime(HIT_LOCATION location);
bool checkIsHindered(HIT_LOCATION location);
bool checkShouldDelayedDeath(HIT_LOCATION location, bool isHollowPoint);

// Returns the column and row to where a new infected should spawn, respectively.
std::pair<int, int> getInfectedSpawnPosition(World world)
{
    int colRange = world.mapColumnLimits.second - world.mapColumnLimits.first;
    int col = std::rand() % (colRange+1) + world.mapColumnLimits.first;
    return std::make_pair(col, world.mapRowLimits.first);
}

// Updates each infected's location to follow a player.
void updateInfectedPositions(World& world, Player& player)
{
    if (!player.alive)
        return;
    for (auto& inf : world.infected)
    {
        if (!inf.alive || inf.coordinates == player.coordinates)
            continue;
        if (inf.isHindered && std::rand() % 4 == 3)
            continue;
        if (inf.delayedDeathHitTime.has_value())
        {
            bool isDead = inf.delayedDeathDuration.value() <=
                    std::time(0) - inf.delayedDeathHitTime.value();
            if (isDead){
                inf.markAsDead();
                ++player.killCount;
                continue;
            }
        }

        int deltaCol = player.coordinates.first - inf.coordinates.first;
        int deltaRow = player.coordinates.second - inf.coordinates.second;

        if (deltaCol < 0)
            inf.coordinates.first -= 1;
        else if (deltaCol > 0)
            inf.coordinates.first += 1;
        if (deltaRow > 0)
            inf.coordinates.second += 1;
        else if (deltaRow < 0)
            inf.coordinates.second -= 1;
    }
}

void handleFirearmShot(World& world, Player& player)
{
    if (!player.activeWeapon.canShoot)
        return;
    bool pointBlank = false;

    // Determine if blood splatter should be displayed.
    auto checkShouldSplatter = [&](HIT_LOCATION location, Magazine mag){
        if (mag.cartridgeType == CARTRIDGE_TYPE::CARTRIDGE_22LR)
            return false;
        else if (location == HIT_LOCATION::HEAD && mag.isHollowPoint)
            return true;
        return std::rand() % 50 + 1 <= (mag.isHollowPoint ? 2 : 1);
    };

    auto checkBulletHit = [&](int distance){
        Firearm firearm = player.activeWeapon;
        double decay = firearm.accuracyDecay;
        double pHit = firearm.accuracyScaleFactor * std::exp(-decay * distance);
        if (pHit >= 1.0){
            pointBlank = true;
            return true;
        }
        return static_cast<double>(std::rand()) / RAND_MAX <= pHit;
    };

    int playerCol = player.coordinates.first, playerRow = player.coordinates.second;

    DIRECTION direction = player.facingDirection;
    bool isVert = direction == NORTH || direction == SOUTH;
    std::vector<std::pair<int, int>> coordinatesInRange{};

    auto getEnemyDistance = [&](Infected inf){
        if (isVert)
            return std::abs(player.coordinates.second - inf.coordinates.second);
        return std::abs(player.coordinates.first - inf.coordinates.first);
    };

    // Kinetic energy of the fired bullet; decreases with distance due to air
    // resistance (drag).
    int activeBulletKE = player.activeWeapon.magazine.kineticEnergy;

    std::pair<int, int> bulletCoordinates = player.coordinates;
    auto updateBulletCoordinates = [&]()
    {
        if (direction == NORTH || direction == SOUTH)
            bulletCoordinates.second += direction == SOUTH ? +1 : -1;
        else
            bulletCoordinates.first += direction == EAST ? +1 : -1;
        return checkCoordinatesInsideMap(world, bulletCoordinates);
    };
    updateBulletCoordinates();

    bool isBulletHp = player.activeWeapon.magazine.isHollowPoint;
    // Check each coordinate in the bullet's path to see if it hits an infected.
    while (updateBulletCoordinates() && activeBulletKE > 79)
    {
        activeBulletKE -=
                player.activeWeapon.magazine.kineticEnergyLossPerMeter;
        for (auto& inf : world.infected)
        {
            if (inf.alive && inf.coordinates == bulletCoordinates)
            {
                int playerToInfDistance = getEnemyDistance(inf);
                if (!checkBulletHit(playerToInfDistance))
                    return;

                int keDamage = activeBulletKE;
                activeBulletKE *= isBulletHp ? 0.1 : 0.8;
                keDamage -= activeBulletKE;

                HIT_LOCATION hitLocation = getHitLocation(pointBlank);

                bool wasFatal = isBulletHp &&
                        hitLocation == HIT_LOCATION::HEAD ||
                        checkBulletWasFatal(hitLocation, keDamage);

                if (wasFatal){
                    inf.markAsDead();
                    ++player.killCount;
                }
                else if (checkIsHindered(hitLocation))
                    inf.isHindered = true;

                // Add delayed death to infected if in probability range.
                if (checkShouldDelayedDeath(hitLocation, isBulletHp))
                {
                    if (inf.delayedDeathHitTime.has_value() &&
                            inf.delayedDeathDuration.has_value())
                        inf.delayedDeathDuration.value() /= 2;
                    else{
                        inf.delayedDeathDuration =
                                getDelayedDeathTime(hitLocation);
                        inf.delayedDeathHitTime = std::time(0);
                    }
                }

                // Adding splatter effect to infected when shot.
                if (checkShouldSplatter(hitLocation,
                        player.activeWeapon.magazine))
                {
                    if (inf.deathSplatter.has_value()){
                        auto coords = getSplatterCoordinates(
                                inf.coordinates, hitLocation,
                                player.activeWeapon.magazine,
                                player.facingDirection);
                        for (auto coord : coords)
                            inf.deathSplatter->coordinates.push_back(coord);
                    }
                    else{
                        SplatterEffect splatter{.coordinates=
                            getSplatterCoordinates(
                                inf.coordinates, hitLocation,
                                player.activeWeapon.magazine,
                                player.facingDirection)};
                        inf.deathSplatter = splatter;
                    }
                }
            }
        }
    }
}

void handleGrenadeExplosion(World& world, Player& player, int explosiveId)
{
    // Returns the grenade from `activeExplosives` with `explosiveId`.
    auto getGrenade = [&](){
        int index = world.activeExplosives.size();
        for (int i = 0; i < world.activeExplosives.size(); ++i){
            bool hasId = world.activeExplosives.at(i)._explosiveId.has_value();
            if (hasId && world.activeExplosives.at(i)._explosiveId.value() ==
                    explosiveId)
            {
                index = i;
                break;
            }
        }
        return world.activeExplosives.at(index);
    };


    int kineticEnergy = getGrenade().fragmentKineticEnergy;
    int fragmentCount = getGrenade().fragmentCount;
    int energyLossPerMeter = getGrenade().fragmentKineticEnergyLossPerMeter;

    auto checkWasFatal = [&](int distance){
        double area = computeAreaFromDistance(distance);
        int pascals = computeInverseSquareLaw(getGrenade().explosionPascals,
            distance
        );
        int fragments = getGrenade().fragmentCount / area;

        int energy = kineticEnergy - energyLossPerMeter * distance;
        double explosionLethalProb = computeExplosionLethalProb(pascals);
        double fragmentLethalProb = computeFragmentLethalProb(energy, fragments);
        bool wasFatal = static_cast<double>(std::rand()) / RAND_MAX <= explosionLethalProb;
        if (!wasFatal)
            wasFatal = static_cast<double>(std::rand()) / RAND_MAX <= fragmentLethalProb;
        return wasFatal;
    };

    usleep(getGrenade().explosionDelay * 1e6);

    double playerDistance = getMapPointsDistance(
        player.coordinates, getGrenade().coordinates
    );

    bool wasFatalToPlayer = checkWasFatal(playerDistance);
    if (wasFatalToPlayer){
        player.gameOverMessage = "cause of death: grenade";
        player.alive = false;
        return;
    }
    int pascalAtPlayer = computeInverseSquareLaw(
        getGrenade().explosionPascals, playerDistance
    );

    bool isClose = computeEardrumRuptureProb(pascalAtPlayer);

    std::thread(playAudio, isClose ? getGrenade().explodeCloseAudioFile :
        getGrenade().explodeAudioFile
    ).detach();

    for (auto& inf : world.infected)
    {
        int distance = getMapPointsDistance(
            inf.coordinates, getGrenade().coordinates
        );

        if (checkWasFatal(distance)){
            inf.markAsDead();
            ++player.killCount;
            continue;
        }

        double area = computeAreaFromDistance(distance);
        int fragments = getGrenade().fragmentCount / area;

        for (int i = 0; i < fragments && !inf.isHindered; ++i){
            if (checkIsHindered(getHitLocation(false)))
                inf.isHindered = true;
        }
    }

    for (auto exp = world.activeExplosives.begin(); exp != world.activeExplosives.end(); ++exp)
        if (exp->_explosiveId.has_value() && exp->_explosiveId.value() == explosiveId){
            world.activeExplosives.erase(exp);
            break;
        }
}

void handleClaymoreExplosion(World& world, Player& player, Explosive explosive)
{
    // Claymore explosion fires fragments within a 60 degree arc.
    int fragmentsDegrees = 60;

    std::pair<int, int> claymorePosition = explosive.coordinates;
    DIRECTION claymoreDirection = explosive.facingDirection.value();
    int kineticEnergy = explosive.fragmentKineticEnergy;
    int kineticEnergyLossPerMeter = explosive.fragmentKineticEnergyLossPerMeter;
    int explosionPascals = explosive.explosionPascals;

    // Check if position is within fragments range.
    auto checkIsInFragmentArea = [&](std::pair<int, int> coord){
        bool isHorizontal = claymoreDirection == EAST || claymoreDirection == WEST;
        int distance = getMapPointsDistance(coord, claymorePosition);
        int coordChange = computeCoordinatesChange(coord, claymorePosition, isHorizontal);
        int coordChangeWide = computeCoordinatesChange(coord, claymorePosition, !isHorizontal);
        int areaWidth = getSectorWidthAtDistance(coordChange, fragmentsDegrees);
        bool result = coordChangeWide <= areaWidth/2;
        return result;
    };

    // How many fragments at position.
    auto fragmentCountAtPos = [&](std::pair<int, int> coord){
        if (!checkIsInFragmentArea(coord) || !checkClaymoreIsFacingPos(explosive, coord))
            return 0;
        bool isHorizontal = claymoreDirection == EAST || claymoreDirection == WEST;
        int diff = computeCoordinatesChange(coord, claymorePosition, isHorizontal);
        int area = computeSectorAreaFromDistance(diff, fragmentsDegrees);
        int fragments = area == 0 ? explosive.fragmentCount : explosive.fragmentCount / area;
        return fragments;
    };

    // Check if explosion was lethal.
    auto checkFatal = [&](std::pair<int, int> coord){
        int fragments = fragmentCountAtPos(coord);
        double distance = getMapPointsDistance(coord, claymorePosition);
        int energy = kineticEnergy - kineticEnergyLossPerMeter * distance;
        bool p = computeFragmentLethalProb(energy, fragments);
        bool pEx = computeExplosionLethalProb(
            computeInverseSquareLaw(explosionPascals, distance));
        bool fragmentsLethal = static_cast<double>(std::rand()) / RAND_MAX <= p;
        bool explosionLethal = static_cast<double>(std::rand()) / RAND_MAX <= pEx;
        return fragmentsLethal || explosionLethal;
    };

    player.hasPlantedClaymore = false;
    usleep(explosive.explosionDelay * 1e6);

    bool lethalToPlayer = checkFatal(player.coordinates);
    if (lethalToPlayer){
        player.alive = false;
        player.gameOverMessage = "cause of death: claymore";
        return;
    }
    double pascalsAtPlayer = computeInverseSquareLaw(
        explosionPascals, getMapPointsDistance(
            player.coordinates, claymorePosition));
    double isCloseProb = computeEardrumRuptureProb(pascalsAtPlayer);
    bool isClose = static_cast<double>(std::rand()) / RAND_MAX <= isCloseProb;

    std::thread(playAudio, isClose ? explosive.explodeCloseAudioFile : explosive.explodeAudioFile).detach();
    for (auto ex = world.activeExplosives.begin(); ex != world.activeExplosives.end();)
    {
        if (ex->_explosiveId == explosive._explosiveId){
            world.activeExplosives.erase(ex);
            break;
        }
        else
            ++ex;
    }

    for (auto& inf : world.infected)
    {
        if (checkFatal(inf.coordinates))
        {
            inf.markAsDead();
            ++player.killCount;
            continue;
        }

        // Check if fragments hindered infected.
        int fragments = fragmentCountAtPos(inf.coordinates);
        for (int i = 0; i < fragments && !inf.isHindered; ++i)
            if (checkIsHindered(getHitLocation(false)))
                inf.isHindered = true;
    }
}

bool checkBulletWasFatal(const HIT_LOCATION location, const int joules)
{
    double pFatal = computeDeathProbability(location, joules);
    bool wasFatal = static_cast<double>(std::rand()) / RAND_MAX < pFatal;
    return wasFatal;
}

// Time until bullet causes death if the initial hit was not fatal.
int getDelayedDeathTime(HIT_LOCATION location)
{
    switch (location){
        case HIT_LOCATION::HEAD:{
            return 0;
        }
        case HIT_LOCATION::THORAX:{
            return std::rand() % 9 + 2;
        }
        case HIT_LOCATION::ABDOMEN:{
            return std::rand() % 22 + 5;
        }
        default: {
            return 85 / 2;
        }
    }
}

bool checkShouldDelayedDeath(HIT_LOCATION location, bool isHollowPoint)
{
    int rNum = std::rand() % 100 + 1;
    switch (location){
        case HIT_LOCATION::HEAD:
            return rNum <= 0.9;
        case HIT_LOCATION::THORAX:
            return rNum <= isHollowPoint ? 88 : 50;
        case HIT_LOCATION::ABDOMEN:
            return rNum <= isHollowPoint ? 60 : 20;
        default:
            return rNum <= isHollowPoint ? 30 : 10;
    }
}

bool checkIsHindered(HIT_LOCATION location)
{
    int randNum = std::rand() % 100 + 1;
    switch (location){
        case HIT_LOCATION::HEAD:
            return 1;
        case HIT_LOCATION::ABDOMEN:
            return randNum <= 50;
        case HIT_LOCATION::LIMBS:
            return randNum <= 50;
        default:
            return 0;
    }
}

// Check if a claymore is facing the direction at a given coordinate.
bool checkClaymoreIsFacingPos(const Explosive claymore, const std::pair<int, int> coord)
{
    bool isFacing = false;
    DIRECTION claymoreDirection = claymore.facingDirection.value();
    std::pair<int, int> claymoreCoordinates = claymore.coordinates;

    switch (claymoreDirection)
    {
        case EAST:
            isFacing = coord.first > claymoreCoordinates.first;
            break;
        case WEST:
            isFacing = coord.first < claymoreCoordinates.first;
            break;
        case NORTH:
            isFacing = coord.second < claymoreCoordinates.second;
            break;
        case SOUTH:
            isFacing = coord.second > claymoreCoordinates.second;
            break;
    }
    return isFacing;
}

void removeDeadInfected(World& world)
{
    time_t currentEpoch = std::time(0);
    for (auto it = world.infected.begin(); it != world.infected.end();)
    {
        // Remove dead infected after 3 seconds.
        if (!it->alive && it->timeOfDeath.has_value() &&
                it->timeOfDeath.value() + 3 <= currentEpoch)
            world.infected.erase(it);
        else
            ++it;
    }
}
