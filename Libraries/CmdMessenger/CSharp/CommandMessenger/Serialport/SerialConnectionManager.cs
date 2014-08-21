using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;
using System.Timers;
using System.Windows.Forms;
using CommandMessenger.ConnectionManager;
using CommandMessenger.TransportLayer;
using Timer = System.Timers.Timer;

namespace CommandMessenger.Serialport
{
    [Serializable()]
    public class LastConnectedSetting 
    {
        public String Port{ get; set; }
        public int BaudRate { get; set; }
    }

    public class SerialConnectionManager :  IConnectionManager , IDisposable 
    {
        const string SettingsFileName = @"LastConnectedSerialSetting.cfg";
        private LastConnectedSetting _lastConnectedSetting;
        private readonly CmdMessenger _cmdMessenger;
        private readonly SerialTransport _serialTransport;
        private long _watchdogTimeOut;
        private readonly Timer _watchdogTimer;
        bool watchDogRunning;
        private int _scanType = 0;

        private bool _activeConnection;
        public event EventHandler ConnectionTimeout;
        public event EventHandler ConnectionFound;
        public event EventHandler<ConnectionManagerProgressEventArgs> Progress;
        private readonly BackgroundWorker _scanThread;
        private readonly int _challengeCommandId;
        private readonly int _responseCommandId;

        public bool Connected { get; set; }

        // The control to invoke the callback on
        private Control _controlToInvokeOn;
        private readonly object _tryConnectionLock = new object();

        public SerialConnectionManager(SerialTransport serialTransport, CmdMessenger cmdMessenger, int challengeCommandId, int responseCommandId)
        {
            if (serialTransport == null) return;
            if (cmdMessenger    == null) return;

            _controlToInvokeOn = null;
            _serialTransport = serialTransport;
            _cmdMessenger = cmdMessenger;
            _scanThread = new BackgroundWorker {WorkerSupportsCancellation = true, WorkerReportsProgress = false};
            _scanThread.DoWork += ScanThreadDoWork;
            _scanThread.RunWorkerCompleted += ScanThreadRunWorkerCompleted;
            _challengeCommandId = challengeCommandId;
            _responseCommandId = responseCommandId;

            _lastConnectedSetting = new LastConnectedSetting();
            ReadSettings();
            _cmdMessenger.Attach((int)responseCommandId, OnResponseCommandId);
            _serialTransport.UpdatePortCollection();

            _watchdogTimer = new Timer(2000); 
            _watchdogTimer.Elapsed += TimerElapsed;

        }

        /// <summary> Sets a control to invoke on. </summary>
        /// <param name="controlToInvokeOn"> The control to invoke on. </param>
        public void SetControlToInvokeOn(Control controlToInvokeOn)
        {
            _controlToInvokeOn = controlToInvokeOn;
        }

        private void InvokeEvent(EventHandler eventHandler)
        {
            try
            {
                if (eventHandler == null) return;
                if (_controlToInvokeOn != null && _controlToInvokeOn.InvokeRequired)
                {
                    //Asynchronously call on UI thread
                    _controlToInvokeOn.Invoke((MethodInvoker)(() => eventHandler(this, null)));
                    Thread.Yield();
                }
                else
                {
                    //Directly call
                    eventHandler(this, null);
                }
            }
            catch (Exception)
            {
            }
        }

        private void InvokeEvent<TEventHandlerArguments>(EventHandler<TEventHandlerArguments> eventHandler, TEventHandlerArguments eventHandlerArguments) where TEventHandlerArguments : EventArgs 
        {
            try
            {
                if (eventHandler == null) return;
                if (_controlToInvokeOn != null && _controlToInvokeOn.InvokeRequired)
                {
                    //Asynchronously call on UI thread
                    _controlToInvokeOn.Invoke((MethodInvoker)(() => eventHandler(this, eventHandlerArguments)));
                    Thread.Yield();
                }
                else
                {
                    //Directly call
                    eventHandler(this, eventHandlerArguments);
                }
            }
            catch (Exception)
            {
            }
        }

        private void Log(int level, string logMessage) 
        {
            var args = new ConnectionManagerProgressEventArgs {Level = level, Description = logMessage};
            InvokeEvent(Progress, args);
        }

        private void OnResponseCommandId(ReceivedCommand arguments)
        {            
            // Do nothing
        }

        public void StartScan()
        {
            if (_scanThread.IsBusy != true)
            {
                // Start the asynchronous operation.
                _scanThread.RunWorkerAsync();
            }   
        }

        public void StopScan()
        {
            if (_scanThread.WorkerSupportsCancellation)
            {
                // Cancel the asynchronous operation.
                _scanThread.CancelAsync();
            }
        }

        private void ScanThreadRunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            //if (ConnectionFound != null) ConnectionFound.Invoke(this, null);
        }

        private void ScanThreadDoWork(object sender, DoWorkEventArgs e)
        {
            if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager scan";
            var worker = sender as BackgroundWorker;
            _activeConnection = false;
            
            while (!_activeConnection)
            {
                if (worker != null && worker.CancellationPending)
                {
                    e.Cancel = true;
                    break;
                }
                if (_scanType == 0)
                {
                    _scanType = 1;
                    try { _activeConnection = QuickScan(); } catch{}
                }                
                else if (_scanType == 1)
                {
                    _scanType = 0;
                    try { _activeConnection = ThoroughScan(); } catch { }                  
                }
            }

            // Trigger event when a connection was made
            if (_activeConnection && ConnectionFound != null) InvokeEvent(ConnectionFound); // ConnectionFound.Invoke(this, null);
        }

        public bool TryConnection(string portName, int baudRate, int timeOut)
        {
            // Try specific port name & baud rate

            _serialTransport.CurrentSerialSettings.PortName = portName;
            _serialTransport.CurrentSerialSettings.BaudRate = baudRate;
            return TryConnection(timeOut); 
        }

        public bool TryConnection(int timeOut)
        {
            lock(_tryConnectionLock)
            Connected = false;
            Log(1, @"Trying serial port " + _serialTransport.CurrentSerialSettings.PortName + @" baud rate " + _serialTransport.CurrentSerialSettings.BaudRate);
            if (_serialTransport.Connect())
            {
                Connected = (ArduinoAvailable(timeOut));
                
                if (Connected)
                {
                    Log(1, "Connected at serial port " + _serialTransport.CurrentSerialSettings.PortName + @" baud rate " + _serialTransport.CurrentSerialSettings.BaudRate);
                    StoreSettings();
                }
                return Connected;
            }
            return false;
        }

        public bool ArduinoAvailable(int timeOut)
        {
            Log(3, "Polling Arduino");
            var challengeCommand = new SendCommand(_challengeCommandId, _responseCommandId, timeOut);
            var responseCommand = _cmdMessenger.SendCommand(challengeCommand);
            return responseCommand.Ok;
        }

        public bool Disconnect()
        {            
            Connected = false;
            return _cmdMessenger.Disconnect();
        }

        // Single scan on foreground thread
        public bool SingleScan()
        {
            if (QuickScan()) return true;
            if (ThoroughScan()) return true;
            return false;
        }        

        public bool QuickScan()
        {            
            Log(3, "Performing quick scan");
            const int longTimeOut =  500;
            const int shortTimeOut = 200;

            // First try if currentConnection is open or can be opened
            if (TryConnection(longTimeOut)) return true;

            // Then try if last stored connection can be opened
            Log(3, "Trying last stored connection");
            if (TryConnection(_lastConnectedSetting.Port, _lastConnectedSetting.BaudRate, longTimeOut)) return true;

            // Then see if port list has changed
            if (NewPortInList().Count > 0) { _scanType = 2; return false; }

            // Quickly run through most used ports
            int[] commonBaudRates =
                {
                    115200, // Arduino Uno, Mega, with AT8u2 USB
                    57600,  // Arduino Duemilanove, FTDI Serial
                    9600    // Often used as default, but slow!
                };
            _serialTransport.UpdatePortCollection();
            for (var port = _serialTransport.CurrentSerialSettings.PortNameCollection.Length- 1; port >= 0; port--)
            {
                // If port list has changed, interrupt scan and test new ports first
                if (NewPortScan()) return true;

                var portName = _serialTransport.CurrentSerialSettings.PortNameCollection[port];
                // First set port name
                _serialTransport.CurrentSerialSettings.PortName = portName;
                // Now update BaudRate Collection
                _serialTransport.UpdateBaudRateCollection();
                var baudRateCollection =_serialTransport.CurrentSerialSettings.BaudRateCollection;

                Log(1, 
                    "Trying Port" + portName + ", possible speeds " +
                    baudRateCollection.Count + " " +
                    (baudRateCollection.Count > commonBaudRates.Length ? ", trying " + commonBaudRates.Length : "")
                    );

                //  Now loop through baud rate collection
                foreach (var commonBaudRate in commonBaudRates)
                {

                    if (_serialTransport.CurrentSerialSettings.BaudRateCollection.Contains(commonBaudRate))
                    {                        
                        if (TryConnection(portName,commonBaudRate, shortTimeOut)) return true;
                        Thread.Sleep(25); 
                    }                    
                }
            }
            return false;
        }

        public bool ThoroughScan()
        {
            Console.WriteLine("Performing thorough scan");
            Log(1, "Performing thorough scan");
            // First try last used connection
            const int longTimeOut = 2000;
            const int shortTimeOut = 100;

            // First try if currentConnection is open or can be opened
            if (TryConnection(longTimeOut)) return true;

            // Then try if last stored connection can be opened
            if (TryConnection(_lastConnectedSetting.Port, _lastConnectedSetting.BaudRate, longTimeOut)) return true;

            // If port list has changed, interrupt scan and test new ports first
            if (NewPortScan()) return true;

            // Slowly walk through 
            _serialTransport.UpdatePortCollection();
            foreach (var portName in _serialTransport.CurrentSerialSettings.PortNameCollection)
            {
                // First set port name
                _serialTransport.CurrentSerialSettings.PortName = portName;
                // update BaudRate Collection
                _serialTransport.UpdateBaudRateCollection();
                //  Now loop through baud rate collection
                var baudRateCollection = _serialTransport.CurrentSerialSettings.BaudRateCollection;

                Log(1, "Trying Port" + portName + ", possible speeds " + baudRateCollection.Count);

                foreach (var baudRate in baudRateCollection)
                {
                    // Then see if port list has changed
                    if (NewPortInList().Count > 0) { _scanType = 2; return false; }
                    {
                        if (TryConnection(portName,baudRate, shortTimeOut))
                            return true;
                        Thread.Sleep(100); 
                    }
                }
            }
            return false;
        }

        public bool NewPortScan()
        {            
            const int shortTimeOut = 200;

            // Then see if port list has changed
            var newPorts = NewPortInList();
            if (newPorts.Count == 0) { return false; }

            Console.WriteLine("Trying new ports");
            Log(1, "Trying new ports");

            // Quickly run through most used ports
            int[] commonBaudRates =
                {
                    115200, // Arduino Uno, Mega, with AT8u2 USB
                    57600,  // Arduino Duemilanove, FTDI Serial
                    9600    // Often used as default, but slow!
                };
            _serialTransport.UpdatePortCollection();
            foreach (var portName in newPorts)
            {
                // First set port name
                _serialTransport.CurrentSerialSettings.PortName = portName;
                // Now update BaudRate Collection
                _serialTransport.UpdateBaudRateCollection();
                //  Now loop through baud rate collection
                var allBaudRates = _serialTransport.CurrentSerialSettings.BaudRateCollection;
                // First add commonBaudRates available
                var sortedBaudRates = commonBaudRates.Where(allBaudRates.Contains).ToList();
                // Then add other BaudRates 
                sortedBaudRates.AddRange(allBaudRates.Where(baudRate => !commonBaudRates.Contains(baudRate)));              

                foreach (var currentBaudRate in sortedBaudRates)
                {
                        if (TryConnection(portName, currentBaudRate, shortTimeOut)) return true;
                        Thread.Sleep(100);
                }
            }
            return false;
        }


        private List<string> NewPortInList()
        {
            var oldPortCollection = _serialTransport.CurrentSerialSettings.PortNameCollection;
            var portCollection    = SerialPort.GetPortNames();
            return portCollection.Where(port => !oldPortCollection.Any(port.Contains)).ToList();
        }

        private void StoreSettings()
        {
            _lastConnectedSetting.Port = _serialTransport.CurrentSerialSettings.PortName;
            _lastConnectedSetting.BaudRate = _serialTransport.CurrentSerialSettings.BaudRate;

            var fileStream = File.Create(SettingsFileName);
            var serializer = new BinaryFormatter();
            serializer.Serialize(fileStream,_lastConnectedSetting);
            fileStream.Close();
        }

        private void ReadSettings()
        {
            // Read from file

            _lastConnectedSetting.Port = "COM1";
            _lastConnectedSetting.BaudRate = 115200;
            if (File.Exists(SettingsFileName))
            {
                var fileStream = File.OpenRead(SettingsFileName);
                var deserializer = new BinaryFormatter();
                _lastConnectedSetting = (LastConnectedSetting)deserializer.Deserialize(fileStream);
                fileStream.Close();
            }
        }

        public void StartWatchDog(long watchdogTimeOut)
        {
            watchDogRunning = true;
            _watchdogTimeOut = watchdogTimeOut;
            _watchdogTimer.Interval = _watchdogTimeOut;
            _watchdogTimer.Start();
            Thread.Yield();    
        }

        public void StopWatchDog()
        {
            watchDogRunning = false;
            _watchdogTimer.Stop();
            Thread.Yield();             
        }

        private void TimerElapsed(object sender, ElapsedEventArgs e)
        {
            try { ConnectionWatchDog(); } catch {}
        }

        private void ConnectionWatchDog()
        {
            if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager watchdog";
            // Check how long ago last command was received
            var lastLineTimeStamp = _cmdMessenger.LastReceivedCommandTimeStamp;

            if ((TimeUtils.Millis - lastLineTimeStamp) >= 2*_watchdogTimeOut)
            {
                if (ConnectionTimeout != null && watchDogRunning)
                {                    
                    {
                        StopWatchDog();
                        InvokeEvent(ConnectionTimeout); 
                    }
                }
            } else if ((TimeUtils.Millis - lastLineTimeStamp) >= _watchdogTimeOut)
            {
                // We queue the command in order to not be intrusive
                _cmdMessenger.QueueCommand(new SendCommand(_challengeCommandId));
                // We do not need to more, since a response will update LastReceivedCommandTimeStamp
            }
        }

        // Dispose 
        public void Dispose()
        {
            Dispose(true);
        }

        // Dispose
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                StopScan();
                StopWatchDog();
                _scanThread.DoWork -= ScanThreadDoWork;
                _scanThread.RunWorkerCompleted -= ScanThreadRunWorkerCompleted;
                _watchdogTimer.Elapsed -= TimerElapsed;
                _watchdogTimer.Close();
            }
        }

    }
}
