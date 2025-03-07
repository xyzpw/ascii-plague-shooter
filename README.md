# ascii-plague-shooter
![downloads](https://img.shields.io/github/downloads/xyzpw/ascii-plague-shooter/total)

**ascii-plague-shooter** is a simple top-down shooter game made with ncurses.<br>

<video src="https://github.com/user-attachments/assets/b900ae9e-74e1-45a2-b686-19b36e1c66c0"></video>

## Gameplay
The objective is to survive against rage-infected zombie-like enemies
 until rescue arrives.<br>

### Features
- Animations: Animations such as explosions.
- Real formulas: Real physics formulas and randomized values for realism.
- Multiple firearms: Variety of &#8212; real &#8212; different firearms.
- Realistic reloading: Same magazines are used after reloading. Firearms
 are chambered, shooting the entire mag increases the time to reload.
- Bullets: Different bullet types with higher lethality.
- Accuracy: The further an enemy, the less probability of a successful hit.
- Hit locations: Bullets can hit different locations which affect the
 probability of it being fatal.
- Splatter effect: Splatter effects after shooting an enemy may occur &#8212;
 although rare.
- Supply drops: Supply drops will be dropped to help you survive.
- Explosives: Explosives can be used which are mostly lethal by fragments.
- Realistic death and injury mechanics: Enemies don't have health &#8212;
 they die on multiple factors.

#### Firearms
Different firearms use different cartridge types, each have a different
 probability of being fatal.<br>
Enemies shot may also have a delayed death &#8212; it may take a several
 seconds or even longer depending on the hit location and severity.<br>
Enemies may even be hindered when shot or hit by explosive fragments.

#### Fast Reloading
If there are multiple enemies and you can't reload, you can fast reload &#8212;
 you will lose the entire mag at the cost of reloading faster.

If the firearm does not use detachable magazines, fast reloading loads a single
 round, whereas regular reloading continues until the magazine is full or there
 are no more rounds in the inventory.

##### Accuracy
The further an enemy is being shot at is, the less chance of the shot being
 a successful hit.

##### Bullet Types
Some bullets can be a hollow point, which will increase the probability of
 a shot being fatal at the cost of penetrating less enemies.

#### Explosives
Explosives can be used to take out multiple enemies if you are low on ammo
 or reloading.
> [!TIP]
> Your own explosives can kill you, stay far away!

##### M67 Grenade
An M67 grenade can be thrown with the `g` key. The distance of the thrown
 grenade is random, this is due to the throw angle and throw velocity being
 randomized.<br>
The M67 grenade will have about a thousand fragments going in all
 directions.<br>
The time to detonate is randomized from 4 to 5 seconds.

##### Claymores
Claymores can be placed with the `c` key, once a claymore is placed, it can
 be detonated with the `c` key again which will have a 1 second delay.<br>
The claymore will shoot hundreds of fragments in the direction it is facing.

##### M16 Mines
When an infected steps on an M16 mine, it will explode and send fragments in
all directions, killing nearby infected.

#### Rescue
When rescue arrives, you have a limited amount of time to get to the aircraft
 before they leave without you &#8212; you will lose the game.

### Controls
- Movement: `wasd`
- Look: `WASD`
- Shoot: `space`
- Reload: `r`
- Fast reload: `R`
- Equip item: `e`
- Throw grenade: `g`
- Throw grenade close: `G`
- Plant claymore: `c`
- Plant mine: `m`
- Swap firearm: `q`
- Quit game: `backspace`

## Installation
To install, `cd` into the root of the repo and use the `make` command to
 compile.<br>
Example on compiling and running the game:
```bash
$ make
$ ./plague-shooter
```

To play without colors:
```bash
$ ./plague-shooter --no-color
```

To reduce rescue arrival time:
```bash
$ ./plague-shooter --quick
```

To play without animations:
```bash
$ ./plague-shooter --no-animation
```
