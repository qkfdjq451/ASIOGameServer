#include "../../../Global.h"
#include "LoginState.h"
#include "../../../User/User.h"
#include "../../../Character/Character.h"
#include "../../../StatusManager/StatusManager.h"





void LoginState::On_Read(const PS & symbol, void * recv_data, unsigned short size)
{
	switch (symbol)
	{
	case PS::REQ_SELECT_CHARACTER:		
	{
		auto ss = session.lock();
		if (ss)
		{
			//TODO : Character DB에서 데이터를 받아와 값을 대입해야 하지만 우선  임시로 1번 캐릭터의 정보를 넣어줌
			MySQLManager::Get()->Async_Query(MySQLManager::Select_UserCharacter(1).c_str(), 
				[ss](QVector& vec, bool result)
			{
				if (result)
				{
					auto func = std::make_shared<Function<void>>([ss,vec]
					{
						static int charcode = 0;
						auto user = ss->GetUser().lock();
						if (user)
						{
							//Character 객체 만들기
							auto character = std::make_shared<Character>(user, charcode);							
							
							character->SetTypeCode(stoi(vec[0][2].second));
							character->SetName(vec[0][3].second);
							character->SetLevel(stoi(vec[0][4].second));
														
							StatusManager::Get()->SetStatus(character);							
							character->SetMapKey(stoi(vec[0][5].second));
							auto position = Vector3(vec[0][6].second, vec[0][7].second, vec[0][8].second);																					
							character->SetPosition(position);							
							user->SetCharacer(character);
							character->Init();

							//유저에게 보내주기						
							auto fbb = make_shared< flatbuffers::FlatBufferBuilder >();
							auto nick = fbb->CreateString(character->GetName());
							auto charB = FB::CharacterBuilder(*fbb);

							charB.add_code(charcode++);
							charB.add_type((FB::CharacterType)character->GetTypeCode());
							charB.add_nick(nick);
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
				}//if (result)
			});
		}//if (ss)
		break;
	}//PS::REQ_SELECT_CHARACTER:
	
	


	}//switch (symbol)
}
