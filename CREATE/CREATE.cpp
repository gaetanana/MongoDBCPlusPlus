//
// Created by g.gonfiantini on 16/05/2023.
//

#include "CREATE.h"

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

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace std;

//************************** Fonctions auxiliaires **************************//

/**
 * Cette fonction permet de savoir si une collection existe dans la base de données
 */
bool collectionExist(mongocxx::database database, string collectionName){
    auto cursor = database.list_collections(); //Cursor sur la liste des collections
    //On parcourt le cursor
    for (auto&& doc : cursor) {
        //Si le nom de la collection est le même que celui entré en paramètre alors on retourne true
        if (doc["name"].get_utf8().value.to_string() == collectionName) {
            return true;
        }
    }
    return false;
}

/**
 * Cette fonction prend en paramètre du XML et le convertit en JSON
 * @param xml
 * @return Json::Value
 */
Json::Value xmlNodeToJson(const pugi::xml_node& xmlNode){
    // Création d'un noeud JSON vide
    Json::Value jsonNode;

    // Parcours de tous les attributs du noeud XML
    for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute()){
        // Ajoute chaque attribut au noeud JSON
        jsonNode[attr.name()] = attr.value();
    }

    // Parcours de tous les noeuds enfants du noeud XML
    for (pugi::xml_node child = xmlNode.first_child(); child; child = child.next_sibling()){
        // Si l'enfant est un texte (pcdata)
        if (child.first_child().type() == pugi::node_pcdata){
            // Crée un nouveau noeud JSON pour l'enfant
            Json::Value childJson;
            // Ajoute la valeur du texte de l'enfant au JSON
            childJson["value"] = child.child_value();

            // Parcours tous les attributs de l'enfant
            for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute()){
                // Ajoute chaque attribut à la section "attributes" du JSON
                childJson["attributes"][attr.name()] = attr.value();
            }

            // Ajoute le JSON de l'enfant au JSON du noeud courant
            jsonNode[child.name()].append(childJson);
        } else {
            // Si l'enfant n'est pas un texte, effectue la conversion récursivement
            jsonNode[child.name()].append(xmlNodeToJson(child));
        }
    }

    // Renvoie le JSON du noeud
    return jsonNode;
}

std::pair<string, long long> xmlToJson(string xml){
    // Début du chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    // Crée un nouveau document XML
    pugi::xml_document doc;

    // Essaye de charger le XML depuis la chaîne en paramètre
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    // Si le chargement échoue, affiche une erreur et renvoie une paire vide
    if (!result) {
        std::cout << "Erreur lors du chargement du fichier XML : " << result.description() << "\n";
        return std::make_pair("", 0);
    }

    // Récupère le noeud racine du document
    pugi::xml_node root = doc.document_element();

    // Convertit le noeud racine en JSON
    Json::Value rootJson = xmlNodeToJson(root);

    // Prépare un constructeur de flux de sortie pour le JSON
    Json::StreamWriterBuilder builder;
    // Configure l'indentation du JSON
    builder["indentation"] = "\t";

    // Convertit le JSON en chaîne de caractères
    string jsonStr = Json::writeString(builder, rootJson);

    // Fin du chrono
    auto stop = chrono::high_resolution_clock::now();
    // Calcule la durée de l'exécution
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Renvoie la chaîne JSON et le temps d'exécution
    return std::make_pair(jsonStr, duration.count());
}

/**
 * Cette fonction permet de lire en mémoire tous les fichiers XML d'un dossier et renvoie un vecteur de chaîne
 * @param path
 * @return vector<string>
 */
std::vector<std::string> chargementEnMemoireXML(const std::string& dirPath) {
    //Chrono pour mesurer le temps de la mise en mémoire des fichiers XML
    auto start = chrono::high_resolution_clock::now();

    std::vector<std::string> xmlFilesContents;
    // Parcours chaque fichier dans le dossier
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".xml") {
            // Ouvre le fichier et charge son contenu dans un flux de chaînes
            std::ifstream file(entry.path());
            if (!file) {
                std::cout << "Erreur lors de l'ouverture du fichier " << entry.path() << ".\n";
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            xmlFilesContents.push_back(buffer.str());
        }
    }
    //Fin du chrono
    auto stop = chrono::high_resolution_clock::now();
    //Calcule la durée de l'exécution
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    std::cout << "Temps de chargement en memoire des fichiers XML : " << duration.count() << " microsecondes\n";
    return xmlFilesContents;
}



//************************** Fonctions CREATE **************************//

/**
* Cette fonction permet de créer une collection dans la base de données MongoDB
 * Elle permet de créer une collection dans la base de données actiaDataBase
*/
void createCollection(mongocxx::client& client) {
    mongocxx::database db = client["actiaDataBase"]; //On récupère la base de données actiaDataBase
    //On demande à l'utilisateur d'entrer le nom de la collection
    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    //Vérifie si la collection existe déjà
    if(collectionExist(db, collectionName)){
        std::cout << "La collection existe deja.\n";
        return;
    }
    //Création de la collection
    bsoncxx::document::value create_collection_cmd =
            bsoncxx::builder::stream::document{}
                    << "create" << collectionName
                    << bsoncxx::builder::stream::finalize;
    //On exécute la commande
    db.run_command(create_collection_cmd.view());
    //On affiche un message de confirmation de la création de la collection
    std::cout << "Collection " << collectionName << " créée avec succès.\n";
}

/**
 * Cette fonction permet de créer un document dans une collection de la base de données MongoDB
 * Elle demande à l'utilisateur dans quelle collection il veut créer un document et un chemin absolu vers un fichier XML
 * et la fonction convertit le XML en JSON et l'insère dans la collection
 */
void createOneDocumentJSON(mongocxx::client& client) {
    //On récupère la base de données actiaDataBase
    std::string dbName = "actiaDataBase";
    mongocxx::database db = client[dbName];

    //On demande à l'utilisateur d'entrer le nom de la collection dans laquelle il veut créer un document
    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    mongocxx::collection coll = db[collectionName];

    //On demande à l'utilisateur d'entrer le chemin absolu vers le fichier XML
    std::string filePath;
    std::cout << "Veuillez entrer le chemin absolu vers le fichier XML : ";
    std::getline(std::cin, filePath);

    //On ouvre le fichier
    std::ifstream file(filePath);
    if (!file) {
        std::cout << "Erreur lors de l'ouverture du fichier.\n";
        return;
    }
    //On lit le fichier et on le stocke dans un string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlStr = buffer.str();
    //On convertit le XML en JSON
    auto result = xmlToJson(xmlStr);
    string jsonStr = result.first; //On récupère le JSON
    long long duration = result.second; //On récupère le temps d'exécution de la conversion
    cout << "Temps d'éxecution de la conversion :  " << duration << " microsecondes" << endl;
    bsoncxx::document::value document = bsoncxx::from_json(jsonStr);
    //On insère le document dans la collection
    bsoncxx::stdx::optional<mongocxx::result::insert_one> resultInsert = coll.insert_one(document.view());
    //On affiche un message de confirmation de l'insertion du document
    if(resultInsert) {
        std::cout << "Document inséré avec succès.\n";
    } else {
        std::cout << "Erreur lors de l'insertion du document.\n";
    }
}

/**
 * Cette fonction permet de créer plusieurs documents dans une collection de la base de données MongoDB
 * Elle demande à l'utilisateur dans quelle collection il veut créer des documents et un chemin absolu vers un fichier XML
 * et la fonction convertit le XML en JSON et l'insère dans la collection.
 * Cette fonction charge pas tous les fichiers XML en mémoire, elle les lit un par un.
 */
void createManyDocumentsJSON(mongocxx::client& client) {
    // Début du chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    std::string dbName = "actiaDataBase";
    mongocxx::database db = client[dbName];
    long totalTimeConversionXMLToJSON = 0;
    long totalInsertDocument = 0;

    std::string collectionName;
    // Demande à l'utilisateur le nom de la collection
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    mongocxx::collection coll = db[collectionName];

    // Vérifie si la collection existe
    if(!collectionExist(db, collectionName)){
        std::cout << "La collection n'existe pas.\n";
        return;
    }

    std::string dirPath;
    // Demande à l'utilisateur le chemin du dossier contenant les fichiers XML
    std::cout << "Veuillez entrer le chemin absolu vers le dossier contenant les fichiers XML : ";
    std::getline(std::cin, dirPath);

    std::cout << "Traitement en cours . . .";

    // Parcours chaque fichier dans le dossier
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".xml") {

            // Ouvre le fichier et charge son contenu dans un flux de chaînes
            std::ifstream file(entry.path());
            if (!file) {
                std::cout << "Erreur lors de l'ouverture du fichier " << entry.path() << ".\n";
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string xmlStr = buffer.str();

            // Convertit le XML en JSON et mesure le temps d'exécution
            auto result = xmlToJson(xmlStr);
            string jsonStr = result.first;
            long long duration = result.second;
            totalTimeConversionXMLToJSON += duration;

            // Convertit le JSON en document BSON pour l'insertion dans MongoDB
            bsoncxx::document::value document = bsoncxx::from_json(jsonStr);

            // Insère le document dans la collection
            bsoncxx::stdx::optional<mongocxx::result::insert_one> resultInsert = coll.insert_one(document.view());

            // Si l'insertion a réussi, incrémente le compteur de documents insérés
            if(resultInsert) {
                totalInsertDocument++;
            }
        }
    }

    // Fin du chrono et calcul de la durée
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    cout << "Temps d'execution de l'insertion  : " << duration.count() << " secondes" << endl;
    cout << "Temps total de conversion XML vers JSON : " << totalTimeConversionXMLToJSON << " microsecondes" << endl;
    cout << "Nombre de documents inseres : " << totalInsertDocument << endl;
}

/**
 * Cette fonction permet de créer plusieurs documents dans une collection de la base de données MongoDB
 * Elle demande à l'utilisateur dans quelle collection il veut créer des documents et un chemin absolu vers un fichier XML
 * et la fonction convertit le XML en JSON et l'insère dans la collection.
 * Cette fonction charge tous les fichiers XML en mémoire avant de les convertir en JSON et de les insérer dans la collection.
 */
void createManyDocumentsJSONInMemory(mongocxx::client &client){
    // Début du chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();
    long totalTimeConversionXMLToJSON = 0;

    std::string dbName = "actiaDataBase";
    mongocxx::database db = client[dbName];

    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    mongocxx::collection coll = db[collectionName];
    // Vérifie si la collection existe
    if(!collectionExist(db, collectionName)){
        std::cout << "La collection n'existe pas.\n";
        return;
    }

    //Demander le chemin du dossier contenant les fichiers XML
    std::string pathDir;
    std::cout << "Veuillez entrer le chemin absolu vers le dossier contenant les fichiers XML : ";
    std::getline(std::cin, pathDir);

    std::vector<bsoncxx::document::value> documents;
    std::vector<std::string> xmlFiles = chargementEnMemoireXML(pathDir);

    for(const auto& xmlStr : xmlFiles){
        auto result = xmlToJson(xmlStr);
        string jsonStr = result.first;
        long long duration = result.second;
        totalTimeConversionXMLToJSON += duration;

        // Convertit le JSON en document BSON pour l'insertion dans MongoDB
        bsoncxx::document::value document = bsoncxx::from_json(jsonStr);
        // Ajoute le document à la liste
        documents.push_back(document);
    }
    // Insère tous les documents en une seule opération
    coll.insert_many(documents);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    cout << "Temps d'execution de l'insertion  : " << duration.count() << " secondes" << endl;
    cout << "Nombre de documents inseres : " << documents.size() << endl;
    cout << "Temps total de conversion XML vers JSON : " << totalTimeConversionXMLToJSON << " microsecondes" << endl;
}
