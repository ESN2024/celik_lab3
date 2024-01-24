<h1> ESN12 - lab3 </h1>

<h2> Objectif </h2>

Dans le cadre de notre cours de conception conjointe en GE5A à Polytech Clermont, nous avons appliqué les connaissances théoriques acquises lors des sessions pratiques. Dans ce projet, réalisé sur une carte Terasic DE10-Lite avec un FPGA Altera MAX 10, notre objectif est d'interfacer un accéléromètre ADXL345 via I2C, le configurer correctement et afficher les différentes axes sur des afficheurs 7 segments à l'aide d'un timer et d'interruptions.

<h2> Création d'un système NIOS II basique </h2>

<h3> Schéma fonctionnel </h3>

Nous débuterons par la création d'une représentation schématique du système final avant d'entamer la phase de mise en œuvre. Il se concentre uniquement sur les périphériques définis dans nos objectifs. Nous pouvons donc retrouver l'architecture qui englobe plusieurs modules essentiels :

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/6819b16d-9553-4939-ba9a-308d0ffc7043)

<h3> Platform Designer </h3>

Suite à la création d'un nouveau projet sur Quartus, procéder à l'ouverture du Platform Designer afin d'intégrer tous les éléments requis.

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/e4b2ad48-6d70-4fcc-9ce9-28c0fe66b0f4)
![image](https://github.com/ESN2024/celik_lab3/assets/117944504/6509283b-a4dc-4d5d-8907-72cb658971c9)

Les composants à ajouter comprennent :

- Horloge
- RAM
- NIOS-2
- JTAG
  
De plus, j'ai ajouté des PIO (Ports d'Entrée/Sortie Programmables) pour la gestion des E/S :

- PIO 7 bits pour le segment 0
- PIO 7 bits pour le segment 1
- PIO 7 bits pour le segment 2
- PIO 7 bits pour le segment 3
- PIO 7 bits pour le segment 4
- PIO 7 bits pour le segment 5
- timer_0
- PIO 1 bit pour le bouton poussoir 

<h3> Quartus </h3>

Suite au Platform Designer, nous allons effectuer une vérification minutieuse pour s'assurer de l'absence d'erreurs puis procéder à la génération du code VHDL. Ensuite, nous allons établir les connexions des entrées/sorties en associant les broches correspondantes sur la carte en utilisant l'outil Pin Planner de Quartus pour une configuration précise.

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/304a1149-73aa-449d-8a3d-1cac2463d02d)

Après cette étape, compiler le projet Quartus afin de préparer le fichier de configuration puis programmer la carte à l'aide de l'USB-Blaster pour transférer le code vers le FPGA.

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/0e912d25-762a-4a3b-bf0a-e87b3de68f34)

<h3> Flux logiciel du NIOS II </h3>

Ouvrir un terminal NIOS II et générer le BSP en lien avec les spécifications de la carte DE-10 :

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/5bdbb290-1f14-4b60-a3c1-1132eec7edb3)

Générer le Makefile du projet :

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/ab5e0d84-1673-4bf0-a604-98168ac87301)

Tout est prêt, nous allons maintenant écrire notre code C qui fonctionne sur notre propre processeur. Ne pas oublier de mettre à jour le Makefile avec le code source. Le programme généré peut être chargé et exécuté dans le softcore du FPGA

<h3> Language C </h3>

![image](https://github.com/ESN2024/celik_lab3/assets/117944504/6460e352-2ac1-4f6a-98da-5a2750ce447e)

<h3> Résultats </h3>

- _Initialisation de l'I2C_ : Pour initialiser la communication avec l'I2C, j'ai utilisé les fichiers fournis par l'IP opencores_i2c. J'ai d'abord appelé la fonction I2C_init() puis utilisé I2C_start() sur le registre 0x1D pour vérifier sa fonctionnalité. Ensuite, j'ai mis en œuvre les fonctions read_I2C() et write_I2C() en exploitant les fichiers IP fournis pour les opérations de lecture et d'écriture.

- Affichage UART : Pour afficher les données sur l'UART, j'ai lu les registres X0(0x32), X1(0x33), Y0(0x34), Y1(0x35), Z0(0x36) et Z1(0x37). Ensuite, j'ai effectué une opération de décalage et un OU logique entre le bit de poids faible (LSB) et le bit de poids fort (MSB) pour obtenir les mesures finales. Enfin, j'ai affiché les valeurs obtenues sur l'UART.

![Uploading image.png…]()

- _Calibration_ : Pour calibrer l'accéléromètre, j'ai initialement réglé les registres OFSX(0x1E), OFSY(0x1F) et OFSZ(0x20) à 0, éliminant ainsi toute valeur de décalage. Ensuite, j'ai lu les mesures sur l'UART. Par exemple, pour Z, j'ai obtenu la valeur 7c0 (en base 10 : 1984, multipliée par 3.9 : 7737 m/s²). La valeur souhaitée pour Z, lorsque la carte est plate, est de 9800 m/s². Par conséquent, j'ai ajouté progressivement un décalage au registre OFSZ pour obtenir le résultat souhaité. J'ai suivi le même processus pour X et Y afin d'obtenir 0 m/s² pour les deux registres.



<h3> Conclusion </h3>


