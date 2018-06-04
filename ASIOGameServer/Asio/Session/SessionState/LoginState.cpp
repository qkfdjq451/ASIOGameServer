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
			//TODO : Character DB에서 정보를 넣어줘야 하지만 우선  임시로 넣어줌
			MySQLManager::Get()->Async_Query(MySQLManager::Select_UserCharacter(1), 
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
							auto characer = std::make_shared<Character>(user);
							int index = 0;
							characer->SetName(vec[0][2].second);
							characer->SetLevel(atoi(vec[0][3].second.c_str()));
							characer->SetHP(atof(vec[0][4].second.c_str()));
							characer->SetPower(atof(vec[0][5].second.c_str()));
							characer->SetSpeed(atof(vec[0][6].second.c_str()));
							characer->SetMapKey(atoi(vec[0][7].second.c_str()));
							characer->SetPosition(Vector3(vec[0][8].second, vec[0][9].second, vec[0][10].second));
							user->SetCharacer(characer);
							characer->Init();

							//유저에게 보내주기						
							auto fbb = make_shared< flatbuffers::FlatBufferBuilder >();
							auto nick = fbb->CreateString(characer->GetName());
							auto charB = FB::CharacterBuilder(*fbb);

							charB.add_code(charcode++);
							charB.add_type(FB::CharacterType::CharacterType_Sinbi);
							charB.add_nick(nick);
							charB.add_position(&characer->GetPosition().ToFBVector3());
							charB.add_hp(characer->GetHP());
							charB.add_power(characer->GetPower());
							charB.add_speed(characer->GetSpeed());
							charB.add_mapcode(characer->GetMapKey());
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
