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
/**
 * Cette fonction permet de savoir si une collection existe dans la base de données
 *
 */
bool collectionExist(mongocxx::database database, string collectionName){
    auto cursor = database.list_collections();
    for (auto&& doc : cursor) {
        if (doc["name"].get_utf8().value.to_string() == collectionName) {
            return true;
        }
    }
    return false;
}


/**
* Cette fonction permet de créer une collection dans la base de données MongoDB
 * Elle permet de créer une collection dans la base de données actiaDataBase
*/
void createCollection(mongocxx::client& client) {
    mongocxx::database db = client["actiaDataBase"];
    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);

    if(collectionExist(db, collectionName)){
        std::cout << "La collection existe deja.\n";
        return;
    }

    bsoncxx::document::value create_collection_cmd =
            bsoncxx::builder::stream::document{}
                    << "create" << collectionName
                    << bsoncxx::builder::stream::finalize;

    db.run_command(create_collection_cmd.view());
    std::cout << "Collection " << collectionName << " créée avec succès.\n";
}


/**
 * Cette fonction prend en paramètre du XML et le convertit en JSON
 * @param xml
 * @return
  */
Json::Value xmlNodeToJson(const pugi::xml_node& xmlNode){
    Json::Value jsonNode;
    for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute()){
        jsonNode[attr.name()] = attr.value();
    }
    for (pugi::xml_node child = xmlNode.first_child(); child; child = child.next_sibling()){
        if (child.first_child().type() == pugi::node_pcdata){
            Json::Value childJson;
            childJson["value"] = child.child_value();
            for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute()){
                childJson["attributes"][attr.name()] = attr.value();
            }
            jsonNode[child.name()].append(childJson);
        } else {
            jsonNode[child.name()].append(xmlNodeToJson(child));
        }
    }
    return jsonNode;
}


std::pair<string, long long> xmlToJson(string xml){
    //Chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());
    if (!result) {
        std::cout << "Erreur lors du chargement du fichier XML : " << result.description() << "\n";
        return std::make_pair("", 0);
    }
    pugi::xml_node root = doc.document_element();

    Json::Value rootJson = xmlNodeToJson(root);

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "\t";
    string jsonStr = Json::writeString(builder, rootJson);

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    //cout << "Temps d'exécution de la conversion : " << duration.count() << " microsecondes" << endl;

    return std::make_pair(jsonStr, duration.count());
}

/**
 * Cette fonction permet de créer un document dans une collection de la base de données MongoDB
 * Elle demande à l'utilisateur dans quelle collection il veut créer un document et un chemin absolu vers un fichier XML
 * et la fonction convertit le XML en JSON et l'insère dans la collection
 */

void createOneDocumentJSON(mongocxx::client& client) {
    std::string dbName = "actiaDataBase";
    mongocxx::database db = client[dbName];

    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    mongocxx::collection coll = db[collectionName];

    std::string filePath;
    std::cout << "Veuillez entrer le chemin absolu vers le fichier XML : ";
    std::getline(std::cin, filePath);

    std::ifstream file(filePath);
    if (!file) {
        std::cout << "Erreur lors de l'ouverture du fichier.\n";
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string xmlStr = buffer.str();

    auto result = xmlToJson(xmlStr);
    string jsonStr = result.first;
    long long duration = result.second;
    cout << "Temps d'éxecution de la conversion :  " << duration << " microsecondes" << endl;
    bsoncxx::document::value document = bsoncxx::from_json(jsonStr);

    bsoncxx::stdx::optional<mongocxx::result::insert_one> resultInsert = coll.insert_one(document.view());

    if(resultInsert) {
        std::cout << "Document inséré avec succès.\n";
    } else {
        std::cout << "Erreur lors de l'insertion du document.\n";
    }


}

/**
* Cette fonction permet de créer plusieurs documents dans une collection de la base de données MongoDB
 * Elle demande à l'utilisateur dans quelle collection il veut créer des documents et un chemin absolu vers un fichier XML
 * et la fonction convertit le XML en JSON et l'insère dans la collection
*/

void createManyDocumentsJSON(mongocxx::client& client) {
    //Chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();
    std::string dbName = "actiaDataBase";
    mongocxx::database db = client[dbName];
    long totalTimeConversionXMLToJSON = 0;
    long totalInsertDocument = 0;

    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);
    mongocxx::collection coll = db[collectionName];

    if(!collectionExist(db, collectionName)){
        std::cout << "La collection n'existe pas.\n";
        return;
    }

    std::string dirPath;
    std::cout << "Veuillez entrer le chemin absolu vers le dossier contenant les fichiers XML : ";
    std::getline(std::cin, dirPath);

    std::cout << "Traitement en cour . . .";
    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".xml") {

            std::ifstream file(entry.path());
            if (!file) {
                std::cout << "Erreur lors de l'ouverture du fichier " << entry.path() << ".\n";
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string xmlStr = buffer.str();
            auto result = xmlToJson(xmlStr);
            string jsonStr = result.first;
            long long duration = result.second;
            totalTimeConversionXMLToJSON += duration;
            bsoncxx::document::value document = bsoncxx::from_json(jsonStr);

            bsoncxx::stdx::optional<mongocxx::result::insert_one> resultInsert = coll.insert_one(document.view());

            if(resultInsert) {
                totalInsertDocument++;
                //std::cout << "Document insere avec succes pour le fichier " << entry.path() << ".\n";
                //Fin du chrono
            } else {
                //std::cout << "Erreur lors de l'insertion du document pour le fichier " << entry.path() << ".\n";
            }
        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    cout << "Temps d'execution de l'insertion  : " << duration.count() << " secondes" << endl;
    cout << "Temps total de conversion XML vers JSON : " << totalTimeConversionXMLToJSON << " microsecondes" << endl;
    cout << "Nombre de documents inseres : " << totalInsertDocument << endl;
}