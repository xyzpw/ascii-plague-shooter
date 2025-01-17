#include <vector>
#include <thread>
#include <optional>
#include <algorithm>
#include <utility>
#include <unistd.h>
#include "common.h"
#include "logic/infected_handler.h"
#include "logic/claymore_logic.h"
#include "physics/bullet_physics.h"
#include "logic/splatter_effect_logic.h"
#include "audio_handler.h"

void handleFirearmShot(World& world, Player& player)
{
    if (player.activeWeapon.chamberRoundType == AMMO_TYPE::PELLET_SPREAD){
        handleShotgunPelletShot(world, player);
        return;
    }

    Position bulletPos = player.weaponPosition;
    int activeBulletKe = player.activeWeapon.magazine.kineticEnergy;
    int bulletKeLoss = player.activeWeapon.magazine.kineticEnergyLossPerMeter;
    bool isBulletHp = player.activeWeapon.chamberRoundType
                    == AMMO_TYPE::HOLLOW_POINT;
    bool isHighVelocity = player.activeWeapon.magazine.isHighVelocity;
    const int PENETRATE_THRESHOLD =
                player.activeWeapon.magazine.penetrateEnergyThreshold;
    const double PENETRATE_FACTOR = isBulletHp ? BULLET_PENETRATE_KE_FACTOR_HP :
            BULLET_PENETRATE_KE_FACTOR;

    const double EXIT_PROBABILITY = getBulletExitProbability(
            player.activeWeapon.magazine.cartridgeType
    );

    std::vector<Position> bulletPositions =
                        getBulletProjectilePositions(
                            world, bulletPos, player.facingDirection);

    bool isFirstImpact = true;
    int perforateCount = 0;
    HIT_LOCATION hitLocation;

    for (auto pos : bulletPositions)
    {
        if (activeBulletKe < PENETRATE_THRESHOLD ||
            perforateCount >= MAX_PERFORATE_COUNT)
        {
            break;
        }

        for (auto &inf : world.infected)
        {
            if (!inf.alive || inf.position != pos){
                continue;
            }

            double playerInfDistance = getPositionDistance(
                player.position, inf.position
            );
            double muzzleInfDistance = getPositionDistance(
                bulletPositions.at(0), inf.position
            );

            // Get the location of the bullet impact.
            if (isFirstImpact){
                std::optional<HIT_LOCATION> locationOpt = getBulletHitLocation(
                    playerInfDistance,
                    player.activeWeapon
                );
                if (!locationOpt.has_value())
                    return;
                hitLocation = locationOpt.value();
                isFirstImpact = false;
            }

            // Amount of kinetic energy from bullet impact.
            int impactKe = activeBulletKe;
            activeBulletKe *= PENETRATE_FACTOR;
            // Set the projectile kinetic energy to 0 if does not perforate.
            if (!checkProbability(EXIT_PROBABILITY)){
                activeBulletKe = 0;
            }
            impactKe -= activeBulletKe;

            if (activeBulletKe > 0){
                ++perforateCount;
            }

            bool isFatal = checkBulletWasFatal(hitLocation, impactKe);
            if (!isFatal && (isBulletHp || isHighVelocity) &&
                hitLocation == HIT_LOCATION::HEAD)
            {
                isFatal = true;
            }

            if (isFatal){
                inf.markAsDead();
                player.gameStats.addKill();
                if (hitLocation == HIT_LOCATION::HEAD)
                    player.gameStats.addHeadshot();
            }
            else if (checkShouldDelayedDeath(hitLocation, isBulletHp) ||
                        isHighVelocity)
            {
                int rate = calculateDelayedDeathLossRate(
                    hitLocation, isHighVelocity
                );
                inf._updateDelayedDeath(rate);
            }

            // Check if the bullet wound was hindering.
            if (checkShouldHinder(hitLocation)){
                inf.makeHindered();
            }

            // Check if the bullet wound causes a splatter effect.
            if (checkShouldSplatter(hitLocation, isHighVelocity, impactKe,
                muzzleInfDistance))
            {
                SplatterEffect s;
                s.positions = getSplatterPositions(
                    inf.position, hitLocation, player.facingDirection,
                    impactKe, muzzleInfDistance
                );
                inf._updateSplatterEffect(s);
            }
        }
        activeBulletKe -= bulletKeLoss;
    }
}

// Handle what happens to infected based on shotgun pellet trajectories.
void handleShotgunPelletShot(World& world, Player& player)
{
    Magazine &activeMag = player.activeWeapon.magazine;
    DIRECTION facingDirection = player.facingDirection;
    bool isFacingDirectionVertical =
         facingDirection == NORTH || facingDirection == SOUTH;

    const int PENETRATE_THRESHOLD = activeMag.penetrateEnergyThreshold;
    const double PELLET_KE = activeMag.kineticEnergy;
    const double PELLET_KE_LOSS = activeMag.kineticEnergyLossPerMeter;

    int activePelletCount = activeMag.pelletsPerShell.value();

    auto pelletTrajectories = getShotgunPelletTrajectories(
        world, player.weaponPosition,
        facingDirection, player.activeWeapon
    );
    if (pelletTrajectories.size() == 0){
        return;
    }
    const Position MUZZLE_POS = getBulletProjectilePositions(
        world, player.weaponPosition, facingDirection
    ).at(0);

    // Inactive columns/rows, used so infected behind another will not be hit.
    std::vector<int> inactivePelletCols;
    std::vector<int> inactivePelletRows;

    auto checkInactivePos = [&](Position pos)
    {
        if (isFacingDirectionVertical){
            auto temp = std::count(
                inactivePelletCols.begin(), inactivePelletCols.end(), pos.column
            );
            if (temp > 0)
                return true;
        } else {
            auto temp = std::count(
                inactivePelletRows.begin(), inactivePelletRows.end(), pos.row
            );
            if (temp > 0)
                return true;
        }
        return false;
    };

    bool hasImpacted = false;
    HIT_LOCATION hitLocation;
    for (auto trajectory : pelletTrajectories)
    {
        if (activePelletCount < 1){
            break;
        }

        for (auto posInfo : trajectory){
            for (auto &inf : world.infected)
            {
                if (inf.position != posInfo.first || !inf.alive){
                    continue;
                }
                else if (activePelletCount < 1){
                    break;
                }
                else if (checkInactivePos(inf.position)){
                    break;
                }
                if (isFacingDirectionVertical &&
                    player.position.column == inf.position.column)
                {
                    inactivePelletCols.push_back(inf.position.column);
                }
                else if (!isFacingDirectionVertical &&
                         player.position.row == inf.position.row)
                {
                    inactivePelletRows.push_back(inf.position.row);
                }

                double muzzleInfDistance = getPositionDistance(
                    MUZZLE_POS, inf.position
                );

                int impactKe = PELLET_KE - PELLET_KE_LOSS * muzzleInfDistance;
                if (impactKe < PENETRATE_THRESHOLD){
                    break;
                }

                if (!hasImpacted){
                    std::optional<HIT_LOCATION> loc = getBulletHitLocation(
                        muzzleInfDistance, player.activeWeapon
                    );
                    hitLocation = loc.has_value() ? loc.value() :
                                  HIT_LOCATION::ABDOMEN;
                    if (hitLocation == HIT_LOCATION::LIMBS){
                        hitLocation = HIT_LOCATION::ABDOMEN;
                    }

                    hasImpacted = true;
                }

                // Check if pellet impact was fatal.
                bool isFatal = false;
                for (int i = 0; i < posInfo.second && !isFatal; ++i){
                    isFatal = checkBulletWasFatal(hitLocation, impactKe);
                }
                if (isFatal){
                    inf.markAsDead();
                    player.gameStats.addKill();
                    if (hitLocation == HIT_LOCATION::HEAD){
                        player.gameStats.addHeadshot();
                    }
                }
                else if (checkShouldDelayedDeath(hitLocation, false)){
                    int rate = 0;
                    for (int i = 0; i < posInfo.second; ++i){
                        rate += calculateDelayedDeathLossRate(hitLocation, false);
                    }
                    inf._updateDelayedDeath(rate);
                }

                if (inf.alive){
                    // Check if any of the hitting pellets should hinder.
                    for (int i = 0; i < posInfo.second && !inf.isHindered; ++i){
                        if (checkShouldHinder(hitLocation)){
                            inf.makeHindered();
                        }
                    }
                }

                // Only add splatter if headshot and within 1 meter from muzzle.
                if (hitLocation == HIT_LOCATION::HEAD && muzzleInfDistance <= 1)
                {
                    SplatterEffect s;
                    s.positions = getSplatterPositions(
                        inf.position, hitLocation, facingDirection,
                        impactKe, muzzleInfDistance
                    );
                    inf._updateSplatterEffect(s);
                }

                activePelletCount -= posInfo.second;
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
    const int PENETRATE_THRESHOLD =
                getGrenade().fragmentPenetrateEnergyThreshold;
    int energyLossPerMeter = getGrenade().fragmentKineticEnergyLossPerMeter;
    int activeFragments = fragmentCount; // number of fragment projectiles.

    usleep(getGrenade().explosionDelay * 1e6);

    double playerDistance = getPositionDistance(
        player.position, getGrenade().position
    );
    bool playerInLethalRange = computeAreaFromDistance(playerDistance) <=
                               fragmentCount;

    // Check if grenade was fatal to player if they are within fragmentation
    // range.
    if (playerInLethalRange)
    {
        int playerFragmentHits = determineFragmentHitCount(
            activeFragments, playerDistance
        );
        activeFragments -= playerFragmentHits;
        bool fatal = checkExplosionWasFatal(
            getGrenade(), playerFragmentHits, playerDistance
        );


        if (fatal){
            player.gameStats.setEndGameMessage(GAME_END_MSG_GRENADE);
            player.alive = false;
            return;
        }
    }

    std::thread(playAudio,
                checkExplosionRupturedEar(getGrenade(), playerDistance) ?
                    getGrenade().explodeCloseAudioFile :
                    getGrenade().explodeAudioFile).detach();

    // Check if explosion was fatal for each infected.
    for (auto& inf : world.infected)
    {
        if (!inf.alive){
            continue;
        }

        int distance = getPositionDistance(
            inf.position, getGrenade().position
        );

        int fragmentHits = determineFragmentHitCount(activeFragments, distance);
        activeFragments -= fragmentHits;
        int hitKe = kineticEnergy - energyLossPerMeter * distance;

        if (fragmentHits == 0 || hitKe < PENETRATE_THRESHOLD){
            continue;
        }

        if (checkExplosionWasFatal(getGrenade(), fragmentHits, distance)){
            inf.markAsDead();
            player.gameStats.addKill();
            player.gameStats.addGrenadeKill();
            continue;
        }

        int lossRate = getFragmentDelayedDeathLossRate(fragmentHits);
        if (lossRate > 0){
            inf._updateDelayedDeath(lossRate);
        }

        if (checkExplosionWasHindering(fragmentHits)){
            inf.makeHindered();
        }
    }

    // Remove explosive from world's active explosives after explosion.
    auto exp = std::find_if(world.activeExplosives.begin(),
                            world.activeExplosives.end(), [&](Explosive ex)
        {
            return ex._explosiveId == explosiveId;
        });
    world.activeExplosives.erase(exp);
}

void handleClaymoreExplosion(World& world, Player& player, Explosive explosive)
{
    Position claymorePosition = explosive.position;
    DIRECTION claymoreDirection = explosive.facingDirection.value();

    int fragmentCount = explosive.fragmentCount;
    int kineticEnergy = explosive.fragmentKineticEnergy;
    const int PENETRATE_THRESHOLD = explosive.fragmentPenetrateEnergyThreshold;
    int kineticEnergyLossPerMeter = explosive.fragmentKineticEnergyLossPerMeter;
    int activeFragmentCount = fragmentCount;

    int explosionPascals = explosive.explosionPascals;

    // Check if explosion was lethal.
    auto checkFatal = [&](Position pos){
        int fragments = getClaymoreFragmentCountAtPos(explosive, pos,
                                                      activeFragmentCount);
        activeFragmentCount -= fragments;

        int distance = getPositionDistance(pos, claymorePosition);

        int energy = kineticEnergy - kineticEnergyLossPerMeter * distance;
        int pascals = computeInverseSquareLaw(
            explosive.explosionPascals, distance
        );

        if (checkProbability(calculateFragmentFatalProbability(
            energy, fragments)))
        {
            return true;
        } else if (checkProbability(calculateExplosionFatalProbability(
                    pascals)))
        {
            return true;
        }
        return false;
    };

    player.hasPlantedClaymore = false;
    usleep(explosive.explosionDelay * 1e6);

    bool lethalToPlayer = checkFatal(player.position);

    if (lethalToPlayer){
        player.alive = false;
        player.gameStats.setEndGameMessage(GAME_END_MSG_CLAYMORE);
        return;
    }

    double pascalsAtPlayer = computeInverseSquareLaw(
        explosionPascals, getPositionDistance(
            player.position, claymorePosition
        )
    );

    double isCloseProb = calculateEarRuptureProbability(pascalsAtPlayer);
    bool isClose = checkProbability(isCloseProb);

    std::thread(playAudio, isClose ? explosive.explodeCloseAudioFile :
                explosive.explodeAudioFile).detach();

    // Remove the claymore from world's active explosives after detonation.
    auto exp = std::find_if(world.activeExplosives.begin(),
                            world.activeExplosives.end(), [&](Explosive ex)
        {
            return ex._explosiveId == explosive._explosiveId;
        });
    world.activeExplosives.erase(exp);

    for (auto& inf : world.infected)
    {
        double fragmentDistance = getPositionDistance(
            claymorePosition, inf.position
        );

        int activeFragmentKe = kineticEnergy - fragmentDistance
                               * kineticEnergyLossPerMeter;

        if (activeFragmentKe < PENETRATE_THRESHOLD){
            continue;
        }

        int fragmentCountBeforeCheck = activeFragmentCount;
        if (checkFatal(inf.position))
        {
            inf.markAsDead();
            player.gameStats.addKill();
            player.gameStats.addClaymoreKill();
            continue;
        }
        int fragmentHitCount = fragmentCountBeforeCheck - activeFragmentCount;

        int lossRate = getFragmentDelayedDeathLossRate(fragmentHitCount);
        if (lossRate > 0){
            inf._updateDelayedDeath(lossRate);
        }

        // Check if fragments hindered infected.
        for (int i = 0; i < fragmentHitCount && !inf.isHindered; ++i){
            if (checkShouldHinder(randHitLocation())){
                inf.makeHindered();
            }
        }
    }
}

void handleDelayedDeathInfected(World& world, Player& player)
{
    for (auto &inf : world.infected){
        if (!inf.alive || !inf.delayedDeathStartEpoch.has_value()){
            continue;
        }

        double elapsed =
                getEpochAsDecimal() - inf.delayedDeathStartEpoch.value();

        if (elapsed == 0.0){
            continue;
        }

        inf.delayedDeathCounter = DELAYED_DEATH_COUNTER_MAX -
                                inf.delayedDeathLossRate * elapsed;

        if (inf.delayedDeathCounter <= DELAYED_DEATH_COUNTER_FATAL){
            inf.markAsDead();
            player.gameStats.addKill();
        }
        else if (inf.delayedDeathCounter <= DELAYED_DEATH_COUNTER_HINDER)
        {
            inf.makeHindered();
        }
    }
}
