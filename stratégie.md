# Stratégie EUROBOT
Dans ce document, nous allons décrire la stratégie de notre robot afin de ramener le plus de points possibles. La stratégie de notre robot peut se décomposer selon les étapes qui suivent.

## Plantes
La première étape va consister à la récolte des plantes. Notre robot démarre de la zone 1 (zone de départ) pour récupérer les stocks de plantes via sa pince collectrice et les amener en zone de dépôt (1 et 2). Seules les plantes résistantes seront valides dans ces zones, et une plante dans la bonne zone donne 3 points.
=> 3x3x2= 18pts

## Panneaux solaires
La seconde étape consistera en l'orientation des panneaux solaires. Il a six panneaux solaires à orienter et chaque panneau peut rapporter 5 points. Pour orienter les panneaux, nous utiliserons notre bras transversal.
=> 6x5= 30pts

## PAMIS
La troisième étape est attribuée au PAMI. À la 90ème seconde, le PAMI entrera en contact avec une plante dans la zone de dépôt en moins de 10 secondes. Un PAMI qui est dans une zone de dépôt ramène 5 points, et s'il est en contact avec une plante, il rapporte 5 points de plus.
=> 5+5= 10pts

## Retour à la base
À la fin de la manche, le robot retourne à sa zone de charge. Cette action donne 10 points.
=> 10 pts

## Affichage dynamique
Une dernière manière de marquer des points est de pouvoir prédire le score que le robot va faire durant le match. Nous avons décidé de le faire de manière dynamique via un écran OLED. À chaque tâche effectuée, le score s'incrémentera tout seul. Les points attribués pour cela sont donnés selon la formule suivante min(20-écarts/2; points réalisés).
=> 20 pts

## Conclusion
Dans un premier temps, nous allons hardcoder notre robot afin qu'il puisse faire toutes ces tâches de manière autonome sans aide extérieure, juste à l'aide de ses capteurs. Il pourra se déplacer, se situer et éviter les obstacles grâce à ses capteurs ultrasons. Une fois le robot autonome, nous ajouterons un système de caméra via une balise externe afin que le robot puisse avoir en permanence une idée générale du plateau afin de pouvoir optimiser ses mouvements.
Si tout se passe comme prévu, nous devrions idéalement marquer 88 points.

# TODO liste:
schéma plateau + sketch 3D
Combien de PAMIS finalement? Car il faut qu'ils tiennent dans la zone de départ.
