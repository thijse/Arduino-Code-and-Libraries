using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;
using System.Windows.Forms;
using CommandMessenger.ConnectionManager;
using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;

namespace CommandMessenger.Bluetooth
{

    public enum ConnectionManagerStates
    {
        Scan,
        Watchdog,
        Wait,
        Stop
    }

    [Serializable]
    public class BluetoothConfiguration 
    {
        public BluetoothAddress BluetoothAddress { get; set; }
        public Dictionary<BluetoothAddress, string> StoredDevicePins { get; set; }
    }

    public class BluetoothConnectionManager : IConnectionManager
    {
        private static readonly List<string> CommonDevicePins = new List<string>
            {
                "0000",
                "1111",
                "1234"
            };

        const string SettingsFileName = @"LastConnectedBluetoothSetting.cfg";
        private BluetoothConfiguration _bluetoothConfiguration;
        private readonly CmdMessenger _cmdMessenger;
        private readonly BluetoothTransport _bluetoothTransport;
        private const long WatchdogTimeOut = 2000;
        private const long WatchdogRetryTimeOut = 1000;
        private long _lastCheckTime;
        private const int MaxWatchdogTries = 3;
        private int _watchdogTries;
        private int _scanType;
        private ConnectionManagerStates _connectionManagerState;
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
        private readonly List<BluetoothDeviceInfo> _deviceList;
        private List<BluetoothDeviceInfo> _prevDeviceList;
        private long _nextTimeOutCheck;

        public BluetoothConnectionManager(BluetoothTransport bluetoothTransport, CmdMessenger cmdMessenger, int challengeCommandId, int responseCommandId)
        {
            if (bluetoothTransport == null) return;
            if (cmdMessenger       == null) return;

            _controlToInvokeOn = null;
            _bluetoothTransport = bluetoothTransport;
            _cmdMessenger = cmdMessenger;
            _scanThread = new BackgroundWorker {WorkerSupportsCancellation = true, WorkerReportsProgress = false};
            _scanThread.DoWork += ScanThreadDoWork;
            _challengeCommandId = challengeCommandId;
            _responseCommandId = responseCommandId;

            _bluetoothConfiguration = new BluetoothConfiguration();
            ReadSettings();
            _cmdMessenger.Attach((int)responseCommandId, OnResponseCommandId);

            _deviceList = new List<BluetoothDeviceInfo>();
            _prevDeviceList = new List<BluetoothDeviceInfo>();

            StartConnectionManager();
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
                    _controlToInvokeOn.BeginInvoke((MethodInvoker)(() => eventHandler(this, null)));
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
                if (_controlToInvokeOn.IsDisposed) return;
                if (_controlToInvokeOn != null && _controlToInvokeOn.InvokeRequired)
                {
                    //Asynchronously call on UI thread
                    _controlToInvokeOn.BeginInvoke((MethodInvoker)(() => eventHandler(this, eventHandlerArguments)));
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

        public void StartConnectionManager()
        {
            _connectionManagerState = ConnectionManagerStates.Wait;
            if (_scanThread.IsBusy != true)
            {
                // Start the asynchronous operation.
                _scanType = 0;
                _scanThread.RunWorkerAsync();
            }   
        }

        public void StopConnectionManager()
        {
            _connectionManagerState = ConnectionManagerStates.Stop;
            
            if (_scanThread.WorkerSupportsCancellation)
            {
                // Cancel the asynchronous operation.
                _scanThread.CancelAsync();
            }
            _scanThread.DoWork -= ScanThreadDoWork;
        }

        private void ScanThreadDoWork(object sender, DoWorkEventArgs e)
        {
            
            while (_connectionManagerState != ConnectionManagerStates.Stop)
            {
                // Check if thread is being canceled
                var worker = sender as BackgroundWorker;
                if (worker != null && worker.CancellationPending)
                {
                    _connectionManagerState = ConnectionManagerStates.Stop;
                    break;
                }

                switch (_connectionManagerState)
                {
                    case ConnectionManagerStates.Scan:
                        //Console.WriteLine("BluetoothConnectionManager - scan");
                        DoWorkScan();
                        break;
                    case ConnectionManagerStates.Watchdog:
                        //Console.WriteLine("BluetoothConnectionManager - watchdog");
                        DoWorkWatchdog();
                        break;
                    default:
                        Thread.Sleep(1000);
                        break;
                }
            }
        }

        private void DoWorkScan()
        {
            if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager - scan";
            _activeConnection = false;

            //if (!_activeConnection)
            {

                if (_scanType == 0)
                {
                    _scanType = 1;
                    try { _activeConnection = QuickScan(); } catch { }
                }
                else if (_scanType == 1)
                {
                    _scanType = 0;
                    try { _activeConnection = QuickScan(); } catch { }
                }
            }

            // Trigger event when a connection was made
            if (_activeConnection)
            {
                _connectionManagerState = ConnectionManagerStates.Wait;
                InvokeEvent(ConnectionFound);                
            } 
        }

        private void DoWorkWatchdog()
        {
            if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager - watchdog";
            try { ConnectionWatchDog(); } catch { }
            Thread.Sleep(100);
        }

        private void QuickScanDevices()
        {
            // Fast
            _prevDeviceList = _deviceList;
            _deviceList.Clear();
            _deviceList.AddRange(_bluetoothTransport.BluetoothClient.DiscoverDevices(255, true, true, false, false));
        }

        public void ThorougScanForDevices()
        {
            // Slow
            _deviceList.Clear();
            _deviceList.AddRange(_bluetoothTransport.BluetoothClient.DiscoverDevices(65536, true, true, true, true));
        }

        public bool PairDevice(BluetoothDeviceInfo device)
        {
            //device.Update();
            if (device.Authenticated) return true;
            // Check if PIN has been stored
            if (_bluetoothConfiguration.StoredDevicePins.ContainsKey(device.DeviceAddress))
            {
                if (BluetoothSecurity.PairRequest(device.DeviceAddress, _bluetoothConfiguration.StoredDevicePins[device.DeviceAddress]))
                {
                    device.Update();
                    return device.Authenticated;
                }                    
            }            

            // loop through common PIN numbers to see if they pair
            foreach (string devicePin in CommonDevicePins)
            {
                var isPaired = BluetoothSecurity.PairRequest(device.DeviceAddress, devicePin);
                if (isPaired)
                {
                    _bluetoothConfiguration.StoredDevicePins[device.DeviceAddress] = devicePin;
                    StoreSettings();
                    break;
                }
            }

            device.Update();
            return device.Authenticated;
        }

        public bool TryConnection(BluetoothAddress bluetoothAddress,  int timeOut)
        {
            if (bluetoothAddress == null) return false;
            // Find
            foreach (var bluetoothDeviceInfo in _deviceList)
            {
                if (bluetoothDeviceInfo.DeviceAddress == bluetoothAddress)
                {
                    return TryConnection(bluetoothDeviceInfo, timeOut);
                }
            }
            return false;
        }

        public bool TryConnection(BluetoothDeviceInfo bluetoothDeviceInfo, int timeOut)
        {
            // Try specific settings
            _bluetoothTransport.CurrentBluetoothDeviceInfo = bluetoothDeviceInfo;
            return TryConnection(timeOut); 
        }

        public bool TryConnection(int timeOut)
        {
            lock (_tryConnectionLock)
            {
                // Check if an (old) connection exists
                if (_bluetoothTransport.CurrentBluetoothDeviceInfo == null) return false;

                Connected = false;
                Log(1, @"Trying Bluetooth device " + _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName);
                if (_bluetoothTransport.Connect())
                {
                    Log(3,
                        @"Connected with Bluetooth device " + _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName +
                        ", requesting response");
                    Connected = (ArduinoAvailable(timeOut, 5));

                    if (Connected)
                    {
                        Log(1,
                            "Connected with Bluetooth device " +
                            _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName);
                        StoreSettings();
                    }
                    else
                    {
                        Log(3,
                            @"Connected with Bluetooth device " +
                            _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName + ", received no response");
                    }
                    return Connected;
                }
                else
                {
                    Log(3,
                    @"No connection made with Bluetooth device " + _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName );
                }
                return false;
            }
        }

        public bool ArduinoAvailable(int timeOut)
        {
            //Log(3, "Polling Arduino");
            var challengeCommand = new SendCommand(_challengeCommandId, _responseCommandId, timeOut);
            var responseCommand = _cmdMessenger.SendCommand(challengeCommand);
            return responseCommand.Ok;
        }

        public bool ArduinoAvailable(int timeOut, int tries)
        {
            for (var i = 0; i < tries; i++)
            {
                Log(3, "Polling Arduino, try # "+i);
                //Console.WriteLine("Checking if available, try # "+i);
                if(ArduinoAvailable(timeOut)) return true;
            }
            return false;
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
            const int longTimeOut =  1000;
            const int shortTimeOut = 1000;

            // First try if currentConnection is open or can be opened
            if (TryConnection(longTimeOut)) return true;

            // Do a quick rescan of all devices in range
            QuickScanDevices();

            // Then try if last stored connection can be opened
            Log(3, "Trying last stored connection");

            if (TryConnection(_bluetoothConfiguration.BluetoothAddress, longTimeOut)) return true;

            // Then see if new devices have been added to the list 
            if (NewDevicesScan()) return true;

            foreach (var device in _deviceList)
            {
                Thread.Sleep(200);
                Log(1, "Trying Device " + device.DeviceName + " (" + device.DeviceAddress + ") " );
                if (TryConnection(device, shortTimeOut)) return true;
            }

            return false;
        }

        public bool ThoroughScan()
        {
            Log(3, "Performing thorough scan");
            const int longTimeOut = 1000;
            const int shortTimeOut = 1000;

            // First try if currentConnection is open or can be opened
            if (TryConnection(longTimeOut)) return true;

            // Do a quick rescan of all devices in range
            ThorougScanForDevices();

            // Then try if last stored connection can be opened
            Log(3, "Trying last stored connection");
            if (TryConnection(_bluetoothConfiguration.BluetoothAddress,  longTimeOut)) return true;

            // Then see if new devices have been added to the list 
            if (NewDevicesScan()) return true;

            foreach (var device in _deviceList)
            {
                Thread.Sleep(1000);
                if (PairDevice(device))
                {
                    Log(1, "Trying Device " + device.DeviceName + " (" + device.DeviceAddress + ") ");
                    if (TryConnection(device, shortTimeOut)) return true;        
                }
            }
            return false;
        }

        public bool NewDevicesScan()
        {            
            const int shortTimeOut = 200;

            // Then see if port list has changed
            var newDevices = NewDevicesInList();
            if (newDevices.Count == 0) { return false; }
            
            Log(1, "Trying new devices");

            foreach (var device in newDevices)
            {
                if (TryConnection(device, shortTimeOut)) return true;
                Thread.Sleep(100);
            }
            return false;
        }

        private List<BluetoothDeviceInfo> NewDevicesInList()
        {
            return (from device in _deviceList from prevdevice in _prevDeviceList where device.DeviceAddress != prevdevice.DeviceAddress select device).ToList();
        }

        private void StoreSettings()
        {
            _bluetoothConfiguration.BluetoothAddress = _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceAddress;            

            var fileStream = File.Create(SettingsFileName);
            var serializer = new BinaryFormatter();
            serializer.Serialize(fileStream,_bluetoothConfiguration);
            fileStream.Close();
        }

        private void ReadSettings()
        {
            // Read from file
            if (File.Exists(SettingsFileName))
            {
                var fileStream = File.OpenRead(SettingsFileName);
                var deserializer = new BinaryFormatter();
                _bluetoothConfiguration = (BluetoothConfiguration)deserializer.Deserialize(fileStream);
                fileStream.Close();
            }
        }

        public void StartWatchDog(long watchdogTimeOut)
        {
            Log(1, "Starting Watchdog");
            _lastCheckTime = TimeUtils.Millis;
            _nextTimeOutCheck = _lastCheckTime + WatchdogTimeOut;
            _watchdogTries = 0;
            _connectionManagerState = ConnectionManagerStates.Watchdog;
        }

        public void StopWatchDog()
        {
            Log(1, "Stopping Watchdog");
            _connectionManagerState = ConnectionManagerStates.Wait;
        }

        public void StopScan()
        {
            Log(1, "Stopping device scan");
            _connectionManagerState = ConnectionManagerStates.Wait;
        }

        public void StartScan()
        {
            Log(1, "Starting device scan");
            _connectionManagerState = ConnectionManagerStates.Scan;
        }

        private void ConnectionWatchDog()
        {          
            var lastLineTimeStamp = _cmdMessenger.LastReceivedCommandTimeStamp;
            var currentTimeStamp  = TimeUtils.Millis;

            // If timeout has not elapsed, wait till next watch time
            if (currentTimeStamp < _nextTimeOutCheck) return;
            
            // if a command has been received recently, set next check time
            if (lastLineTimeStamp > _lastCheckTime) {
                Log(3, "Successful watchdog response");
                _lastCheckTime = currentTimeStamp;
                _nextTimeOutCheck = _lastCheckTime + WatchdogTimeOut;
                _watchdogTries = 0;
                return;
            }

            _lastCheckTime = currentTimeStamp;
            // Apparently, other side has not reacted in time
            // If too many tries, notify and stop
            if (_watchdogTries >= MaxWatchdogTries) 
            {
                Log(3, "No watchdog response after final try");
                _watchdogTries = 0;
                _connectionManagerState = ConnectionManagerStates.Wait;
                InvokeEvent(ConnectionTimeout);         
            }

            // We'll try another time
            // We queue the command in order to not be intrusive, but put it in front to get a quick answer
            _cmdMessenger.SendCommand(new SendCommand(_challengeCommandId),SendQueue.InFrontQueue,ReceiveQueue.Default);
            _watchdogTries++;

            _lastCheckTime = currentTimeStamp;
            _nextTimeOutCheck = _lastCheckTime + WatchdogRetryTimeOut;
            Log(3, "No watchdog response, performing try #"+ _watchdogTries);
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
                StopConnectionManager();                
            }
        }

    }
}
