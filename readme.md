# The Big boy
[Eurobot](www.eurobot.org)

## Mission
- Repot the plants and put themin cultivation
- Orient the solar panles
- Pollinate the plants
- Return to charge bateries
- Anticipate future crop yeld

## Rules highlight
![map](https://private-user-images.githubusercontent.com/23726572/280705573-2970816a-4791-41fb-b9a9-4060374664ed.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTEiLCJleHAiOjE2OTkyNzM5NTgsIm5iZiI6MTY5OTI3MzY1OCwicGF0aCI6Ii8yMzcyNjU3Mi8yODA3MDU1NzMtMjk3MDgxNmEtNDc5MS00MWZiLWI5YTktNDA2MDM3NDY2NGVkLnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFJV05KWUFYNENTVkVINTNBJTJGMjAyMzExMDYlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjMxMTA2VDEyMjczOFomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTg5YmUzYmZhNWY3OTEyNTQyMDhiZDU4YzA4OWE1MDRjMTI5MGUyMGEyNzRkYzAwNzg3YjA1ZDI1ZDBkMWNhMTYmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.Bajujn_Csbh1FggLT4hk8UkJ210yp5451AsTSYxXo24)

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
- Use a kinect tower and raspberry pie rest server
- BigBoy ask at a fixe frequency where he is and what path he need base on the requested zone
#### Ladybirds
- Ladybirds -> ask the time to the tower
- activate when needed
- ask the path to the tower
- repeat to update path

### Plants

#### Load internal storage
- Takes plants from 5
- Takes pots from 4

#### Unload storage
- Id the plants -> RFID -> decide where to put
- Load the plant into the pot
- type 1 -> commun | type 2 -> reserved

### Solar

## Case study
 
