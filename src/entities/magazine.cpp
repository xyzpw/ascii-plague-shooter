#include "entities/magazine.h"
#include "constants/firearm_constants.h"
#include "weapon_enums.h"

Magazine::Magazine(CARTRIDGE_TYPE cartridge, int maxRounds, int rounds)
{
    this->cartridgeType = cartridge;
    this->capacity = maxRounds;
    this->cartridgeCount = rounds;
    this->isHighVelocity = false;

    switch (cartridge){
        case CARTRIDGE_TYPE::CARTRIDGE_9MM:
            this->kineticEnergy = BULLET_KE_9MM;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_9MM;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_223_REMINGTON:
            this->kineticEnergy = BULLET_KE_223_REMINGTON;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_223_REMINGTON;
            this->isHighVelocity = true;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_30_06:
            this->kineticEnergy = BULLET_KE_30_06;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_30_06;
            this->isHighVelocity = true;
            break;
        case CARTRIDGE_TYPE::CARTRIDGE_22LR:
            this->kineticEnergy = BULLET_KE_22LR;
            this->kineticEnergyLossPerMeter = BULLET_KE_LOSS_22LR;
            break;
    }
}