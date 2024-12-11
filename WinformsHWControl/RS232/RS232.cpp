#include "RS232.h"
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

using namespace boost::asio;

unsigned int COM::RS232Message::MessageNum = 0;

std::string m_Port = "COM4";

boost::asio::io_service m_IOService;
boost::asio::serial_port m_SerialPort(m_IOService, m_Port); // Actual port

COM::RS232::RS232()
{
	m_RS232SenderThread = std::thread(&RS232::DequeueAndSendMessage, this);
	m_RS232ReceiverThread = std::thread(&RS232::RecieveMessageAndEnqueue, this);
}

COM::RS232::~RS232()
{
	// Ensure proper thread termination
	std::lock_guard<std::mutex> lock(m_MsgsToSendMutex);
	m_StopSenderThread = true;
	m_StopReceiverThread = true;
	m_CVForSend.notify_one();
	m_CVForReceive.notify_one();
	m_RS232SenderThread.detach();
	m_RS232ReceiverThread.detach();
}

void COM::RS232::EnqueueMessage(RS232Message msg)
{
	std::lock_guard<std::mutex> Lock(m_MsgsToSendMutex);
	m_MessagesToSendQueue.push(msg);
	m_CVForSend.notify_one();
}

void COM::RS232::DequeueAndSendMessage()
{
	while (!m_StopSenderThread) {
		std::unique_lock<std::mutex> lock(m_MsgsToSendMutex);
		m_CVForSend.wait(lock, [this] { return !m_MessagesToSendQueue.empty() || m_StopSenderThread; });
		while (!m_MessagesToSendQueue.empty())
		{
			RS232Message msg = m_MessagesToSendQueue.front();
			m_MessagesToSendQueue.pop();
			Send232Message(msg);
			m_CVForSend.notify_one();
		}
	}
}

void COM::RS232::RecieveMessageAndEnqueue()
{
	// Set serial port parameters
	m_SerialPort.set_option(serial_port::baud_rate(115200));
	m_SerialPort.set_option(serial_port::character_size(8));
	m_SerialPort.set_option(serial_port::parity(serial_port::parity::none));
	m_SerialPort.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	m_SerialPort.set_option(serial_port::flow_control(serial_port::flow_control::none));

	// Start reading asynchronously
	ReadData();
}

void COM::RS232::ReadData()
{
	boost::asio::async_read(m_SerialPort, boost::asio::buffer(m_Data, ResponseSize),
		[this](const boost::system::error_code& error, std::size_t n) {
			if (!error)
			{
				BufferReceived();
			}
			else {
				std::cerr << "Error reading data: " << error.message() << std::endl;
				std::wstring Message = (LPCWSTR)L"Data reception did not succeed.\n" +
					std::wstring(error.message().begin(), error.message().end());
				ShowMessageBox(Message, "Connection Error");
			}
			if (!m_StopReceiverThread)
				ReadData();
		});
	if (!m_StopReceiverThread)
		m_IOService.run_one();
}

void COM::RS232::ShowMessageBox(std::wstring& Message, std::string Title)
{
	// Convert std::string to std::wstring
	std::wstring WStrTitle = std::wstring(Title.begin(), Title.end());

	LPCWSTR LPCWSTRMessage = Message.c_str();

	MessageBox(
		NULL,
		LPCWSTRMessage,
		WStrTitle.c_str(),
		MB_ICONWARNING | MB_APPLMODAL | MB_DEFBUTTON2
	);
}

void COM::RS232::BufferReceived()
{
	std::string Msg = (char*)&m_Data;

	if (IsErrorMsg(Msg) && m_IsLastErrMsg)
		return;

	std::string ParsedMsg = ParseMessage(Msg);

	if (ParsedMsg == "")
		return;

	std::lock_guard<std::mutex> lock(m_MsgsToRecieveMutex);
	m_MessagesToReceiveQueue.push(ParsedMsg);

	if (IsErrorMsg(Msg))
	{
		m_IsLastErrMsg = true;
	}
	else
	{
		m_LastMsgID = m_CurrMsgID;
		m_IsLastErrMsg = false;
	}
	
	m_CVForReceive.notify_one();
}

long long COM::RS232::GetMsgID(std::string Msg)
{
	size_t Index = Msg.find("ID:");

	if (Index == std::string::npos)
		return -1;

	std::string StrID = Msg.substr(Index + 3, Msg.length() - Index - 3);
	std::stringstream StrStream(StrID);
	long long Res;
	StrStream >> Res;
	return Res;
}

bool COM::RS232::IsSameMsgIDAsLast()
{
	return m_CurrMsgID == m_LastMsgID;
}

bool COM::RS232::IsErrorMsg(std::string Msg)
{
	return Msg.find("Error") != std::string::npos;
}

std::string COM::RS232::ParseMessage(std::string Msg)
{
	if (Msg == "")
		return "";
		
	m_CurrMsgID = GetMsgID(Msg);

	if(IsSameMsgIDAsLast())
		return "";
	return Msg.substr(0, Msg.find("\n"));
}

std::string COM::RS232::DequeueReceivedLine()
{
	std::unique_lock<std::mutex> lock(m_MsgsToRecieveMutex);
	m_CVForReceive.wait(lock, [this] { return !m_MessagesToReceiveQueue.empty(); });
	std::string Output = m_MessagesToReceiveQueue.front();
	m_MessagesToReceiveQueue.pop();
	return Output;
}

void COM::RS232::Send232Message(RS232Message Message)
{
	// Set serial port parameters

	m_SerialPort.set_option(serial_port_base::baud_rate(115200));
	m_SerialPort.set_option(serial_port_base::character_size(8));
	m_SerialPort.set_option(serial_port_base::parity(serial_port_base::parity::none));

	m_SerialPort.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
	m_SerialPort.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

	// Convert the message into a byte buffer
	RS232MessageToSend MsgToSend = (RS232MessageToSend)Message;

	const int Size = sizeof(RS232MessageToSend);
	const uint8_t* pBuffer = (uint8_t*)&MsgToSend;

	// Write the buffer to the serial port
	boost::system::error_code error;
	write(m_SerialPort, buffer(pBuffer, Size), error);

	if (error) {
		std::cerr << "Error writing to serial port: " << error.message() << std::endl;
		std::wstring Message = (LPCWSTR)L"Data transmission did not succeed.\n" +
			std::wstring(error.message().begin(), error.message().end());
		ShowMessageBox(Message, "Connection Error");
	}
}

void COM::RS232::ClosePortAndStopIOService()
{
	m_IOService.stop();
	m_SerialPort.close();
}
