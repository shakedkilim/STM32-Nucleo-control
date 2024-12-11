using System.ComponentModel;

namespace Vision
{
    public partial class AppForm : Form
    {
        #region Public Methods

        public AppForm()
        {
            InitializeComponent();

            try
            {
                _AppControl = new AppControl();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"{ex.Message}\n\nCheck hardware connection.", "Error");
            }

            _SendRequestStepper1BackgroundWorker = new BackgroundWorker();
            _SendRequestStepper1BackgroundWorker.DoWork += SendRequestStepper1BackgroundWorker_DoWork;
            _SendRequestStepper1BackgroundWorker.RunWorkerCompleted += SendRequestStepper1BackgroundWorker_RunWorkerCompleted;

            _SendRequestStepper2BackgroundWorker = new BackgroundWorker();
            _SendRequestStepper2BackgroundWorker.DoWork += SendRequestStepper2BackgroundWorker_DoWork;
            _SendRequestStepper2BackgroundWorker.RunWorkerCompleted += SendRequestStepper2BackgroundWorker_RunWorkerCompleted;

            _SendRequestChangeLightBrightnessBackgroundWorker = new BackgroundWorker();
            _SendRequestChangeLightBrightnessBackgroundWorker.DoWork += SendRequestChangeLightBrightnessBackgroundWorker_DoWork;
            _SendRequestChangeLightBrightnessBackgroundWorker.RunWorkerCompleted += SendRequestChangeLightBrightnessBackgroundWorker_RunWorkerCompleted;

            if (_AppControl != null)
                _AppControl.ReceiveMsgsQueueNotEmptyEvent += new ReceiveMsgsQueueNotEmptyDel(StartDequeueResponseAndShowOnFormThread);
        }

        public void StartDequeueResponseAndShowOnFormThread()
        {
            _ReceiveResposneBackgroundThread = new Thread(() =>
            {
                string Response = DequeueResponse();
                ShowResponseOnForm(Response);
            });
            _ReceiveResposneBackgroundThread.Start();
        }

        private void DisableStepper1Buttons()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => DisableStepper1Buttons()));
            }
            else
            {
                ButtonLeftStepper1.Enabled = false;
                ButtonRightStepper1.Enabled = false;
            }
        }

        private void EnableStepper1Buttons()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => EnableStepper1Buttons()));
            }
            else
            {
                ButtonLeftStepper1.Enabled = true;
                ButtonRightStepper1.Enabled = true;
            }
        }

        private void DisableStepper2Buttons()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => DisableStepper2Buttons()));
            }
            else
            {
                ButtonLeftStepper2.Enabled = false;
                ButtonRightStepper2.Enabled = false;
            }
        }

        private void EnableStepper2Buttons()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => EnableStepper2Buttons()));
            }
            else
            {
                ButtonLeftStepper2.Enabled = true;
                ButtonRightStepper2.Enabled = true;
            }
        }

        private void EnablePlusAndMinus()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => EnablePlusAndMinus()));
            }
            else
            {
                ButtonPlus.Enabled = true;
                ButtonMinus.Enabled = true;
            }
        }

        private void DisablePlusAndMinus()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => DisablePlusAndMinus()));
            }
            else
            {
                ButtonPlus.Enabled = false;
                ButtonMinus.Enabled = false;
            }
        }

        private void SendRequestStepper1BackgroundWorker_DoWork(object? sender, DoWorkEventArgs e)
        {
            var Parameters = e.Argument as SteppersMovement;
            if (_AppControl == null || Parameters == null)
                return;
            _AppControl.MoveSteps(Parameters.Stepper1MoveDelta, Parameters.Stepper2MoveDelta);
        }

        private void SendRequestStepper1BackgroundWorker_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            EnableStepper1Buttons();
        }

        private void SendRequestStepper2BackgroundWorker_DoWork(object? sender, DoWorkEventArgs e)
        {
            var Parameters = e.Argument as SteppersMovement;
            if (Parameters == null || _AppControl == null)
                return;
            _AppControl.MoveSteps(Parameters.Stepper1MoveDelta, Parameters.Stepper2MoveDelta);
        }

        private void SendRequestStepper2BackgroundWorker_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            EnableStepper2Buttons();
        }

        private void SendRequestChangeLightBrightnessBackgroundWorker_DoWork(object? sender, DoWorkEventArgs e)
        {
            if (_AppControl == null)
                return;
            int LightIntesityChange = 0;
            if (e.Argument != null)
                LightIntesityChange = (int)e.Argument;
            _AppControl.ChangeLightIntensity(LightIntesityChange);
        }

        private void ShowResponseOnForm(string Response)
        {
            if (Response != null && Response != "")
            {
                if (InvokeRequired)
                {
                    BeginInvoke(new Action(() => ShowResponseOnForm(Response)));
                }
                else
                {
                    ResponseslistBox.Items.Add(Response);
                    ResponseslistBox.SelectedIndex = ResponseslistBox.Items.Count - 1;
                    ResponseslistBox.Show();
                }
            }
        }

        private string DequeueResponse()
        {
            if (_AppControl == null)
                return "";
            string? ResponseMessage = null;
            _AppControl.GetResponsesQueue().TryTake(out ResponseMessage, 100);

            if (ResponseMessage == null)
                return "";
            return ResponseMessage;
        }

        private void SendRequestChangeLightBrightnessBackgroundWorker_RunWorkerCompleted(object? sender, RunWorkerCompletedEventArgs e)
        {
            EnablePlusAndMinus();
        }

        private void ButtonRightStepper1_Click(object sender, EventArgs e)
        {
            var Parameters = new SteppersMovement(-1, 0);
            DisableStepper1Buttons();
            _SendRequestStepper1BackgroundWorker.RunWorkerAsync(Parameters);
        }

        private void ButtonRightStepper2_Click(object sender, EventArgs e)
        {
            var Parameters = new SteppersMovement(0, -1);
            DisableStepper2Buttons();
            _SendRequestStepper2BackgroundWorker.RunWorkerAsync(Parameters);
        }

        private void ButtonLeftStepper1_Click(object sender, EventArgs e)
        {
            var Parameters = new SteppersMovement(1, 0);
            DisableStepper1Buttons();
            _SendRequestStepper1BackgroundWorker.RunWorkerAsync(Parameters);
        }

        private void ButtonLeftStepper2_Click(object sender, EventArgs e)
        {
            var Parameters = new SteppersMovement(0, 1);
            DisableStepper2Buttons();
            _SendRequestStepper2BackgroundWorker.RunWorkerAsync(Parameters);
        }

        private void ButtonPlus_Click(object sender, EventArgs e)
        {
            DisablePlusAndMinus();
            _SendRequestChangeLightBrightnessBackgroundWorker.RunWorkerAsync(1);
        }

        private void ButtonMinus_Click(object sender, EventArgs e)
        {
            DisablePlusAndMinus();
            _SendRequestChangeLightBrightnessBackgroundWorker.RunWorkerAsync(-1);
        }

        private void AppForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            _SendRequestStepper1BackgroundWorker.Dispose();
            _SendRequestChangeLightBrightnessBackgroundWorker.Dispose();
            _SendRequestStepper2BackgroundWorker.Dispose();

            if (_AppControl != null)
            {
                _AppControl.ReceiveMsgsQueueNotEmptyEvent -= StartDequeueResponseAndShowOnFormThread;
                _AppControl.ClosePortAndStopIOService();
                _AppControl.Dispose();
            }

            _ReceiveResposneBackgroundThread = null;
        }

        #endregion

        #region Private Members

        private AppControl? _AppControl;
        private readonly BackgroundWorker _SendRequestStepper1BackgroundWorker;
        private readonly BackgroundWorker _SendRequestStepper2BackgroundWorker;
        private readonly BackgroundWorker _SendRequestChangeLightBrightnessBackgroundWorker;
        private Thread? _ReceiveResposneBackgroundThread;

#endregion
    }

    class SteppersMovement
    {
        public SteppersMovement(int iStepper1MoveDelta, int iStepper2MoveDelta)
        {
            Stepper1MoveDelta = iStepper1MoveDelta;
            Stepper2MoveDelta = iStepper2MoveDelta;
        }
        public int Stepper1MoveDelta { get; set; }
        public int Stepper2MoveDelta { get; set; }
    }
}
