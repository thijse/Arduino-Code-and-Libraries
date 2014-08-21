using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Threading;
using System.Timers;
using System.Windows.Forms;
using Timer = System.Timers.Timer;

namespace CommandMessenger
{
    public class ConnectionManagerProgressEventArgs : EventArgs
    {
        public int Level { get; set; }
        public String Description { get; set; }
    }



    //public class BluetoothConnectionManager : IDisposable
    //{
    //    private static readonly List<string> CommonDevicePins = new List<string>
    //        {
    //            "0000",
    //            "1111",
    //            "1234"
    //        };

    //    const string SettingsFileName = @"LastConnectedBluetoothSetting.xml";
    //    private BluetoothConfiguration _bluetoothConfiguration;
    //    private readonly CmdMessenger _cmdMessenger;
    //    private readonly BluetoothTransport _bluetoothTransport;
    //    private long _watchdogTimeOut;
    //    private readonly Timer _watchdogTimer;
    //    private int _scanType;

    //    private bool _activeConnection;
    //    public event EventHandler ConnectionTimeout;
    //    public event EventHandler ConnectionFound;
    //    public event EventHandler<ConnectionManagerProgressEventArgs> Progress;
    //    private readonly BackgroundWorker _scanThread;
    //    private readonly int _challengeCommandId;
    //    private readonly int _responseCommandId;

    //    public bool Connected { get; set; }

    //    // The control to invoke the callback on
    //    private Control _controlToInvokeOn;
    //    private readonly object _tryConnectionLock = new object();
    //    private readonly List<BluetoothDeviceInfo> _deviceList;
    //    private List<BluetoothDeviceInfo> _prevDeviceList;

    //    public BluetoothConnectionManager(BluetoothTransport bluetoothTransport, CmdMessenger cmdMessenger, int challengeCommandId, int responseCommandId)
    //    {
    //        _controlToInvokeOn = null;
    //        _bluetoothTransport = bluetoothTransport;
    //        _cmdMessenger = cmdMessenger;
    //        _scanThread = new BackgroundWorker {WorkerSupportsCancellation = true, WorkerReportsProgress = false};
    //        _scanThread.DoWork += ScanThreadDoWork;
    //        _scanThread.RunWorkerCompleted += ScanThreadRunWorkerCompleted;
    //        _challengeCommandId = challengeCommandId;
    //        _responseCommandId = responseCommandId;

    //        _bluetoothConfiguration = new BluetoothConfiguration();
    //        ReadSettings();
    //        _cmdMessenger.Attach((int)responseCommandId, OnResponseCommandId);
    //        //_bluetoothTransport.UpdatePortCollection();

    //        _deviceList = new List<BluetoothDeviceInfo>();
    //        _prevDeviceList = new List<BluetoothDeviceInfo>();
    //        _watchdogTimer = new Timer(2000); 
    //        _watchdogTimer.Elapsed += TimerElapsed;

    //    }

    //    /// <summary> Sets a control to invoke on. </summary>
    //    /// <param name="controlToInvokeOn"> The control to invoke on. </param>
    //    public void SetControlToInvokeOn(Control controlToInvokeOn)
    //    {
    //        _controlToInvokeOn = controlToInvokeOn;
    //    }

    //    private void InvokeEvent(EventHandler eventHandler)
    //    {
    //        try
    //        {
    //            if (eventHandler == null) return;
    //            if (_controlToInvokeOn != null && _controlToInvokeOn.InvokeRequired)
    //            {
    //                //Asynchronously call on UI thread
    //                _controlToInvokeOn.Invoke((MethodInvoker)(() => eventHandler(this, null)));
    //                Thread.Yield();
    //            }
    //            else
    //            {
    //                //Directly call
    //                eventHandler(this, null);
    //            }
    //        }
    //        catch (Exception)
    //        {
    //        }
    //    }

    //    private void InvokeEvent<TEventHandlerArguments>(EventHandler<TEventHandlerArguments> eventHandler, TEventHandlerArguments eventHandlerArguments) where TEventHandlerArguments : EventArgs 
    //    {
    //        try
    //        {
    //            if (eventHandler == null) return;
    //            if (_controlToInvokeOn != null && _controlToInvokeOn.InvokeRequired)
    //            {
    //                //Asynchronously call on UI thread
    //                _controlToInvokeOn.Invoke((MethodInvoker)(() => eventHandler(this, eventHandlerArguments)));
    //                Thread.Yield();
    //            }
    //            else
    //            {
    //                //Directly call
    //                eventHandler(this, eventHandlerArguments);
    //            }
    //        }
    //        catch (Exception)
    //        {
    //        }
    //    }

    //    private void Log(int level, string logMessage) 
    //    {
    //        var args = new ConnectionManagerProgressEventArgs {Level = level, Description = logMessage};
    //        InvokeEvent(Progress, args);
    //    }

    //    private void OnResponseCommandId(ReceivedCommand arguments)
    //    {            
    //        // Do nothing
    //    }

    //    public void StartScan()
    //    {
    //        if (_scanThread.IsBusy != true)
    //        {
    //            // Start the asynchronous operation.
    //            _scanThread.RunWorkerAsync();
    //        }   
    //    }

    //    public void StopScan()
    //    {
    //        if (_scanThread.WorkerSupportsCancellation)
    //        {
    //            // Cancel the asynchronous operation.
    //            _scanThread.CancelAsync();
    //        }
    //    }

    //    private void ScanThreadRunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
    //    {
    //        if (ConnectionFound != null) ConnectionFound.Invoke(this, null);
    //    }

    //    private void ScanThreadDoWork(object sender, DoWorkEventArgs e)
    //    {
    //        if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager scan";
    //        var worker = sender as BackgroundWorker;
    //        _activeConnection = false;
            
    //        while (!_activeConnection)
    //        {
    //            if (worker != null && worker.CancellationPending)
    //            {
    //                e.Cancel = true;
    //                break;
    //            }
    //            if (_scanType == 0)
    //            {
    //                _scanType = 1;
    //                try { _activeConnection = QuickScan(); } catch{}
    //            }                
    //            else if (_scanType == 1)
    //            {
    //                _scanType = 0;
    //                try { _activeConnection = ThoroughScan(); } catch { }                  
    //            }
    //        }
    //    }


    //    private void QuickScanDevices()
    //    {
    //        // Fast
    //        _prevDeviceList = _deviceList;
    //        _deviceList.Clear();
    //        _deviceList.AddRange(_bluetoothTransport.BluetoothClient.DiscoverDevices(255, true, true, false, false));
    //    }

    //    public void ThorougScanForDevices()
    //    {
    //        // Slow
    //        _deviceList.Clear();
    //        _deviceList.AddRange(_bluetoothTransport.BluetoothClient.DiscoverDevices(65536, true, true, true, true));
    //    }

    //    public bool PairDevice(BluetoothDeviceInfo device)
    //    {
    //        //device.Update();
    //        if (device.Authenticated) return true;
    //        // Check if PIN has been stored
    //        if (_bluetoothConfiguration.StoredDevicePins.ContainsKey(device.DeviceAddress))
    //        {
    //            if (BluetoothSecurity.PairRequest(device.DeviceAddress, _bluetoothConfiguration.StoredDevicePins[device.DeviceAddress]))
    //            {
    //                device.Update();
    //                return device.Authenticated;
    //            }                    
    //        }            

    //        // loop through common PIN numbers to see if they pair
    //        foreach (string devicePin in CommonDevicePins)
    //        {
    //            var isPaired = BluetoothSecurity.PairRequest(device.DeviceAddress, devicePin);
    //            if (isPaired)
    //            {
    //                _bluetoothConfiguration.StoredDevicePins[device.DeviceAddress] = devicePin;
    //                StoreSettings();
    //                break;
    //            }
    //        }

    //        device.Update();
    //        return device.Authenticated;
    //    }

    //    public bool TryConnection(BluetoothAddress bluetoothAddress, string pin, int timeOut)
    //    {
    //        // Find
    //        foreach (var bluetoothDeviceInfo in _deviceList)
    //        {
    //            if (bluetoothDeviceInfo.DeviceAddress == bluetoothAddress)
    //            {
    //                return TryConnection(bluetoothDeviceInfo, pin, timeOut);
    //            }
    //        }
    //        return false;
    //    }

    //    public bool TryConnection(BluetoothDeviceInfo bluetoothDeviceInfo, string pin, int timeOut)
    //    {
    //        // Try specific settings
    //        _bluetoothTransport.CurrentBluetoothDeviceInfo = bluetoothDeviceInfo;
    //        return TryConnection(timeOut); 
    //    }

    //    public bool TryConnection(int timeOut)
    //    {
    //        lock(_tryConnectionLock)
    //        Connected = false;
    //        Log(1, @"Trying Bluetooth device" + _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName);
    //        if (_bluetoothTransport.Connect())
    //        {
    //            Connected = (ArduinoAvailable(timeOut));
                
    //            if (Connected)
    //            {
    //                Log(1, "Connected with Bluetooth device " + _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceName);
    //                StoreSettings();
    //            }
    //            return Connected;
    //        }
    //        return false;
    //    }

    //    public bool ArduinoAvailable(int timeOut)
    //    {
    //        Log(3, "Polling Arduino");
    //        var challengeCommand = new SendCommand(_challengeCommandId, _responseCommandId, timeOut);
    //        var responseCommand = _cmdMessenger.SendCommand(challengeCommand);
    //        return responseCommand.Ok;
    //    }

    //    public bool Disconnect()
    //    {            
    //        Connected = false;
    //        return _cmdMessenger.Disconnect();
    //    }

    //    // Single scan on foreground thread
    //    public bool SingleScan()
    //    {
    //        if (QuickScan()) return true;
    //        if (ThoroughScan()) return true;
    //        return false;
    //    }        

    //    public bool QuickScan()
    //    {            
    //        Log(3, "Performing quick scan");
    //        const int longTimeOut =  500;
    //        const int shortTimeOut = 200;

    //        // First try if currentConnection is open or can be opened
    //        if (TryConnection(longTimeOut)) return true;

    //        // Do a quick rescan of all devices in range
    //        QuickScanDevices();

    //        // Then try if last stored connection can be opened
    //        Log(3, "Trying last stored connection");

    //        if (TryConnection(_bluetoothConfiguration.BluetoothAddress, null, longTimeOut)) return true;

    //        // Then see if new devices have been added to the list 
    //        if (NewDevicesScan()) return true;

    //        foreach (var device in _deviceList)
    //        {
    //                Log(1, "Trying Device" + device.DeviceName + " (" + device.DeviceAddress + ") " );
    //                if (TryConnection(device, null, shortTimeOut)) return true;
    //            }

    //        return false;
    //    }

    //    public bool ThoroughScan()
    //    {
    //        Log(3, "Performing thorough scan");
    //        const int longTimeOut = 500;
    //        const int shortTimeOut = 200;

    //        // First try if currentConnection is open or can be opened
    //        if (TryConnection(longTimeOut)) return true;

    //        // Do a quick rescan of all devices in range
    //        ThorougScanForDevices();

    //        // Then try if last stored connection can be opened
    //        Log(3, "Trying last stored connection");
    //        if (TryConnection(_bluetoothConfiguration.BluetoothAddress, null, longTimeOut)) return true;

    //        // Then see if new devices have been added to the list 
    //        if (NewDevicesScan()) return true;

    //        foreach (var device in _deviceList)
    //        {
    //            if (PairDevice(device))
    //            {
    //                Log(1, "Trying Device" + device.DeviceName + " (" + device.DeviceAddress + ") ");
    //                if (TryConnection(device, null, shortTimeOut)) return true;        
    //            }
    //        }

    //        return false;
    //    }

    //    public bool NewDevicesScan()
    //    {            
    //        const int shortTimeOut = 200;

    //        // Then see if port list has changed
    //        var newDevices = NewDevicesInList();
    //        if (newDevices.Count == 0) { return false; }

    //        Console.WriteLine("Trying new devices");
    //        Log(1, "Trying new devices");

    //        foreach (var device in newDevices)
    //        {
    //            if (TryConnection(device, null, shortTimeOut)) return true;
    //            Thread.Sleep(100);
    //        }
    //        return false;
    //    }


    //    private List<BluetoothDeviceInfo> NewDevicesInList()
    //    {
    //        return (from device in _deviceList from prevdevice in _prevDeviceList where device.DeviceAddress != prevdevice.DeviceAddress select device).ToList();
    //    }

    //    private void StoreSettings()
    //    {
    //        _bluetoothConfiguration.BluetoothAddress = _bluetoothTransport.CurrentBluetoothDeviceInfo.DeviceAddress;            

    //        var fileStream = File.Create(SettingsFileName);
    //        var serializer = new BinaryFormatter();
    //        serializer.Serialize(fileStream,_bluetoothConfiguration);
    //        fileStream.Close();
    //    }

    //    private void ReadSettings()
    //    {
    //        // Read from file

    //       // _bluetoothConfiguration.Port = "COM1";
    //       // _bluetoothConfiguration.BaudRate = 115200;
    //        if (File.Exists(SettingsFileName))
    //        {
    //            var fileStream = File.OpenRead(SettingsFileName);
    //            var deserializer = new BinaryFormatter();
    //            _bluetoothConfiguration = (BluetoothConfiguration)deserializer.Deserialize(fileStream);
    //            fileStream.Close();
    //        }
    //    }

    //    public void StartWatchDog(long watchdogTimeOut)
    //    {
    //        _watchdogTimeOut = watchdogTimeOut;
    //        _watchdogTimer.Interval = _watchdogTimeOut;
    //        _watchdogTimer.Start();
    //        Thread.Yield();    
    //    }

    //    public void StopWatchDog()
    //    {              
    //        _watchdogTimer.Stop();
    //        Thread.Yield();             
    //    }

    //    private void TimerElapsed(object sender, ElapsedEventArgs e)
    //    {
    //        try { ConnectionWatchDog(); } catch {}
    //    }

    //    private void ConnectionWatchDog()
    //    {
    //        if (Thread.CurrentThread.Name == null) Thread.CurrentThread.Name = "BluetoothConnectionManager watchdog";
    //        // Check how long ago last command was received
    //        var lastLineTimeStamp = _cmdMessenger.LastReceivedCommandTimeStamp;

    //        if ((TimeUtils.Millis - lastLineTimeStamp) >= 2*_watchdogTimeOut)
    //        {
    //            if (ConnectionTimeout != null)
    //            {                    
    //                {
    //                    StopWatchDog();
    //                    InvokeEvent(ConnectionTimeout); 
    //                }
    //            }
    //        } else if ((TimeUtils.Millis - lastLineTimeStamp) >= _watchdogTimeOut)
    //        {
    //            // We queue the command in order to not be intrusive
    //            _cmdMessenger.QueueCommand(new SendCommand(_challengeCommandId));
    //            // We do not need to more, since a response will update LastReceivedCommandTimeStamp
    //        }
    //    }

    //    // Dispose 
    //    public void Dispose()
    //    {
    //        Dispose(true);
    //    }

    //    // Dispose
    //    protected virtual void Dispose(bool disposing)
    //    {
    //        if (disposing)
    //        {
    //            StopScan();
    //            StopWatchDog();
    //            _scanThread.DoWork -= ScanThreadDoWork;
    //            _scanThread.RunWorkerCompleted -= ScanThreadRunWorkerCompleted;
    //            _watchdogTimer.Elapsed -= TimerElapsed;
    //            _watchdogTimer.Close();
    //        }
    //    }

    //}
}
