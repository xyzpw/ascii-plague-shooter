#include "entities/firearm.h"
#include "entities/magazine.h"
#include "weapon_enums.h"
#include "constants/firearm_constants.h"

Firearm::Firearm(FIREARM_TYPE type)
{
    firearmType = type;
    switch (type)
    {
        case FIREARM_TYPE::GLOCK_17: {
            reloadTime = 1.75;
            fastReloadTime = 0.5;
            chamberReloadDelay = 0.1;
            name = "Glock 17";
            shootAudioFile = "9mm.wav";
            shootIntervalMs = 300;
            accuracyDecay = HANDGUN_ACCURACY_DECAY;
            accuracyScaleFactor = HANDGUN_ACCURACY_MULTIPLIER;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_9MM;
            loadedRounds = 10;
            magazine = Magazine(cartridgeType, 10, 9);
            break;
        }
        case FIREARM_TYPE::AR15: {
            reloadTime = 2.5;
            fastReloadTime = 1;
            chamberReloadDelay = 0.2;
            name = "AR15";
            shootAudioFile = "223_remington.wav";
            shootIntervalMs = 250;
            accuracyDecay = A_RIFLE_ACCURACY_DECAY;
            accuracyScaleFactor = A_RIFLE_ACCURACY_MULTIPLIER;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON;
            loadedRounds = 20;
            magazine = Magazine(cartridgeType, 20, 19);
            break;
        }
        case FIREARM_TYPE::BOLT_ACTION_RIFLE:{
            reloadTime = 3;
            fastReloadTime = 1.5;
            chamberReloadDelay = 1;
            name = "Remington 700";
            shootAudioFile = "30_06.wav";
            shootIntervalMs = 1000;
            accuracyDecay = A_RIFLE_ACCURACY_DECAY;
            accuracyScaleFactor = A_RIFLE_ACCURACY_MULTIPLIER;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_30_06;
            loadedRounds = 4;
            magazine = Magazine(cartridgeType, 4, 3);
            break;
        }
        case FIREARM_TYPE::RUGER_MK_IV:{
            reloadTime = 1.75;
            fastReloadTime = 0.5;
            chamberReloadDelay = 0.1;
            name = "Ruger Mk. IV";
            shootAudioFile = "22lr.wav";
            shootIntervalMs = 250;
            accuracyDecay = HANDGUN_ACCURACY_DECAY;
            accuracyScaleFactor = HANDGUN_ACCURACY_MULTIPLIER;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_22LR;
            loadedRounds = 10;
            magazine = Magazine(cartridgeType, 10, 9);
            break;
        }
    }
}