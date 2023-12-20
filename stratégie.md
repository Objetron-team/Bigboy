# Stratégie EUROBOT
Dans ce document, nous allons décrire la stratégie de notre robot afin de ramener le plus de points possibles. La stratégie de notre robt peur se décomposer selon les étapes qui suivent.

## Plantes
La première étape va consister à la récolte des plantes. Notre robot démarre de la zone 1 (zone de départ) pour récupérer les stocks de plante via sa pince collectrice et les amnener en zone de dépot (1 et 2). Seules les plantes résistantes seront valides dans ces zones et une plante dans la bonne zone donne 3 points.
=> 3x3x2= 18pts

## Panneaux solaires
La seconde étape consistera en l'orientation des panneaux solaires. Il  a six panneaux solaires à orienter et chaque panneau peut rapporter 5 points. Pour orienter les panneaux, nous utiliserons notre bras transversal.
=> 6x5= 30pts

## PAMIS
La troisième étape est attribuée au PAMI. A la 90ème seconde, le PAMI entrera en contact avec une plante dans la zone de dépot en moins de 10 secondes. Un PAMI qui est dans une zone de dépot ramène 5 points et si il est en contact avec une plante il rapporte 5 points de plus
=> 5+5= 10pts

## Retour à la base
A la fin de la manche la manche le robot retourne à sa zone de charge. Cette action donne 10 points
=> 10 pts

## Affichage dynamique
Une dernière manière de marquer des points et de pouvoir prédire le score que le robot va faire durant le match. Nous avons décidé de le faire de manière dynamique via un écran oled. A chaque tâche effectuée, le score s'incrementera tout seul. Les points attribués pour cela sont donnés selon la formule suivante min(20-ecarts/2;point réalisés)
=> 20 pts

## Conclusion
Si tout se passe comme prévu, nous devrions idéalement marqué 88 points.

# Encore à faire
schéma plateau + sketch 3D
Combien de pamis finalement? Car il faut qu'ils tiennent dans la zone de départ