#pragma once


struct Vector3
{
	float x;
	float y;
	float z;
	Vector3(const FB::Vec3* vec) :x(vec->x()), y(vec->y()), z(vec->z()) {}

	Vector3(float x = 0, float y = 0, float z = 0)
		:x(x), y(y), z(z) {}
	Vector3(std::string x, std::string y, std::string z )
		:x((float)stof(x)), y((float)stof(y)), z((float)stof(z)) {}
	FB::Vec3 ToFBVector3()
	{
		return FB::Vec3(x, y, z);
	}


	//Math
	bool Normalize(float Tolerance= SMALL_NUMBER)
	{
		const float SquareSum = x * x + y * y + z * z;
		if (SquareSum > Tolerance)
		{
			const float Scale = 1.f / sqrtf(SquareSum);
			x *= Scale; 
			y *= Scale; 
			z *= Scale;
			return true;
		}
		return false;
	}

	Vector3 operator-=(const Vector3& V)
	{
		x -= V.x; y -= V.y; z -= V.z;
		return *this;
	}

	Vector3 operator+=(const Vector3& V)
	{
		x += V.x; y += V.y; z += V.z;
		return *this;
	}

	Vector3 operator-=(const float& Scale)
	{
		x -= Scale; y -= Scale; z -= Scale;
		return *this;
	}

	Vector3 operator+=(const float& Scale)
	{		
		x += Scale; y += Scale; z += Scale;
		return *this;
	}

	Vector3 operator+(const Vector3& V)
	{
		return Vector3(x + V.x, y + V.y, z + V.z);
	}

	Vector3 operator-(const Vector3& V)
	{
		return Vector3(x - V.x, y - V.y, z - V.z);
	}

	Vector3 operator+(const float& Scale)
	{
		return Vector3(x + Scale, y + Scale, z + Scale);
	}

	Vector3 operator-(const float& Scale)
	{
		return Vector3(x - Scale, y - Scale, z - Scale);
	}


	Vector3 operator*=(const float& Scale)
	{
		x *= Scale; y *= Scale; z *= Scale;
		return *this;
	}

	Vector3 operator*(const float& Scale)
	{
		return Vector3(x * Scale, y * Scale, z * Scale);
	}

	void operator=(const Vector3& V)
	{
		x = V.x; y = V.y; z = V.z;
	}

	std::string ToString()
	{
		std::string str;
		str = "X : " + std::to_string(x) + " Y : " + std::to_string(y) + " Z : " + std::to_string(z);
		return str;
	}

	Vector3 ToUnrealLocation()
	{
		Vector3 temp = *this;
		this->x = -temp.x * 100.0f;
		this->y = -temp.z * 100.0f;
		this->z = temp.y * 100.0f;
		return *this;
	}

	Vector3 ToGLLocation()
	{
		Vector3 temp = *this;
		this->x = -temp.x * 0.01f;
		this->y = temp.z * 0.01f;
		this->z = -temp.y * 0.01f;
		return *this;
	}

	static float Distance(Vector3 first, Vector3 second)
	{
		Vector3 temp=first - second;
		temp.x *= temp.x;
		temp.y *= temp.y;
		temp.z *= temp.z;
		return std::sqrt(temp.x + temp.y + temp.z);
	}
};

struct MoveData
{
	int userCode;
	int userMap;

	MoveType moveType;
	Vector3 position;
	Vector3 foward;
	float speed;
};



union RecvBuffer
{
	uint8 buffer[BUFSIZE];
	struct RecvPacket
	{
		unsigned short size;
		PS symbol;
		uint8 buffer;
	} packet;
};

struct SendBuffer : public std::enable_shared_from_this<SendBuffer>
{
	SendBuffer(const PS& symbol, std::shared_ptr<flatbuffers::FlatBufferBuilder> fbb)
	{		
		buffer.packet.size = fbb->GetSize() + sizeof(unsigned short) + sizeof(PS);

		buffer.packet.symbol = symbol;
		memcpy(buffer.packet.data_buffer, fbb->GetBufferPointer(), fbb->GetSize());
	}

	SendBuffer(const PS& symbol, void* buffer_pointer, short buffersize)
	{	
		buffer.packet.size = buffersize + sizeof(unsigned short) + sizeof(PS);

		buffer.packet.symbol = symbol;
		memcpy(buffer.packet.data_buffer, buffer_pointer, buffersize);
	}

	SendBuffer(const PS& symbol)
	{
		buffer.packet.size =  sizeof(unsigned short) + sizeof(PS);
		buffer.packet.symbol = symbol;
	}

	union SendPacket
	{
		uint8 start_pointer;
		struct Packet
		{
			unsigned short  size;
			PS symbol;
			uint8 data_buffer[BUFSIZE];
		} packet;
	} buffer;
};

class Func
{
public:
	virtual void func() = 0;
};

template<typename T>
class Function :public Func, public std::enable_shared_from_this<Function<T>>
{
public:
	Function(std::function<void(T)> fun, T param) :
		data(param), fun(fun)
	{}
	void func()
	{
		fun(data);
	}
private:
	std::function<void(T)> fun;
	T data;	
};


template<>
class Function<void> :public Func
{
public:
	Function(std::function<void()> fun)
		:fun(fun)
	{}
private:
	std::function<void()> fun;
	void func()
	{
		fun();
	}
};
