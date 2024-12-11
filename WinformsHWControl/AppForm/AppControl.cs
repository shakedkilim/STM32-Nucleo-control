using Interfaces;
using System.Collections.Concurrent;
using System.IO;

namespace Vision
{
    public delegate void ReceiveMsgsQueueNotEmptyDel();
    public class AppControl : IDisposable
    {
        #region Public Methods

        public AppControl()
        {
            HWInterface = new HWManager();
            RequestsQueue = new BlockingCollection<Request>();
            ResponsesQueue = new BlockingCollection<string>();
            _SendMsgsQueueNotEmptyEvent = new AutoResetEvent(false);
            StartSendRequestThread();
            StartReceiveResponseThread();
        }

        public void Dispose()
        {
            StopSendRequestThread = true;
            StopReceiveResponseThread = true;
            _SendMsgsQueueNotEmptyEvent.Set();

            if(HWInterface != null)
                HWInterface.CloseAllThreadsAndDeleteHWInterface();
        }

        public void ClosePortAndStopIOService()
        {
            if (HWInterface != null)
            {
                HWInterface.ClosePortAndStopIOService();
            }
        }

        public void MoveSteps(int iStepper1MoveDelta, int iStepper2MoveDelta)
        {
            int[] Data = { iStepper1MoveDelta, iStepper2MoveDelta };
            Request message = new Request(Device.Motor, Data);
            RequestsQueue.Add(message);
            _SendMsgsQueueNotEmptyEvent.Set();
        }

        public void ChangeLightIntensity(int iChange)
        {
            int[] Data = { iChange };
            Request message = new Request(Device.LED, Data);
            RequestsQueue.Add(message);
            _SendMsgsQueueNotEmptyEvent.Set();
        }

        public BlockingCollection<string>  GetResponsesQueue()
        { 
            return ResponsesQueue;
        }

        #endregion

        #region Public Members
        public event ReceiveMsgsQueueNotEmptyDel? ReceiveMsgsQueueNotEmptyEvent;
        #endregion

        #region Private Methods

        private void StartSendRequestThread()
        {
            _SendMesageBackgroundThread = new Thread(() =>
            {
                while (true)
                {
                    _SendMsgsQueueNotEmptyEvent.WaitOne();
                    if (StopSendRequestThread || HWInterface == null || RequestsQueue.Count == 0)
                        return;
                    Request Request;
                    RequestsQueue.TryTake(out Request, 100);

                    var Parameters = Request.Data;

                    switch (Request.DeviceName)
                    {
                        case Device.Motor:
                            HWInterface.MoveSteps(Parameters[0], Parameters[1]);
                            break;
                        case Device.LED:
                            HWInterface.ChangeLightIntensity(Parameters[0]);
                            break;
                    }
                }
            });
            _SendMesageBackgroundThread.Start();
        }

        private void StartReceiveResponseThread()
        {
            _ResponseMessageBackgroundThread = new Thread(() =>
            {
                while (true)
                {
                    if (HWInterface == null)
                        return;

                    IntPtr Ptr = HWInterface.GetNextResponseMessagePtr();

                    if (Ptr == IntPtr.Zero || StopReceiveResponseThread)
                        return;

                    string? ResponseMessage = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(Ptr);

                    if (ResponseMessage == null)
                        return;

                    ResponsesQueue.Add(ResponseMessage);

                    // Free the allocated memory
                    System.Runtime.InteropServices.Marshal.FreeHGlobal(Ptr);
                    if(ReceiveMsgsQueueNotEmptyEvent != null)
                        ReceiveMsgsQueueNotEmptyEvent.Invoke();
                }

            });
            _ResponseMessageBackgroundThread.Start();
        }

        #endregion

        #region Private Members
        private HWInterface? HWInterface;
        private BlockingCollection<Request> RequestsQueue;
        private BlockingCollection<string> ResponsesQueue;
        private Thread? _SendMesageBackgroundThread;
        private Thread? _ResponseMessageBackgroundThread;
        private AutoResetEvent _SendMsgsQueueNotEmptyEvent;
        bool StopSendRequestThread = false;
        bool StopReceiveResponseThread = false;
        #endregion
    }
}
