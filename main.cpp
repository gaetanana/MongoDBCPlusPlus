#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"

#include <ctime>
#include <thread>

#include "CREATE/CREATE.h"
#include "DELETE/DELETE.h"
#include "READ/READ.h"
#include "UPDATE/UPDATE.h"

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

int main() {
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://root:examplepassword@localhost:27017");
    mongocxx::client client(uri);
    while (true) {

        std::cout << "\n========================================================\n";
        std::cout << "|| Programme de test de la base de donnees MongoDB    ||\n";
        std::cout << "========================================================\n";
        std::cout << "============== Veuillez choisir une option : ===========\n";
        std::cout << "||                                                    ||\n";
        std::cout << "|| 1 - Create                                         ||\n";
        std::cout << "|| 2 - Read                                           ||\n";
        std::cout << "|| 3 - Update                                         ||\n";
        std::cout << "|| 4 - Delete                                         ||\n";
        std::cout << "|| 5 - Quitter le programme                           ||\n";
        std::cout << "||                                                    ||\n";
        std::cout << "========================================================\n";
        int choix;
        std::cout << "Votre choix : ";
        std::cin >> choix;
        std::cout << "\n";
        if (choix == 1) {
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Create                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Creer une collection dans la base de donnes 'actiaDataBase'   ||\n";
            std::cout << "|| 2 - Creer un document avec un fichier XML en valeur               ||\n";
            std::cout << "|| 3 - Stocker l'ensemble des fichiers XML d'un dossier dans Redis   ||\n";
            std::cout << "|| 4 - Quitter le menu Create                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "========================================================================\n";
            int choixCreate;
            std::cout << "\nVotre choix : ";
            std::cin >> choixCreate;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if(choixCreate == 1 ){
                //Création de la collection
                createCollection(client);
            }
            else if(choixCreate == 2){
                //Création d'un document
                createOneDocumentJSON();
            }
            else if(choixCreate == 3){
                //Création de plusieurs documents
                createManyDocumentsJSON();
            }
            else if(choixCreate == 4){
                std::cout << "\nVous avez quitté le menu Create\n";
            }
        }
        else if(choix == 2){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Read                                ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Lire la valeur d'un document                                  ||\n";
            std::cout << "|| 2 - Lire tous les documents d'une collection                      ||\n";
            std::cout << "|| 3 - Lire tous les documents qui ont un type Human                 ||\n";
            std::cout << "|| 4 - Lire tous les documents qui ont un type Human avec            ||\n";
            std::cout << "||     une probabilite superieure a 0.5                              ||\n";
            std::cout << "|| 5 - Lire tous les documents qui ont un type Human avec une        ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23                                           ||\n";
            std::cout << "|| 6 - Lire tous les documents qui ont un type  Human avec une       ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23 et qui sont de genre masculin             ||\n";
            std::cout << "|| 7 - Quitter le menu Read                                          ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";

            int choixRead;
            std::cout << "\nVotre choix : ";
            std::cin >> choixRead;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // consomme la nouvelle ligne

            if(choixRead == 1){
                //Lire la valeur d'un document
                readOneDocument();

            }else if(choixRead == 2){
                //Lire tous les documents d'une collection
                readAllDocument();
            }
            else if(choixRead == 3){
                readAllDocumentWithHuman();
            }
            else if(choixRead == 4){
                readAllDocumentWithHumanProbability();
            }
            else if(choixRead == 5){
                readAllDocumentWithHumanProbabilityAndDate();
            }
            else if(choixRead == 6){
                readAllDocumentWithHumanProbabilityAndDateGender();
            }
            else if(choixRead == 7){
                std::cout << "\nVous avez quitté le menu Read\n";
            }
        }
        else if(choix == 3){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Update                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Modifier la valeur Human d'un document                        ||\n";
            std::cout << "|| 2 - Modifier toutes valeurs Human par une autre valeur            ||\n";
            std::cout << "|| 3 - Quitter le menu Update                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";
            int choixUpdate;
            std::cout << "\nVotre choix : ";
            std::cin >> choixUpdate;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if(choixUpdate == 1){
                //Modifier la valeur Human d'un document
                updateOneDocument();
            }
            else if(choixUpdate == 2){
                //Modifier toutes valeurs Human par une autre valeur
                updateAllHumanDocument();
            }
            else if(choixUpdate == 3){
                std::cout << "\nVous avez quitté le menu Update\n";
            }
        }
        else if(choix == 4){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Delete                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Supprimer une document avec son id d'une collection           ||\n";
            std::cout << "|| 2 - Supprimer tous les documents d'une collection                 ||\n";
            std::cout << "|| 3 - Quitter le menu Delete                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";
            int choixDelete;
            std::cout << "\nVotre choix : ";
            std::cin >> choixDelete;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if(choixDelete == 1){
                deleteOneDocument();
            }else if(choixDelete == 2){
                deleteAllDocuments();
            }else if(choixDelete == 3){
                std::cout << "\nVous avez quitté le menu Delete\n";
            }
        }
        else if(choix == 5){
            cout << "========================================================\n";
            cout << "|| Vous avez quitte le programme, a bientot !         ||\n";
            cout << "========================================================\n";
            break;
        }

    }
    return 0;
}
