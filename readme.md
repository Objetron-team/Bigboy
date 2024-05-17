# The Big boy
[Eurobot](https://www.coupederobotique.fr/wp-content/uploads/Eurobot2024_Rules_CUP_EN_FINAL.pdf)

## Mission
- Repot the plants and place them in cultivation
- Orient the solar panels
- Pollinate the plants
- Return to charge batteries
- Anticipate future crop yields

## Rules highlight
![](/map.png)

|||
|-|-|
| 1) Departure/arrival and drop-off areas           | 8) Solar panels               |
| 2) Departure/arrival and reserved drop-off areas  | 9) Reserved solar panels      |
| 3) Departure areas SIMA (ladybug hive)            | 10) ArUco marker number 20    |
| 4) Pot supply                                     | 11) ArUco marker number       |
| 5) Plant supply                                   | 12) ArUco marker number 22    |
| 6) Reserved planters                              | 13) ArUco marker number 23    |
| 7) Planters                                       | 14) Fixed beacon supports     |
|                                                   | 15) Remote computing area     |

## Strategy

### Positionning

#### Bigboy
- Bigboy will be positioned in the departure area in a defined position.
- Bigboy will use its encoders to know its position.
- The mouvement will be determine by the system of tasks.

#### Ladybirds
- Ladybirds will wait for the Bigboy to send a signal to start moving.

### Plants

#### Storage
- Plants will be stored in the storage area of the robot.

#### Open/Close the claw
- Bigboy will open and close its claw to take/let plants in its storage area.
- The claws will be controlled by servo motor.

### Solar
- Not done in this project