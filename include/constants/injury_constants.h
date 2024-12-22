#pragma once

constexpr int MINIMUM_LETHAL_ENERGY = 79;

constexpr double HINDER_PROBABILITY_ABDOMEN = 0.5;
constexpr double HINDER_PROBABILITY_LIMBS = 0.5;
constexpr double HINDER_PROBABILITY_HEAD = 1.0;

// Rate at which an enemy gets closer to death due to delayed death.
constexpr int DELAYED_DEATH_LOSS_RATE_HEAD_MIN = 13;
constexpr int DELAYED_DEATH_LOSS_RATE_HEAD_MAX = 44;
constexpr int DELAYED_DEATH_LOSS_RATE_THORAX_MIN = 75;
constexpr int DELAYED_DEATH_LOSS_RATE_THORAX_MAX = 250;
constexpr int DELAYED_DEATH_LOSS_RATE_ABDOMEN_MIN = 5;
constexpr int DELAYED_DEATH_LOSS_RATE_ABDOMEN_MAX = 132;
constexpr int DELAYED_DEATH_LOSS_RATE_LIMBS_MIN = 2;
constexpr int DELAYED_DEATH_LOSS_RATE_LIMBS_MAX = 6;

// Probability of gunshot wound resulting in a delayed death.
constexpr double DELAYED_DEATH_PROBABILITY_HEAD = 0.9;
constexpr double DELAYED_DEATH_PROBABILITY_THORAX = 0.5;
constexpr double DELAYED_DEATH_PROBABILITY_THORAX_HP = 0.88;
constexpr double DELAYED_DEATH_PROBABILITY_ABDOMEN = 0.2;
constexpr double DELAYED_DEATH_PROBABILITY_ABDOMEN_HP = 0.6;
constexpr double DELAYED_DEATH_PROBABILITY_LIMBS = 0.1;
constexpr double DELAYED_DEATH_PROBABILITY_LIMBS_HP = 0.3;

constexpr double DELAYED_DEATH_PROBABILITY_FRAGMENT = 0.2;

// Probability of bullet having an exit wound.
constexpr double BULLET_EXIT_PROBABILITY_22LR = 0.1;
constexpr double BULLET_EXIT_PROBABILITY_9MM = 0.6;

constexpr int DELAYED_DEATH_COUNTER_MAX = 5000;
constexpr int DELAYED_DEATH_COUNTER_HINDER = 4250;
constexpr int DELAYED_DEATH_COUNTER_FATAL = 3000;

constexpr int HEADSHOT_SPATTER_REQUIRED_FORCE = 2000;
