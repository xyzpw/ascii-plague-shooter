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
    Position bulletPos = player.weaponPosition;
    int activeBulletKe = player.activeWeapon.magazine.kineticEnergy;
    int bulletKeLoss = player.activeWeapon.magazine.kineticEnergyLossPerMeter;
    bool isBulletHp = player.activeWeapon.magazine.isHollowPoint;
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
                if (!inf.delayedDeathStartEpoch.has_value()){
                    inf.delayedDeathStartEpoch = getEpochAsDecimal();
                }
                inf.delayedDeathLossRate +=
                    calculateDelayedDeathLossRate(hitLocation, isHighVelocity,
                                                    impactKe);
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
                if (inf.splatter.has_value()){
                    for (auto p : s.positions){
                        inf.splatter->positions.push_back(p);
                    }
                }
                else{
                    inf.splatter = s;
                }
            }
        }
        activeBulletKe -= bulletKeLoss;
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

    double playerDistance = getPositionDistance(
        player.position, getGrenade().position
    );

    // Check if grenade was fatal to player if they are within fragmentation range.
    if (computeAreaFromDistance(playerDistance) <= fragmentCount &&
        checkExplosionWasFatal(getGrenade(), playerDistance))
    {
        player.gameStats.setEndGameMessage(GAME_END_MSG_GRENADE);
        player.alive = false;
        return;
    }

    std::thread(playAudio,
                checkExplosionRupturedEar(getGrenade(), playerDistance) ?
                    getGrenade().explodeCloseAudioFile :
                    getGrenade().explodeAudioFile).detach();

    for (auto& inf : world.infected)
    {
        int distance = getPositionDistance(
            inf.position, getGrenade().position
        );

        if (checkExplosionWasFatal(getGrenade(), distance)){
            inf.markAsDead();
            player.gameStats.addKill();
            player.gameStats.addGrenadeKill();
            continue;
        }

        if (checkExplosionWasHindering(getGrenade(), distance)){
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
    int kineticEnergy = explosive.fragmentKineticEnergy;
    int kineticEnergyLossPerMeter = explosive.fragmentKineticEnergyLossPerMeter;
    int explosionPascals = explosive.explosionPascals;

    // Check if explosion was lethal.
    auto checkFatal = [&](Position pos){
        int fragments = getClaymoreFragmentCountAtPos(explosive, pos);
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
        if (checkFatal(inf.position))
        {
            inf.markAsDead();
            player.gameStats.addKill();
            player.gameStats.addClaymoreKill();
            continue;
        }

        // Check if fragments hindered infected.
        int fragments = getClaymoreFragmentCountAtPos(explosive, inf.position);
        for (int i = 0; i < fragments && !inf.isHindered; ++i){
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
