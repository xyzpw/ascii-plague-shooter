#include "entities/explosive.h"
#include "constants/explosive_constants.h"
#include "random_utils.h"

Explosive::Explosive(EXPLOSIVE_TYPE type)
{
    explosiveType = type;
    switch (type)
    {
        case EXPLOSIVE_TYPE::M67_GRENADE:
            explosionPascals = M67_PASCALS;
            explosionDelay = randIntInRange(
                static_cast<int>(M67_EXPLOSION_DELAY_MIN * 10),
                static_cast<int>(M67_EXPLOSION_DELAY_MAX * 10)
            ) / 10.0;
            explodeAudioFile = "explosion_with_debris.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = GRENADE_CHAR;
            fragmentCount = M67_FRAGMENT_COUNT;
            fragmentKineticEnergy = M67_FRAGMENT_KE;
            fragmentKineticEnergyLossPerMeter = M67_FRAGMENT_KE_LOSS;
            fragmentPenetrateEnergyThreshold = M67_PENETRATE_ENERGY_THRESHOLD;
            break;
        case EXPLOSIVE_TYPE::M18A1_CLAYMORE:
            explosionPascals = CLAYMORE_PASCALS;
            explosionDelay = CLAYMORE_EXPLOSION_DELAY;
            explodeAudioFile = "explosion.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = CLAYMORE_CHAR_DEFAULT;
            fragmentCount = CLAYMORE_FRAGMENT_COUNT;
            fragmentKineticEnergy = CLAYMORE_FRAGMENT_KE;
            fragmentKineticEnergyLossPerMeter = CLAYMORE_FRAGMENT_KE_LOSS;
            fragmentPenetrateEnergyThreshold =
                    CLAYMORE_PENETRATE_ENERGY_THRESHOLD;
            break;
    }
}