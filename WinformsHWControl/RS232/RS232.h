#pragma once
#include<queue>
#include<mutex>
#include<vector>
#include <iostream>

namespace COM
{
	constexpr int MsgDataBufferSize = 3;
	constexpr int BufferMsgQueueSize = 512;
	constexpr int ResponseSize = 59;

	enum Device { Stepper, LED };

	struct RS232Message
	{
		long long MessageID;
		static unsigned int MessageNum;
		Device DeviceName;
		int* Data = nullptr;

		RS232Message(Device stDeviceName, int* pData)
		{
			MessageID = GenerateUUID();
			DeviceName = stDeviceName;
			Data = pData;
			MessageNum = MessageNum + 1 % BufferMsgQueueSize;
		}

		long long GenerateUUID()
		{
			// Get current time point with nanosecond precision
			auto now = std::chrono::system_clock::now();
			auto duration = now.time_since_epoch();
			auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

			return millis;
		}
	};

	#pragma pack(push, 1)
	struct RS232MessageToSend
	{
		long long MessageID;
		unsigned char DeviceName;
		unsigned int MessageNum;
		int Data[3];

		RS232MessageToSend(RS232Message Msg)
		{
			MessageID = Msg.MessageID;
			DeviceName = Msg.DeviceName;
			MessageNum = Msg.MessageNum;
			for (int i = 0; i < MsgDataBufferSize; i++)
			{
				Data[i] = Msg.Data[i];
			}
		}
	};
	#pragma pack(pop)

	class RS232
	{
	public:
		RS232();
		~RS232();
		void EnqueueMessage(RS232Message Message);
		void Send232Message(RS232Message Message);
		void ClosePortAndStopIOService();
		std::string DequeueReceivedLine();
	private:
		void RecieveMessageAndEnqueue();
		void DequeueAndSendMessage();
		void ReadData();
		void ShowMessageBox(std::wstring& Message, std::string Title);
		void BufferReceived();
		long long GetMsgID(std::string Msg);
		bool IsSameMsgIDAsLast();
		bool IsSameMsgErrIDAsLast();
		bool IsErrorMsg(std::string Msg);
		std::string ParseMessage(std::string Msg);
	private:
		std::queue<RS232Message> m_MessagesToSendQueue;
		std::queue<std::string> m_MessagesToReceiveQueue;
		std::mutex m_MsgsToSendMutex;
		std::mutex m_MsgsToRecieveMutex;
		std::thread m_RS232SenderThread;
		std::thread m_RS232ReceiverThread;
		std::condition_variable m_CVForSend;
		std::condition_variable m_CVForReceive;
		bool m_StopSenderThread = false;
		bool m_StopReceiverThread = false;
		bool m_IsLastErrMsg = false;
		std::string m_Line;
		uint8_t m_Data[ResponseSize];
		long long m_CurrMsgID = 0;
		long long m_LastMsgID = -1;
	};
}



