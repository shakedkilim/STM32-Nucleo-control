#pragma once
#include "../RS232/RS232.h"

using namespace Interfaces;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Vision
{
	constexpr unsigned int BufferSize = 2;

	public ref class HWManager : HWInterface
	{
	public:
		HWManager();
		~HWManager();
		virtual void ClosePortAndStopIOService();
		virtual bool MoveSteps(int iStepper1MoveDelta, int iStepper2MoveDelta);
		virtual bool ChangeLightIntensity(int iChange);
		virtual void CloseAllThreadsAndDeleteHWInterface();
		virtual IntPtr GetNextResponseMessagePtr();
		void EnqueueMessage(COM::RS232Message Msg);
	private:
		COM::RS232* m_pRS232Coms;
	};
}
