#include <iostream>
#include <utility>
#include <unistd.h>
#include "common.h"

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
            bulletKineticEnergy = 1695;
            bulletKineticEnergyLossPerMeter = 4.4;
            reloadTime = 2.5;
            fastReloadTime = 1;
            chamberReloadDelay = 0.2;
            name = "AR15";
            shootAudioFile = "ar15_shoot.wav";
            shootIntervalMs = 250;
            accuracyDecay = 0.016;
            accuracyScaleFactor = 1.27;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON;
            loadedRounds = 20;
            magazine = Magazine(cartridgeType, 20, 19);
            break;
        }
        case FIREARM_TYPE::BOLT_ACTION_RIFLE:{
            bulletKineticEnergy = 3894;
            bulletKineticEnergyLossPerMeter = 6.32;
            reloadTime = 3;
            fastReloadTime = 1.5;
            chamberReloadDelay = 1;
            name = "Remington 700";
            shootAudioFile = "bolt_action_rifle_shoot.wav";
            shootIntervalMs = 1000;
            accuracyDecay = 0.005;
            accuracyScaleFactor = 1.32;
            cartridgeType = CARTRIDGE_TYPE::CARTRIDGE_30_06;
            loadedRounds = 4;
            magazine = Magazine(cartridgeType, 4, 3);
            break;
        }
        case FIREARM_TYPE::RUGER_MK_IV:{
            bulletKineticEnergy = 178;
            bulletKineticEnergyLossPerMeter = 0.63;
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

Explosive::Explosive(EXPLOSIVE_TYPE type)
{
    explosiveType = type;
    switch (type)
    {
        case EXPLOSIVE_TYPE::M67_GRENADE:
            explosionPascals = 962e3;
            explosionDelay = (std::rand() % (55 - 40 + 1) + 40) / 10.0;
            explodeAudioFile = "explosion_with_debris.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = "\u2022";
            fragmentCount = 1300;
            fragmentKineticEnergy = 105;
            fragmentKineticEnergyLossPerMeter = 0.47;
            break;
        case EXPLOSIVE_TYPE::M18A1_CLAYMORE:
            explosionPascals = 4556e3;
            explosionDelay = 1;
            explodeAudioFile = "explosion.wav";
            explodeCloseAudioFile = "explosion_close.wav";
            explosiveChar = "<";
            fragmentCount = 700;
            fragmentKineticEnergy = 467;
            fragmentKineticEnergyLossPerMeter = 2.2;
            break;
    }
}

Magazine::Magazine(CARTRIDGE_TYPE cartridge, int maxRounds, int rounds)
{
    this->cartridgeType = cartridge;
    this->capacity = maxRounds;
    this->cartridgeCount = rounds;

    switch (cartridge){
        case CARTRIDGE_TYPE::CARTRIDGE_9MM:
            this->kineticEnergy = 494;
            this->kineticEnergyLossPerMeter = 1.4;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON:
            this->kineticEnergy = 1738;
            this->kineticEnergyLossPerMeter = 4.4;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_30_06:
            this->kineticEnergy = 3894;
            this->kineticEnergyLossPerMeter = 6.32;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_22LR:
            this->kineticEnergy = 178;
            this->kineticEnergyLossPerMeter = 0.63;
            break;
    }
}
