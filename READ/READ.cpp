//
// Created by g.gonfiantini on 16/05/2023.
//

#include "READ.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <fstream>
#include <filesystem>
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"
#include "json/value.h"
#include "json/writer.h"
#include "pugixml.hpp"
#include "../CREATE/CREATE.h"
#include <chrono>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

/**
 * Cette fonction permet de lire un document grâce à son ID
 */
void readOneDocument(mongocxx::client &client) {
    mongocxx::database db = client["actiaDataBase"]; //Récupère la base de données
    //Demande à l'utilisateur le nom de la collection
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);
    //Vérifie si la collection existe
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    //Demande à l'utilisateur l'ID du document à lire
    cout << "Entrer l'ID du document a lire : ";
    string id;
    getline(cin, id);
    //Requête pour lire le document
    bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(
            document{} << "_id" << id << finalize);
    //Vérifie si la lecture s'est bien passée
    if (result) {
        cout << bsoncxx::to_json(*result) << "\n";
    } else {
        cout << "Erreur lors de la lecture du document.\n";
    }
}

/**
 * Cette fonction permet de lire tous les documents d'une collection
 * 0 filtre
 * @param client
 */
void readAllDocument(mongocxx::client &client) {
    // Début du chrono pour mesurer le temps d'exécution
    auto startChrono = chrono::high_resolution_clock::now();
    int nbDocuments = 0;

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    mongocxx::cursor cursor = collection.find({});
    for (auto doc: cursor) {
        //cout << bsoncxx::to_json(doc) << "\n";
        //Affiche uniquement l'id du document
        cout << doc["_id"].get_oid().value.to_string() << "\n";
        nbDocuments++;
    }
    cout << "Nombre de documents : " << nbDocuments << "\n";
    //Fin chrono
    auto endChrono = chrono::high_resolution_clock::now();
    auto durationChrono = chrono::duration_cast<chrono::microseconds>(endChrono - startChrono);
    cout << "Temps d'execution : " << durationChrono.count() << " microsecondes\n";

}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human
 * 1 filtre
 * @param client
 */
void readAllDocumentWithHuman(mongocxx::client &client) {
    //Début du chrono pour mesurer le temps d'exécution
    auto startChrono = chrono::high_resolution_clock::now();
    //Compteur de documents lus avec un type Human
    int nbDocuments = 0;

    mongocxx::database db = client["actiaDataBase"];
    //Demande à l'utilisateur le nom de la collection
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);
    //Vérifie si la collection existe
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    //Requête pour lire les documents avec un type Human
    auto cursor = collection.find(
            document{} << "tt:VideoAnalytics.0.tt:Frame.0.tt:Object.0.tt:Appearance.0.tt:Class.0.tt:Type.0.value"
                       << "Human" << finalize);
    //Affiche les documents
    for (auto doc: cursor) {
        //Affiche uniquement l'id du document
        cout << doc["_id"].get_oid().value.to_string() << "\n";
        nbDocuments++;
    }
    cout << "Nombre de documents : " << nbDocuments << "\n";
    //Fin chronomètre
    auto endChrono = chrono::high_resolution_clock::now();
    auto durationChrono = chrono::duration_cast<chrono::microseconds>(endChrono - startChrono);
    cout << "Temps d'execution : " << durationChrono.count() << " microsecondes\n";
}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5.
 * 2 filtres
 * @param client
 */
void readAllDocumentWithHumanProbability(mongocxx::client &client) {
    // Début du chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    // Compteur de documents avec un type Human et une probabilité supérieure à 0.5
    int nbDocs = 0;

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);

    // Vérifie si la collection existe
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    // Récupère tous les documents de la collection
    auto cursor = collection.find({});
    // Parcours chaque document dans la collection
    for (auto doc: cursor) {
        auto analytics = doc["tt:VideoAnalytics"];
        if (analytics && analytics.type() == bsoncxx::type::k_array) {

            // Parcours chaque frame dans l'array "tt:VideoAnalytics"
            for (auto frame: analytics.get_array().value) {
                auto objects = frame["tt:Frame"][0]["tt:Object"];
                if (objects && objects.type() == bsoncxx::type::k_array) {

                    // Parcours chaque appearance dans l'array "tt:Object"
                    for (auto appearance: objects.get_array().value) {
                        auto classes = appearance["tt:Appearance"][0]["tt:Class"];
                        if (classes && classes.type() == bsoncxx::type::k_array) {

                            // Vérifie si une des classes est "Human" avec une probabilité supérieure à 0.5
                            bool hasHumanWithHighLikelihood = std::any_of(classes.get_array().value.begin(),
                                                                          classes.get_array().value.end(),
                                                                          [](auto type) {
                                                                              auto typeVal = type["tt:Type"][0]["value"];
                                                                              auto likelihood = type["tt:Type"][0]["attributes"]["Likelihood"];
                                                                              return typeVal && likelihood &&
                                                                                     likelihood.type() ==
                                                                                     bsoncxx::type::k_utf8 &&
                                                                                     typeVal.get_utf8().value.to_string() ==
                                                                                     "Human" &&
                                                                                     stod(likelihood.get_utf8().value.to_string()) >=
                                                                                     0.50;
                                                                          });

                            // Si une des classes est "Human" avec une probabilité supérieure à 0.5, incrémente le compteur
                            if (hasHumanWithHighLikelihood) {
                                nbDocs++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    // Fin du chrono et calcul de la durée
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Nombre de documents : " << nbDocs << endl;
    cout << "Temps d'execution : " << chrono::duration_cast<chrono::microseconds>(elapsed).count()
         << " microsecondes\n";
}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5 et supérieure à une date donnée dans le programme.
 * 3 filtres
 */
void readAllDocumentWithHumanProbabilityAndDate(mongocxx::client &client) {
    // Début du chrono pour mesurer le temps d'exécution
    auto startChrono = chrono::high_resolution_clock::now();

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);

    // Vérifie si la collection existe
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }

    mongocxx::collection collection = db[collectionName];
    auto cursor = collection.find({});

    int documentCount = 0;

    // Parcours chaque document dans la collection
    for (auto &&doc: cursor) {

        // Parcours chaque 'tt:VideoAnalytics' dans le document
        auto videoAnalytics = doc["tt:VideoAnalytics"].get_array().value;
        for (auto &&analytics: videoAnalytics) {

            // Parcours chaque 'tt:Frame' dans 'tt:VideoAnalytics'
            auto frames = analytics["tt:Frame"].get_array().value;
            for (auto &&frame: frames) {

                // Parcours chaque 'tt:Object' dans 'tt:Frame'
                auto objects = frame["tt:Object"].get_array().value;
                for (auto &&object: objects) {

                    // Parcours chaque 'tt:Appearance' dans 'tt:Object'
                    auto appearances = object["tt:Appearance"].get_array().value;
                    for (auto &&appearance: appearances) {

                        // Parcours chaque 'tt:Class' dans 'tt:Appearance'
                        auto classes = appearance["tt:Class"].get_array().value;
                        for (auto &&cls: classes) {

                            // Parcours chaque 'tt:Type' dans 'tt:Class'
                            auto types = cls["tt:Type"].get_array().value;
                            for (auto &&type: types) {

                                // Extrais les valeurs requises
                                std::string valueType = type["value"].get_utf8().value.to_string();
                                double valueLikelihood = std::stod(
                                        type["attributes"]["Likelihood"].get_utf8().value.to_string());
                                std::string utcTime = frame["UtcTime"].get_utf8().value.to_string();

                                // Comptabilise le document si les conditions sont remplies
                                if (valueType == "Human" && valueLikelihood > 0.5 && utcTime > "2023-04-21T14:45:23") {
                                    documentCount++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "Nombre de documents: " << documentCount << "\n";

    // Fin du chrono et calcul de la durée
    auto endChrono = chrono::high_resolution_clock::now();
    auto durationChrono = chrono::duration_cast<chrono::microseconds>(endChrono - startChrono);
    cout << "Temps d'execution : " << durationChrono.count() << " microsecondes\n";
}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5 et supérieure à une date donnée dans le programme ainsi que le genre masculin supérieur à 0,5.
 * @param client
 */
void readAllDocumentWithHumanProbabilityAndDateGender(mongocxx::client &client) {
    // Début du chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);

    // Vérifie si la collection existe
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    auto cursor = collection.find({});
    int documentCount = 0;

    // Parcours chaque document dans la collection
    for (auto &&doc: cursor) {

        // Parcours chaque 'tt:VideoAnalytics' dans le document
        auto videoAnalytics = doc["tt:VideoAnalytics"].get_array().value;
        for (auto &&analytics: videoAnalytics) {

            // Parcours chaque 'tt:Frame' dans 'tt:VideoAnalytics'
            auto frames = analytics["tt:Frame"].get_array().value;
            for (auto &&frame: frames) {

                // Parcours chaque 'tt:Object' dans 'tt:Frame'
                auto objects = frame["tt:Object"].get_array().value;
                for (auto &&object: objects) {

                    // Parcours chaque 'tt:Appearance' dans 'tt:Object'
                    auto appearances = object["tt:Appearance"].get_array().value;
                    for (auto &&appearance: appearances) {

                        // Parcours chaque 'tt:Class' dans 'tt:Appearance'
                        auto classes = appearance["tt:Class"].get_array().value;
                        for (auto &&cls: classes) {

                            // Parcours chaque 'tt:Type' dans 'tt:Class'
                            auto types = cls["tt:Type"].get_array().value;
                            for (auto &&type: types) {

                                // Extrais les valeurs requises
                                std::string valueType = type["value"].get_utf8().value.to_string();
                                double valueLikelihood = std::stod(type["attributes"]["Likelihood"].get_utf8().value.to_string());
                                std::string utcTime = frame["UtcTime"].get_utf8().value.to_string();

                                // Vérifie la présence de l'information de genre dans 'tt:Extension'
                                if (appearance["tt:Extension"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"] &&
                                    !appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"].get_array().value[0]["value"].get_utf8().value.empty()) {

                                    // Extrais l'information de genre
                                    double valueGender = std::stod(
                                            appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"].get_array().value[0]["value"].get_utf8().value.to_string());

                                    // Comptabilise le document si les conditions sont remplies
                                    if (valueType == "Human" && valueLikelihood > 0.5 && utcTime > "2023-05-16" &&
                                        valueGender > 0.5) {
                                        cout << doc["_id"].get_oid().value.to_string() << "\n";
                                        documentCount++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << "Nombre de documents: " << documentCount << "\n";

    // Fin du chrono et calcul de la durée
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << chrono::duration_cast<chrono::microseconds>(elapsed).count()
         << " microsecondes\n";
}

