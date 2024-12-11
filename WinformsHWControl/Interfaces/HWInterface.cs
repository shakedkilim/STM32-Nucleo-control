namespace Interfaces
{
    public enum Device { Motor, LED };
    public struct Request
    {
        public Device DeviceName;
        public int[] Data;

        public Request(Device stDeviceName, int[] pData)
        {
            DeviceName = stDeviceName;
            Data = pData;
        }
    };

    public interface HWInterface
    {
        abstract public bool MoveSteps(int iStepper1MoveDelta, int iStepper2MoveDelta);
        abstract public bool ChangeLightIntensity(int iChange);
        abstract public IntPtr GetNextResponseMessagePtr();
        abstract public void ClosePortAndStopIOService();
        abstract public void CloseAllThreadsAndDeleteHWInterface();
    }
}
