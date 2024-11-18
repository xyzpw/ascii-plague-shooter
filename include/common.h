#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <ctime>
#include <unordered_map>
#include <optional>

struct World;

enum DIRECTION{
    NORTH,
    SOUTH,
    EAST,
    WEST,
};
enum class HIT_LOCATION{
    HEAD,
    THORAX,
    ABDOMEN,
    LIMBS,
    __COUNT,
};

enum class FIREARM_TYPE{
    GLOCK_17,
    AR15,
    BOLT_ACTION_RIFLE,
    RUGER_MK_IV,
    __COUNT,
};

enum class CARTRIDGE_TYPE{
    CARTRIDGE_9MM,
    CARTRIDGE_223_REMINGTON,
    CARTRIDGE_30_06,
    CARTRIDGE_22LR,
    __COUNT,
};

enum class EXPLOSIVE_TYPE{
    M67_GRENADE,
    M18A1_CLAYMORE,
    __COUNT,
};

enum class PICKUP_TYPE{
    FIREARM,
    EXPLOSIVE,
};

struct SplatterEffect{
    std::vector<std::pair<int,int>> coordinates;
    const char* splatterChar = "*";
};

struct Rescue{
    const char* rescueChar = "\u2708";
    std::pair<int, int> coordinates;
    bool hasArrived = false;
    bool canBoard = false;
    bool isRescueFinished = false;
    time_t timeAtArrival;
    time_t timeAtEscape;
    void triggerRescueArrival(World& world);
};

struct Magazine{
    CARTRIDGE_TYPE cartridgeType;
    int cartridgeCount;
    int capacity;
    bool isHollowPoint = false;
    int kineticEnergy;
    double kineticEnergyLossPerMeter;
    Magazine(CARTRIDGE_TYPE cartridge = CARTRIDGE_TYPE::__COUNT,
            int maxRounds = 0, int rounds = 0);
};

struct Explosive{
    EXPLOSIVE_TYPE explosiveType;
    int explosionPascals;
    double explosionDelay = 0;
    std::string explodeAudioFile = "";
    std::string explodeCloseAudioFile = "";
    std::pair<int, int> coordinates{};
    const char* explosiveChar = "*";
    int fragmentCount;
    int fragmentKineticEnergy;
    double fragmentKineticEnergyLossPerMeter;
    std::optional<int> _explosiveId;
    std::optional<DIRECTION> facingDirection;
    std::optional<std::vector<std::pair<int,int>>> throwCoordinates;
    Explosive(EXPLOSIVE_TYPE type);
};

struct Firearm{
    FIREARM_TYPE firearmType;
    std::string name;
    int magazineCapacity;
    bool isChambered = true;
    bool canShoot = true;

    double reloadTime;
    double fastReloadTime;
    double chamberReloadDelay;
    int shootIntervalMs;
    double isReloading = false;
    int bulletKineticEnergy;
    double bulletKineticEnergyLossPerMeter; // Kinetic energy of bullet lost per meter
    std::string shootAudioFile = "";
    double accuracyDecay;
    double accuracyScaleFactor;
    CARTRIDGE_TYPE cartridgeType;
    Magazine magazine;
    int loadedRounds;
    int _defaultMagEquipCount = 0; // Number of mags equipped from supply drops
    Firearm(FIREARM_TYPE type);
};

struct Inventory{
    std::vector<Firearm> firearms{};
    std::vector<Explosive> explosives{};
    std::vector<Magazine> magazines{};
};

struct SupplyDrop{
    Inventory items;
    const char* message = "";
    std::pair<int, int> coordinates;
    const char* itemChar = "$";
    SupplyDrop(std::pair<int, int> colRange, std::pair<int, int> rowRange);
};

struct Player{
    const char* playerChar = "O";
    const char* weaponChar = "|";
    std::string hudText = "";
    bool alive = true;
    bool canSwitchFirearm = true;
    Firearm activeWeapon;
    Inventory inventory;
    std::pair<int, int> coordinates{};
    std::pair<int, int> weaponCoordinates{};
    DIRECTION facingDirection = NORTH;
    int killCount = 0;
    bool isReloading = false;
    std::string gameOverMessage = "";
    bool hasPlantedClaymore = false;
    bool isRescued = false;
    void fixWeaponAppearance();
    void shootFirearm();
    std::optional<Explosive> throwGrenade(World& world);
    void updateHudText();
    void pickupItem(World& world);
    void switchFirearm();
    void reloadFirearm();
    void fastReloadFirearm();
    void plantClaymore(World& world);
    Player() : activeWeapon(FIREARM_TYPE::RUGER_MK_IV){};
};

struct Infected{
    const char* infectedChar = "Z";
    bool alive = true;
    std::pair<int, int> coordinates;
    bool isHindered = false;
    std::optional<time_t> delayedDeathHitTime;
    std::optional<int> delayedDeathDuration;
    std::optional<time_t> timeOfDeath; // Epoch at which the infected has died.
    std::optional<SplatterEffect> deathSplatter;
};

struct World{
    time_t startTime;
    bool active;
    double metersPerChar = 1;
    std::pair<int, int> mapColumnLimits{};
    std::pair<int, int> mapRowLimits{};
    std::vector<Infected> infected;
    std::vector<SupplyDrop> supplyDrops{};
    std::vector<Explosive> activeExplosives{};
    int latestInfectedPositionUpdate;
    int infectedPositionUpdateMs = 200; // How often infected positions are updated in ms.
    time_t nextSupplyDropEpoch;
    Rescue rescue;
    void dropSupplies();
    World();
};

void playGame(World world); // Implemented in "screen_manager.cpp"

// Implemented in "probability_handler.cpp"
double computeDeathProbability(HIT_LOCATION location, int joules);

// Implemented in "player_manager.cpp"
std::unordered_map<EXPLOSIVE_TYPE, int> checkExplosiveInventorySize(Inventory inventory);

// Implemented in "probability_handler.cpp"
HIT_LOCATION getHitLocation(bool isPointBlank);

std::vector<std::pair<int, int>> getSplatterCoordinates(
    const std::pair<int,int> startCoord, const HIT_LOCATION hitLocation,
    const Magazine magUsed, const DIRECTION direction
);
