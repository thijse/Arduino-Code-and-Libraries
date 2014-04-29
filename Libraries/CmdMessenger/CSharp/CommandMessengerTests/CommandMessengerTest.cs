// *** CommandMessengerTest ***

// This project runs unit tests on several parts on the mayor parts of the CmdMessenger library
// Note that the primary function is not to serve as an example, so the code may be less documented 
// and clean than the example projects. 



using System;
using System.IO.Ports;
using CommandMessenger;
using CommandMessenger.TransportLayer;

namespace CommandMessengerTests
{
 
    public class CommandMessengerTest
    {        
        private SetupConnection _setupConnection;
        private Acknowledge _acknowledge;
        private ClearTextData _clearTextData;
        private BinaryTextData _binaryTextData;
        private TransferSpeed _transferSpeed;
        private MultipleArguments _multipleArguments;

        public CommandMessengerTest()
        {
            // Set up board & transport mode
            var testPlatform = new TestPlatform()
                {
                    Description = @"Teensy 3.1",
                    MinDirectSendSpeed =   3500, // Bytes per second
                    MinSendSpeed       =  90000, // Bytes per second
                    MinReceiveSpeed    = 140000, // Bytes per second       
                    BoardType = BoardType.Bit32, // 32 architecture, needed from binary value conversion
                    Transport = new SerialTransport
                        {
                            CurrentSerialSettings = new SerialSettings()
                                {
                                    PortName = "COM15",     // Can be different!
                                    BaudRate = 115200,      // Bits per second
                                    DataBits = 8,
                                    Parity = Parity.None,
                                    DtrEnable = false,          // Some boards need to send this to enabled                                    
                                },
                            
                        }
                };

            // Set up Command enumerators
            var command = DefineCommands();

            // Initialize tests
            InitializeTests(testPlatform, command);

            // Open log file for testing 
            Common.OpenTestLogFile(@"TestLogFile.txt");
            
            // Run tests
            RunTests();

            Common.CloseTestLogFile();
        }


        private static Enumerator DefineCommands()
        {
            var command = new Enumerator();
            // Set up default commands
            command.Add(new[]
                {
                    "CommError", // Command reports serial port comm error (only works for some comm errors)
                    "kComment", // Command to sent comment in argument
                });
            return command;
        }

        private void InitializeTests(TestPlatform testPlatform, Enumerator command)
        {
            _setupConnection   = new SetupConnection(testPlatform, command);
            _acknowledge       = new Acknowledge(testPlatform, command);
            _clearTextData     = new ClearTextData(testPlatform, command);
            _binaryTextData    = new BinaryTextData(testPlatform, command);
            _multipleArguments = new MultipleArguments(testPlatform, command);
            _transferSpeed     = new TransferSpeed(testPlatform, command);

        }

        private void RunTests()
        {
            // Test opening and closing connection
            _setupConnection.RunTests();
            
            // Test acknowledgment both on PC side and embedded side
            _acknowledge.RunTests();

            // Test all plain text formats
            _clearTextData.RunTests();

            // Test all binary formats
            _binaryTextData.RunTests();            
            
            // Test sending multiple arguments
            _multipleArguments.RunTests();

            // Test large series for completeness (2-way)
            // todo
            
            // Test speed
            _transferSpeed.RunTests();

            // Test load
            // todo
            
            // Test Strategies
            // todo
            
            // Summary of tests
            Common.TestSummary();

            // Exit application
            exit();            
        }


        public void exit()
        {
            Console.WriteLine("Press any key to stop...");
            Console.ReadKey();
            Environment.Exit(0);
        }

    }
}
