#include "entities/firearm.h"
#include "entities/magazine.h"
#include "weapon_enums.h"
#include "constants/firearm_constants.h"

Firearm::Firearm(FIREARM_TYPE type)
{
    firearmType = type;
    switch (type)
    {
        case FIREARM_TYPE::SIG_M17: {
            reloadTime = 2;
            fastReloadTime = 0.5;
            chamberReloadDelay = 0.1;
            name = "SIG Sauer M17";
            shootAudioFile = "sig_m17.wav";
            shootIntervalMs = 300;
            accuracyDecay = FIREARM_ACCURACY_DECAYS.at(type);
            accuracyScaleFactor = FIREARM_ACCURACY_MULTIPLIERS.at(type);
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_9MM;
            loadedRounds = 17;
            magazine = Magazine(cartridgeType, 17, 16);
            feedSystem = RELOAD_TYPE::DETACHABLE_MAGAZINE;
            break;
        }
        case FIREARM_TYPE::AR15: {
            reloadTime = 3;
            fastReloadTime = 2;
            chamberReloadDelay = 0.2;
            name = "AR15";
            shootAudioFile = "223_remington.wav";
            shootIntervalMs = 250;
            accuracyDecay = FIREARM_ACCURACY_DECAYS.at(type);
            accuracyScaleFactor = FIREARM_ACCURACY_MULTIPLIERS.at(type);
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON;
            loadedRounds = 20;
            magazine = Magazine(cartridgeType, 20, 19);
            feedSystem = RELOAD_TYPE::DETACHABLE_MAGAZINE;
            break;
        }
        case FIREARM_TYPE::REMINGTON_700:{
            reloadTime = 3;
            fastReloadTime = 2;
            loadRoundTime = 0.5;
            chamberReloadDelay = 0.2;
            name = "Remington 700";
            shootAudioFile = "30_06.wav";
            shootIntervalMs = 1000;
            accuracyDecay = FIREARM_ACCURACY_DECAYS.at(type);
            accuracyScaleFactor = FIREARM_ACCURACY_MULTIPLIERS.at(type);
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_30_06;
            loadedRounds = 4;
            magazine = Magazine(cartridgeType, 4, 4);
            feedSystem = RELOAD_TYPE::DIRECT_LOAD;
            break;
        }
        case FIREARM_TYPE::RUGER_MK_IV:{
            reloadTime = 2;
            fastReloadTime = 0.5;
            chamberReloadDelay = 0.1;
            name = "Ruger Mk. IV";
            shootAudioFile = "22lr.wav";
            shootIntervalMs = 250;
            accuracyDecay = FIREARM_ACCURACY_DECAYS.at(type);
            accuracyScaleFactor = FIREARM_ACCURACY_MULTIPLIERS.at(type);
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_22LR;
            loadedRounds = 10;
            magazine = Magazine(cartridgeType, 10, 9);
            feedSystem = RELOAD_TYPE::DETACHABLE_MAGAZINE;
            break;
        }
    }
}