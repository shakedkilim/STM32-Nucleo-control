#include "HWManager.h"
#include <crtdbg.h>
Vision::HWManager::HWManager()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	m_pRS232Coms = new COM::RS232();
}

Vision::HWManager::~HWManager()
{
	delete m_pRS232Coms;
}

void Vision::HWManager::ClosePortAndStopIOService()
{
	m_pRS232Coms->ClosePortAndStopIOService();
}

bool Vision::HWManager::MoveSteps(int iStepper1MoveDelta, int iStepper2MoveDelta)
{
	std::unique_ptr<int*> pMsgData = std::make_unique<int*>(new int[BufferSize] { iStepper1MoveDelta, iStepper2MoveDelta });
	if (!pMsgData)
		return false;
	COM::RS232Message msg(COM::Device::Stepper, *pMsgData.get());
	EnqueueMessage(msg);
	return true;
}

bool Vision::HWManager::ChangeLightIntensity(int iChange)
{
	std::unique_ptr<int*> pMsgData = std::make_unique<int*>(new int[BufferSize] { iChange });
	if (!pMsgData)
		return false;
	COM::RS232Message msg(COM::Device::LED, *pMsgData.get());
	EnqueueMessage(msg);
	return true;
}

void Vision::HWManager::CloseAllThreadsAndDeleteHWInterface()
{
	delete this;
}

void Vision::HWManager::EnqueueMessage(COM::RS232Message Msg)
{
	m_pRS232Coms->EnqueueMessage(Msg);
}

IntPtr Vision::HWManager::GetNextResponseMessagePtr()
{
	String^ pMsg = gcnew System::String((m_pRS232Coms->DequeueReceivedLine()).data());

	// Obtain the pointer to the managed string
	IntPtr Ptr = Marshal::StringToHGlobalAnsi(pMsg);

	return Ptr;
}
