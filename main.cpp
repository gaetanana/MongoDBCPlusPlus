#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"
#include "CREATE/CREATE.h"
#include <ctime>
#include <thread>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

int main() {
    /*mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb://root:examplepassword@localhost:27017"}};
    auto collection = conn["actiaDataBase"]["c++Collection"];*/

    // Insert a document
    /*bsoncxx::document::value doc_value = document{} << "name" << "John Doe" << "age" << 30 << finalize;
    bsoncxx::document::view doc_view = doc_value.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(doc_view);
    std::cout << "Inserted " << result->inserted_id().get_oid().value.to_string() << std::endl;*/

    // Update the document
    /*document filter_builder{};
    filter_builder << "name" << "John Doe";
    collection.update_one(filter_builder.view(),document{} << "$set" << open_document << "age" << 31 << close_document << finalize);
    std::cout << "Updated" << std::endl;*/

    // Delete the document
    /*document filter_builder_delete{};
    filter_builder_delete << "name" << "John Doe";
    collection.delete_one(filter_builder_delete.view());
    std::cout << "Deleted" << std::endl;*/
    // Read all documents
    /*mongocxx::cursor cursor = collection.find({});
    for(auto doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }*/

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
                createCollection();
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
            std::cout << "|| 1 - Lire la valeur d'une cle                                      ||\n";
            std::cout << "|| 2 - Lire toutes les cles presentes dans la base de donnees        ||\n";
            std::cout << "|| 3 - Lire toutes les cles qui a dans les valeurs un humain         ||\n";
            std::cout << "|| 4 - Lire toutes les cles qui a dans les valeurs un humain avec    ||\n";
            std::cout << "||     une probabilite de survie superieure a 0.5                    ||\n";
            std::cout << "|| 5 - Lire toutes les cle qui a dans les valeurs un humain avec une ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23                                           ||\n";
            std::cout << "|| 6 - Lire toutes les cle qui a dans les valeurs un humain avec une ||\n"
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

            }else if(choixRead == 2){
            }
            else if(choixRead == 3){
            }
            else if(choixRead == 4){
            }
            else if(choixRead == 5){
            }
            else if(choixRead == 6){
            }
            else if(choixRead == 7){
                std::cout << "\nVous avez quitté le menu Read\n";
            }

        }
        else if(choix == 3){
            cout << "========================================================\n";
            cout << "|| Update n'est pas encore disponible                  ||\n";
            cout << "========================================================\n";
        }

        else if(choix == 4){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Delete                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Supprimer une cle                                             ||\n";
            std::cout << "|| 2 - Supprimer toutes les cles                                     ||\n";
            std::cout << "|| 3 - Quitter le menu Delete                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";
            int choixDelete;
            std::cout << "\nVotre choix : ";
            std::cin >> choixDelete;
            if(choixDelete == 1){
            }else if(choixDelete == 2){
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
