# Stratégie EUROBOT
Dans ce document, nous allons décrire la stratégie de notre robot afin de ramener le plus de points possibles. La stratégie de notre robot peut se décomposer selon les étapes qui suivent.

## Plantes
La première étape va consister à la récolte des plantes. Bigoy démarre de la zone 1 pour l'équipe jaune ou zone 2 pour l'équipe bleue il ira ensuite rechercher les stocks de plantes afin de les emmener dans la zone de dépôt (1 et 2). Seules les plantes résistantes seront valides dans ces zones, et une plante dans la bonne zone donne 3 points.
=> 3*(2+2) = 12pts
### Plantes bonus
Le robot ira ensuite récuperer si possible les plantes du milieu de terrains et celles de l'adversaire si elles n'ont pas été touchées
=> 3*(2+2+2) = 18pts

## Panneaux solaires
Les panneaux solaires ne sont pas compris dans notre stratégie.

## PAMIS
L'étape suivante est attribuée aux PAMIs. À la 90ème seconde, les PAMIs entreront en contact avec une plante dans la zone de dépôt en moins de 10 secondes. Un PAMI qui est dans une zone de dépôt ramène 5 points, et s'il est en contact avec une plante, il rapporte 5 points de plus.
=> 5+5 = 10pts

## Retour à la base
À la fin de la manche, le robot retourne à sa zone de charge du coté adverse. Cette action donne 10 points.
=> 10 pts

## Affichage dynamique
Une dernière manière de marquer des points est de pouvoir prédire le score que le robot va faire durant le match. Nous avons décidé de le faire de manière dynamique via un écran OLED. À chaque tâche effectuée, le score s'incrémentera tout seul. Les points attribués pour cela sont donnés selon la formule suivante min(20-écarts/2; points réalisés).
=> 20 pts

## Conclusion
Bigboy et nos PAMIs utiliseront le même code avec le système de tâche. La détection de collision se fera à l'aide de capteurs ultrasons.
Si tout se passe comme prévu, nous devrions idéalement marquer 70 points.

![Map](/strat_1.png)