#include "../../../Global.h"
#include "LoginState.h"
#include "../../../User/User.h"
#include "../../../Character/Character.h"

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
							int index = 0;
							
							character->SetName(vec[0][2].second);
							character->SetLevel(atoi(vec[0][3].second.c_str()));
							character->SetHP(atof(vec[0][4].second.c_str()));
							character->SetPower(atof(vec[0][5].second.c_str()));
							character->SetSpeed(atof(vec[0][6].second.c_str()));
							//character->SetMapKey(atoi(vec[0][7].second.c_str()));
							character->SetMapKey(3);

							auto position = Vector3(vec[0][8].second, vec[0][9].second, vec[0][10].second);
							position.y+= 100*character->GetCode();
							printf("%f, %f, %f \n", position.x, position.y, position.z);
							character->SetPosition(position);							
							user->SetCharacer(character);
							character->Init();

							//유저에게 보내주기						
							auto fbb = make_shared< flatbuffers::FlatBufferBuilder >();
							auto nick = fbb->CreateString(character->GetName());
							auto charB = FB::CharacterBuilder(*fbb);

							charB.add_code(charcode++);
							charB.add_type(FB::CharacterType::CharacterType_Sinbi);
							charB.add_nick(nick);
							charB.add_position(&character->GetPosition().ToFBVector3());
							charB.add_hp(character->GetHP());
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
