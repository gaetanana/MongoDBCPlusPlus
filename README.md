MongoDBPlusPlus
==============

********
Bienvenue dans mon projet qui a pour objectif de tester MongoDB dans un programme C++.

Prérequis
---------
- Avoir le dossier compressé "MongoDBCPlusPlus.zip" (Vous devez forcément avoir le fichier ZIP, car dans le dépôt GitHub, il n'y a pas le driver).
- Avoir l'IDE Visual Studio 2019
- Un serveur MongoDB qui tourne (vous trouverez un tutoriel à l'emplacement suivant : https://github.com/gaetanana/MongoDBC/blob/main/Documentation/InstallationServeurMongoDB.pdf.

Lancement du programme
----------------------

Pour lancer le programme, il vous suffit de lancer le fichier ``main.cpp`` avec Visual Studio 2019,


Utilisation du programme
------------------------

Lorsque vous lancez le programme, vous devriez avoir l'interface suivante :

![imgLancement.png](imgREADME%2FimgLancement.png)

Une fois que vous êtes sur cette interface, vous pouvez tapez 1 pour accéder au menu create, 2 pour accéder au menu read,
3 pour accéder au menu update et 4 pour accéder au menu delete ou 5 si vous souhaitez quitter le programme.

Si vous tapez 1, vous accéderez au menu create :

![CREATE.png](imgREADME%2FCREATE.png)

Si vous tapez 2, vous accéderez au menu read :

![READ.png](imgREADME%2FREAD.png)

Si vous tapez 3, vous accéderez au menu update :

![UPDATE.png](imgREADME%2FUPDATE.png)

Si vous tapez 4, vous accéderez au menu delete :

![DELETE.png](imgREADME%2FDELETE.png)

Si vous tapez 5, vous quitterez le programme :

![quitter.png](imgREADME%2Fquitter.png)

En suite selon le menu que vous avez choisi, vous pourrez faire des actions sur la base de données MongoDB.

**Dans cette application, il y a la gestion des erreurs, donc si vous faites une faute de frappe, le programme vous le dira et vous pourrez retaper votre commande.**
