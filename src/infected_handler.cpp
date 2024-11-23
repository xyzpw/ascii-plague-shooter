#include <utility>
#include <cmath>
#include <unistd.h>
#include <thread>
#include "common.h"
#include "math_utils.h"
#include "game_utilities.h"
#include "infected_handler.h"
#include "injury_handler.h"

// Returns the column and row to where a new infected should spawn, respectively.
std::pair<int, int> getInfectedSpawnPosition(World world)
{
    int colRange = world.mapColumnLimits.second - world.mapColumnLimits.first;
    int col = randIntInRange(0, colRange) + world.mapColumnLimits.first;
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

        if (inf.isHindered && checkProbability(0.25))
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
    if (!player.activeWeapon.canShoot){
        return;
    }
    bool pointBlank = false;

    // Determine if blood splatter should be displayed.
    auto checkShouldSplatter = [&](HIT_LOCATION location, Magazine mag){
        if (mag.cartridgeType == CARTRIDGE_TYPE::CARTRIDGE_22LR){
            return false;
        }
        else if (location == HIT_LOCATION::HEAD && mag.isHollowPoint){
            return true;
        }
        return checkProbability(mag.isHollowPoint ? 0.04 : 0.02);
    };

    auto checkBulletHit = [&](int distance){
        Firearm firearm = player.activeWeapon;
        double decay = firearm.accuracyDecay;
        double pHit = firearm.accuracyScaleFactor * std::exp(-decay * distance);
        if (pHit >= 1.0){
            pointBlank = true;
            return true;
        }
        return checkProbability(pHit);
    };

    int playerCol = player.coordinates.first,
                    playerRow = player.coordinates.second;

    DIRECTION direction = player.facingDirection;
    std::vector<std::pair<int, int>> coordinatesInRange{};

    // Kinetic energy of the fired bullet; decreases with distance due to air
    // resistance (drag).
    int activeBulletKE = player.activeWeapon.magazine.kineticEnergy;

    std::pair<int, int> bulletCoordinates = player.coordinates;
    auto updateBulletCoordinates = [&]()
    {
        if (direction == NORTH || direction == SOUTH){
            bulletCoordinates.second += direction == SOUTH ? +1 : -1;
        }
        else{
            bulletCoordinates.first += direction == EAST ? +1 : -1;
        }
        return checkCoordinatesInsideMap(world, bulletCoordinates);
    };
    updateBulletCoordinates();

    bool isBulletHp = player.activeWeapon.magazine.isHollowPoint;
    // Check each coordinate in the bullet's path to see if it hits an infected.
    while (updateBulletCoordinates() && activeBulletKE >= MINIMUM_LETHAL_ENERGY)
    {
        activeBulletKE -=
                player.activeWeapon.magazine.kineticEnergyLossPerMeter;
        for (auto& inf : world.infected)
        {
            if (inf.alive && inf.coordinates == bulletCoordinates)
            {
                int playerToInfDistance = getMapPointsDistance(
                    player.coordinates, inf.coordinates
                );

                if (!checkBulletHit(playerToInfDistance))
                    return;

                int keDamage = activeBulletKE;
                activeBulletKE *= isBulletHp ? PENETRATE_KE_FACTOR_HP :
                                    PENETRATE_KE_FACTOR;
                keDamage -= activeBulletKE;

                HIT_LOCATION hitLocation = getHitLocation(pointBlank);

                bool wasFatal = isBulletHp &&
                        hitLocation == HIT_LOCATION::HEAD ||
                        checkBulletWasFatal(hitLocation, keDamage);

                if (wasFatal){
                    inf.markAsDead();
                    ++player.killCount;
                }
                else if (checkIsHindered(hitLocation)){
                    inf.isHindered = true;
                }

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
                if (checkShouldSplatter(hitLocation, player.activeWeapon.magazine))
                {
                    if (inf.deathSplatter.has_value()){
                        auto coords = getSplatterCoordinates(
                                inf.coordinates, hitLocation,
                                player.activeWeapon.magazine,
                                player.facingDirection);
                        for (auto coord : coords){
                            inf.deathSplatter->coordinates.push_back(coord);
                        }
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

    usleep(getGrenade().explosionDelay * 1e6);

    double playerDistance = getMapPointsDistance(
        player.coordinates, getGrenade().coordinates
    );

    if (checkExplosionWasFatal(getGrenade(), playerDistance)){
        player.gameOverMessage = "cause of death: grenade";
        player.alive = false;
        return;
    }

    std::thread(playAudio,
                checkExplosionRupturedEardrum(getGrenade(), playerDistance) ?
                    getGrenade().explodeCloseAudioFile :
                    getGrenade().explodeAudioFile).detach();

    for (auto& inf : world.infected)
    {
        int distance = getMapPointsDistance(
            inf.coordinates, getGrenade().coordinates
        );

        if (checkExplosionWasFatal(getGrenade(), distance)){
            inf.markAsDead();
            ++player.killCount;
            continue;
        }

        if (checkExplosionWasHindering(getGrenade(), distance)){
            inf.isHindered = true;
        }
    }

    // Remove explosive from world's active explosives after explosion.
    for (auto exp = world.activeExplosives.begin();
              exp != world.activeExplosives.end(); ++exp)
        if (exp->_explosiveId.has_value() &&
            exp->_explosiveId.value() == explosiveId)
        {
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

    // Check if explosion was lethal.
    auto checkFatal = [&](std::pair<int, int> coord){
        int fragments = getClaymoreFragmentCountAtPos(explosive, coord);
        int distance = getMapPointsDistance(coord, claymorePosition);
        int energy = kineticEnergy - kineticEnergyLossPerMeter * distance;
        int pascals = computeInverseSquareLaw(
            explosive.explosionPascals, distance
        );
        if (checkProbability(computeFragmentLethalProb(energy, fragments))){
            return true;
        } else if (checkProbability(computeExplosionLethalProb(pascals))){
            return true;
        }
        return false;
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
    bool isClose = checkProbability(isCloseProb);

    std::thread(playAudio, isClose ? explosive.explodeCloseAudioFile :
                explosive.explodeAudioFile).detach();

    // Remove the claymore from world's active explosives after detonation.
    for (auto ex = world.activeExplosives.begin();
         ex != world.activeExplosives.end();)
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
        int fragments = getClaymoreFragmentCountAtPos(explosive, inf.coordinates);
        for (int i = 0; i < fragments && !inf.isHindered; ++i){
            if (checkIsHindered(randHitLocation())){
                inf.isHindered = true;
            }
        }
    }
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
