#include "common.h"
#include "game_utilities.h"

constexpr int BULLET_KE_9MM = 494;
constexpr int BULLET_KE_223_REMINGTON = 1738;
constexpr int BULLET_KE_30_06 = 3894;
constexpr int BULLET_KE_22LR = 178;
constexpr double BULLET_KE_LOSS_9MM = 1.4;
constexpr double BULLET_KE_LOSS_223_REMINGTON = 4.4;
constexpr double BULLET_KE_LOSS_30_06 = 6.32;
constexpr double BULLET_KE_LOSS_22LR = 0.63;

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
            accuracyDecay = 0.07;
            accuracyScaleFactor = 2;
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
            accuracyDecay = 0.016;
            accuracyScaleFactor = 1.27;
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
            accuracyDecay = 0.005;
            accuracyScaleFactor = 1.32;
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
            accuracyDecay = 0.07;
            accuracyScaleFactor = 2;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_22LR;
            loadedRounds = 10;
            magazine = Magazine(cartridgeType, 10, 9);
            break;
        }
    }
}

Magazine::Magazine(CARTRIDGE_TYPE cartridge, int maxRounds, int rounds)
{
    this->cartridgeType = cartridge;
    this->capacity = maxRounds;
    this->cartridgeCount = rounds;

    switch (cartridge){
        case CARTRIDGE_TYPE::CARTRIDGE_9MM:
            this->kineticEnergy = BULLET_KE_9MM;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_9MM;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON:
            this->kineticEnergy = BULLET_KE_223_REMINGTON;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_223_REMINGTON;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_30_06:
            this->kineticEnergy = BULLET_KE_30_06;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_30_06;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_22LR:
            this->kineticEnergy = BULLET_KE_22LR;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_22LR;
            break;
    }
}
