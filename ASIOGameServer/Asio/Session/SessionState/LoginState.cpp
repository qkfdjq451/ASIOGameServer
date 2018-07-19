#include "../../../Global.h"
#include "LoginState.h"
#include "../../../User/User.h"
#include "../../../Character/Character.h"
#include "../../../StatusManager/StatusManager.h"





void LoginState::On_Read(const PS & symbol, void * recv_data, unsigned short size)
{
	switch (symbol)
	{
	case PS::REQ_DUMMIY_CHARACTER:
	{
		auto ss = session.lock();
		if (ss)
		{
			//TODO : Character DB���� �����͸� �޾ƿ� ���� �����ؾ� ������ �켱  �ӽ÷� 1�� ĳ������ ������ �־���
			// �ɸ��Ͱ� �ִ��� Ȯ��
			static int charcode = 0;
			int currentCode = --charcode;
			string query;
			query = "Select Count(*) From UserCharacter Where UserCharacter.CharacterCode=" + to_string(currentCode);
			MySQLManager::Get()->Async_Query_One_Select(query.c_str(),
				[ss, currentCode](string str, bool result)
			{
				int count = stoi(str);
				if (result)
				{
					//�ɸ��� ���� �� �켱 �ɸ��͸� DB�� �����Ѵ�.
					if (count == 0)
					{
						string query;
						query = "Call CreateDummyCharacter(";
						query += to_string(currentCode);
						query += ",'�׽�Ʈ�ɸ���" + to_string(currentCode*-1) + "')";
						string retstr;
						MySQLManager::Get()->Query(query.c_str(), std::ref(retstr));
						int err = stoi(retstr);
						//�ַ��� �־��� �� ó��
						if (err<0)
						{
							
						}
					}
					//�ɸ��� ���� ��ȸ�ϱ�
					string query = "Call GetCharacterInfo(" + to_string(currentCode)+")";
					auto vec = MySQLManager::Get()->Query_Vector(query.c_str());

					auto func = std::make_shared<Function<void>>([ss, vec, currentCode]
					{
						auto user = ss->GetUser().lock();
						if (user)
						{
							//Character ��ü �����
							auto character = std::make_shared<Character>(user, currentCode);

							character->SetTypeCode((FB::CharacterType)stoi(vec[0][2].second));

							auto type = character->GetTypeCode();
							character->SetName(vec[0][3].second);
							character->SetLevel(stoi(vec[0][4].second));

							cout << character->GetName() << endl;

							StatusManager::Get()->SetStatus(character);
							character->SetMapKey(stoi(vec[0][5].second));
							character->SetStartPoint(stoi(vec[0][6].second));
							auto position = Vector3(vec[0][7].second, vec[0][8].second, vec[0][9].second);
							character->SetWarpLocateDestination(position);
							character->SetPosition(position);
							user->SetCharacer(character);
							character->Init();


							//�������� �����ֱ�
							auto fbb = make_shared< flatbuffers::FlatBufferBuilder >();
							auto nick = fbb->CreateString(character->GetName());
							auto charB = FB::CharacterBuilder(*fbb);

							charB.add_code(character->GetCode());
							charB.add_type((FB::CharacterType)character->GetTypeCode());
							charB.add_nick(nick);
							charB.add_level(character->GetLevel());
							charB.add_position(&character->GetPosition().ToFBVector3());
							charB.add_hp(character->GetMaxHP());
							charB.add_power(character->GetPower());
							charB.add_speed(character->GetSpeed());
							charB.add_mapcode(character->GetMapKey());
							auto offsetchar = charB.Finish();
							fbb->Finish(offsetchar);

							ss->SetState(ESessionState::InGame);
							ss->PushSend(PS::CON_SELECT_CHARACTER, move(fbb));
						}
					});
					WorkerGroup::Get_IOGroup()->PostFuction(move(func));					
				}
			});
		}//if (ss)
			//MySQLManager::Get()->Async_Query(query.c_str(),
			//	[ss](QVector& vec, bool result)
			//{
			//	if (result)
			//	{
			//		//
			//		auto func = std::make_shared<Function<void>>([ss,vec]
			//		{
			//			static int charcode = 0;
			//			auto user = ss->GetUser().lock();
			//			if (user)
			//			{
			//				//Character ��ü �����
			//				auto character = std::make_shared<Character>(user, charcode++);							
			//				
			//				character->SetTypeCode((FB::CharacterType)stoi(vec[0][2].second));

			//				auto type = character->GetTypeCode();
			//				character->SetName(vec[0][3].second);
			//				character->SetLevel(stoi(vec[0][4].second));

			//				cout << character->GetName() << endl;

			//				StatusManager::Get()->SetStatus(character);							
			//				character->SetMapKey(stoi(vec[0][5].second));
			//				auto position = Vector3(vec[0][6].second, vec[0][7].second, vec[0][8].second);																					
			//				character->SetWarpLocateDestination(position);
			//				character->SetPosition(position);
			//				user->SetCharacer(character);
			//				character->Init();

			//				
			//				//�������� �����ֱ�
			//				auto fbb = make_shared< flatbuffers::FlatBufferBuilder >();
			//				auto nick = fbb->CreateString(character->GetName());
			//				auto charB = FB::CharacterBuilder(*fbb);

			//				charB.add_code(character->GetCode());
			//				charB.add_type((FB::CharacterType)character->GetTypeCode());
			//				charB.add_nick(nick);
			//				charB.add_position(&character->GetPosition().ToFBVector3());
			//				charB.add_hp(character->GetMaxHP());
			//				charB.add_power(character->GetPower());
			//				charB.add_speed(character->GetSpeed());
			//				charB.add_mapcode(character->GetMapKey());
			//				auto offsetchar = charB.Finish();
			//				fbb->Finish(offsetchar);

			//				ss->SetState(ESessionState::InGame);
			//				ss->PushSend(PS::CON_SELECT_CHARACTER, move(fbb));
			//			}
			//		});
			//		WorkerGroup::Get_IOGroup()->PostFuction(move(func));
				//}//if (result)
			//});		
		break;
	}//PS::REQ_SELECT_CHARACTER:
	
	


	}//switch (symbol)
}
