#include "Global.h"
#include "MongoDB/MongoDB.h"
#include <cpp_redis/cpp_redis>
#include "Log/LogManager.h"
#include "./Cipher/CipherManager.h"
#include "./EventManager/TimeManager.h"
#include "./EventManager/EventManager.h"
#include"GameManager/GameManager.h"
#include "./Monster/MonsterBuilder.h"
#include "./Navi/Navigation.h"
#include "./DamageFormula/DamageFormula.h"
#include "./StatusManager/StatusManager.h"
#include "./ItemManager/ItemManager.h"

int main(int argc, char* argv[])
{
	int IOThreadCount = 4;

	MySQLManager::Create();
	TimerManager::Create();
	EventManager::Create();
	ItemManager::Create();
	DamageFormula::Create();
	StatusManager::Create();
	MonsterBuilder::Init();
	Navigation::Init();
	
	auto gm = std::make_shared<GameManager>();
	try 
	{
		asio::io_service io_service;

		WorkerGroup::Create(io_service);		
		server s(io_service, (short)std::atoi("8888"));

		vector<thread> threads;
		for(int i=0;i<IOThreadCount;i++)
			threads.push_back(thread([&] {io_service.run(); }));		

		gm->SetTag("GameManager");
		gm->Run();
	}
	catch (std::exception& e) 
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	StatusManager::Delete();
	DamageFormula::Delete();
	ItemManager::Delete();
	EventManager::Delete();
	TimerManager::Delete();
	MySQLManager::Delete();
	return 0;
	
	//mongoc_client_t			*client;
	//mongoc_collection_t	*collection;
	//bson_error_t				error;
	//bson_oid_t					oid;
	//bson_t						*doc = nullptr;


	////몽고디비 드라이버 초기화 + DB클라이언트 생성
	//mongoc_init();
	//client = mongoc_client_new("mongodb://35.201.212.220:27017/");
	//mongoc_client_set_appname(client, "inset_example");

	////컬랙션에 접근할 객체 생성
	//collection = mongoc_client_get_collection(client, "Test", "CollectionTest");



	//json models;
	//json temp;

	//temp["transform"]["position"]["x"] = 2.23;
	//temp["transform"]["position"]["y"] = 2.53;
	//temp["transform"]["position"]["z"] = 547.23;

	//temp["transform"]["rotation"]["x"] = 23.51235;
	//temp["transform"]["rotation"]["y"] = 535.23;
	//temp["transform"]["rotation"]["z"] = 23.3546;

	//temp["transform"]["scale"]["x"] = 1;
	//temp["transform"]["scale"]["y"] = 1;
	//temp["transform"]["scale"]["z"] = 1;

	//models["models"].push_back(temp);

	//temp["transform"]["position"]["x"] = 235263;
	//temp["transform"]["position"]["y"] = 2.576553;
	//temp["transform"]["position"]["z"] = 5472355.23;

	//temp["transform"]["rotation"]["x"] = 23.51235;
	//temp["transform"]["rotation"]["y"] = 535.23;
	//temp["transform"]["rotation"]["z"] = 23.3546;

	//temp["transform"]["scale"]["x"] = 2;
	//temp["transform"]["scale"]["y"] = 2;
	//temp["transform"]["scale"]["z"] = 2;

	//models["models"].push_back(temp);


	////Json to Bsoon
	//bson_t* document;
	//auto jsontt = models.dump();

	//document = bson_new_from_json((const uint8_t *)jsontt.c_str(), -1, &error);

	//if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, document, NULL, &error)) {
	//	fprintf(stderr, "%s\n", error.message);
	//}

	//char *str;
	//str = bson_as_json(document, NULL);
	//printf("%s\n", str);
	//// str 문자열 메모리 해제
	//bson_free(str);

	//cpp_redis::client client;
	//client.connect("35.201.212.220", 6379, [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status)
	//{

	//});

	//client.auth("1234");

	////client.set("hello", "42", [](cpp_redis::reply& reply)
	////{
	////	std::cout << "set hello 42: " << reply << std::endl;
	////	// if (reply.is_string())
	////	//   do_something_with_string(reply.as_string())
	////});

	//auto reply = client.get("hello");
	//client.sync_commit();
	//std::cout << "get hello: " << reply.get() << std::endl;

//
//auto aaconsole = spd::stdout_color_mt("console");
//aaconsole->info("Welcome to spdlog!");
//aaconsole->error("Some error message with arg{}..", 1);
//aaconsole->warn("Easy padding in numbers like {:08d}", 12);
//aaconsole->critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
//aaconsole->info("Support for floats {:03.2f}", 1.23456);
//aaconsole->info("Positional args are {1} {0}..", "too", "supported");
//aaconsole->info("{:<30}", "left aligned");
//
//byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
//memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
//memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
//
//std::string plaintext = "아 이런 너무 복잡하자너~";
//std::string ciphertext;
//std::string decryptedtext;
//
//std::cout << "플레인 텍스트 (" << plaintext.size() << "바이트)" << std::endl;
//std::cout << plaintext;
//std::cout << std::endl << std::endl;
//
//CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
//CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);
//CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
//
//stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()),
//	plaintext.length() + 1);
//stfEncryptor.MessageEnd();
//
//std::cout << "씨퍼 텍스트 (" << plaintext.size() << "바이트)" << std::endl;
//for (int i = 0; i < ciphertext.size(); i++)
//{
//	std::cout << "0x" << std::hex << (0xFF & static_cast<byte>(ciphertext[i])) << " ";
//}
//
//
//std::cout << std::endl << std::endl;
//
////복호화 셋팅
//CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
//CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);
//CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption
//	, new CryptoPP::StringSink(decryptedtext));
//
////복호화
//stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
//stfDecryptor.MessageEnd();
//
//std::cout << "복호화 텍스트 :" << std::endl;
//std::cout << decryptedtext;
//std::cout << std::endl << std::endl;
//	return 0;

}
