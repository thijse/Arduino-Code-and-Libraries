using System;
using System.IO.Ports;
using CommandMessenger;
using CommandMessenger.TransportLayer;

namespace CommandMessengerTests.Acknowledge
{
    // This is the list of recognized commands. These can be commands that can either be sent or received. 
    // In order to receive, attach a callback function to these events
    // 
    // Default commands
    // Note that commands work both directions:
    // - All commands can be sent
    // - Commands that have callbacks attached can be received
    // 
    // This means that both sides should have an identical command list:
    // one side can either send it or receive it (sometimes both)
    // If an command exists on only side 1 or 2, it means can either be send 
    // by 1 but not received by 2, or vice versa.  As such it can be removed
    // 
    enum Command
    {
        // Default commands
        CommError, // 0. reports serial port comm error 
        Comment, // 1. Comment, actual comment in argument
        Ack, // 2. acknowledgment that cmd was received


        AreYouReady, // 3. Command asking if other side is ready
        Err, // 4. Reports incorrectly formatted cmd, or cmd not recognized

        // Embedded acknowledge test:

        AskUsIfReady, // 5. Command asking other side to check if we acknowledge
        YouAreReady, // 6. Command to send to other side to tell them we received their acknowledgment    

        // Sending floating point data test
        FloatAddition, // 7.
        FloatAdditionResult, // 8.

        // Send string test
        StringPing, // 9.
        StringPong, // 10.

        BinaryValuePing, // 11
        BinaryValuePong, // 12
    };



    public class CommandMessengerTest
    {
        private readonly Random _randomNumber;
        private CmdMessenger _cmdMessenger;
        private bool _acknowledgementByEmbeddedFinished;
        private SerialTransport _serialTransport;


        public CommandMessengerTest()
        {
            _randomNumber = new Random(DateTime.Now.Millisecond);
            RunTests();
        }

        // ------------------ Command Callbacks -------------------------
        private void AttachCommandCallBacks()
        {
            _cmdMessenger.Attach(OnUnknownCommand);
            _cmdMessenger.Attach((int)Command.AreYouReady, OnAreYouReadyCommand);
            _cmdMessenger.Attach((int)Command.YouAreReady, OnYouAreReadyCommand);
        }

        // ------------------ Command Callbacks -------------------------


        void OnUnknownCommand(ReceivedCommand arguments)
        {
            // In response to unknown commands and corrupt messages
            Console.WriteLine(" Warn > Command without attached callback received");
        }

        private void OnAreYouReadyCommand(ReceivedCommand arguments)
        {
            // In response to AreYouReady ping. We send an ACK acknowledgment to say that we are ready
            // Method 1:
            //_cmdMessenger.SendCommand((int)Command.Ack, "We are ready");
            // Method 2:
            _cmdMessenger.SendCommand(new SendCommand((int)Command.Ack, "We are ready"));
        }

        private void OnYouAreReadyCommand(ReceivedCommand arguments)
        {
            // in response to YouAreReady message 
            TestSendCommandWithAcknowledgementByArduinoFinished(arguments);
        }
        // ------------------ Send Commands -------------------------


        // ------------------ Test functions -------------------------

        private void RunTests()
        {
            // Test opening and closing connection

            TestOpenConnection();
            TestSendCommand();
            // Test acknowledgments
            TestSendCommandWithAcknowledgement();
            TestSendCommandWithAcknowledgementByArduino();
            WaitForAcknowledgementByEmbeddedFinished();
            TestSendFloatingPointData();
            TestStringPingPong();
            TestBinaryValuePingPong();
            TestCloseConnection();
            // Test special characters 
            // Test all plain text formats
            // Test all binary formats
            // Test large series for completeness (2-way)
            // Test sync + async: switch large queue, with ack
            // Test speed
            // Test load
            // Test Strategies
            // enable dtr
            // 


            /// <summary> Constructor that prevents a default instance of this class from being created. </summary>/
            Pause();
        }


        public void Pause()
        {
            Console.WriteLine("Press any key to stop...");
            Console.ReadKey();
        }


        public void ConnectSerial()
        {
            // Create Serial Port object
            _serialTransport = new SerialTransport
            {
                CurrentSerialSettings =
                {
                    PortName = "COM6",
                    BaudRate = 115200,
                    DataBits = 8,
                    Parity = Parity.None,
                }
            };


            _cmdMessenger = new CmdMessenger(_serialTransport, ',', ';', '/');

            // Attach the callbacks to the Command Messenger
            AttachCommandCallBacks();

            // Attach to NewLineReceived and NewLineSent for logging purposes
            _cmdMessenger.NewLineReceived += NewLineReceived;
            _cmdMessenger.NewLineSent += NewLineSent;

            _cmdMessenger.StartListening();
        }

        // Remove beeps
        private string Silence(string input)
        {
            var output = input.Replace('\x0007', ' ');
            return output;
        }

        private void NewLineReceived(object sender, EventArgs e)
        {
            var message = _cmdMessenger.CurrentReceivedLine;
            Console.WriteLine(" Sent > " + Silence(message));
        }

        private void NewLineSent(object sender, EventArgs e)
        {
            //// Log data to text box
            var message = _cmdMessenger.CurrentSentLine;
            Console.WriteLine(" Sent > " + Silence(message));
        }

        public void DisconnectSerial()
        {
            _cmdMessenger.StopListening();
            _cmdMessenger.Dispose();
            _serialTransport.Dispose();
        }

        public void TestOpenConnection()
        {

            Console.WriteLine("*** Test opening connection");
            try
            {
                ConnectSerial();
            }
            catch (Exception)
            {
                Console.WriteLine("Not OK: Exception during opening Serial Connection");
                return;
            }
            Console.WriteLine("OK");
        }

        public void TestCloseConnection()
        {
            Console.WriteLine("*** Test closing connection");
            try
            {
                DisconnectSerial();
            }
            catch (Exception)
            {
                Console.WriteLine("Not OK: Exception during opening Serial Connection");
            }
            Console.WriteLine("OK");
        }


        // Test: send a command without acknowledgment needed
        public void TestSendCommand()
        {
            try
            {
                Console.WriteLine("*** Test sending command");
                _cmdMessenger.SendCommand(new SendCommand((int)Command.AreYouReady));
            }
            catch (Exception)
            {
                Console.WriteLine("Not OK: Exception during sending of command");
            }
            Console.WriteLine("OK");

        }

        // Test: Send a test command with acknowledgment needed
        public void TestSendCommandWithAcknowledgement()
        {
            Console.WriteLine("*** Test sending command and receiving acknowledgment");
            var result = (_cmdMessenger.SendCommand(new SendCommand((int)Command.AreYouReady, (int)Command.Ack, 1000)) != null);
            if (!result)
            {
                Console.WriteLine("Not OK: No acknowledgment for command AreYouReady");
                return;
            }
            Console.WriteLine("OK");
        }


        // Test: Check if Arduino can also receive acknowledgments       
        // This is is a 4-step test :
        // PC      : AskUsIfReady --> 
        // Arduino :                     <-- AreYouReady
        // PC      : Ack -->  
        // Arduino :                     <--- YouAreReady    

        /// <summary>   Tests send command with acknowledgement by arduino.* </summary>
        /*
        public void TestSendCommandWithAcknowledgementByArduino()
        {
            Console.WriteLine("*** Test receiving acknowledgment on embedded side");
            // The Arduino should respond by calling us with AreYouReady command 
            // We will wait for the YouAreReady response. If we would not wait, the response would trigger the associated 
            // TestSendCommandWithAcknowledgementByArduinoFinished function, but if the embedded system would not respond correctly
            // no code would be called and we would have no way of knowing
            var youAreReadyCommand = _cmdMessenger.SendCommand((int)Command.AskUsIfReady, "", true, (int)Command.YouAreReady, 1000);
            if (youAreReadyCommand!=null)
            {
                
                // Because we are waiting for YouAreReady, this (and any other) callbacks have been blocked
                // We could now trigger a callback function associated with the youAreReadyCommand like:
                
                //_cmdMessenger.HandleMessage(youAreReadyCommand);                             
                
                // But in this case it is easier to we can respond directly 
                var result = int.Parse(youAreReadyCommand[1]);
                Console.WriteLine(result == 1 ? "OK" : "Not OK: unexpected response");
            }
            else
            {
                Console.WriteLine("Not OK: No acknowledgment for command AskUsIfReady");
            }
        }
        */

        public void TestSendCommandWithAcknowledgementByArduino()
        {
            Console.WriteLine("*** TestSendCommandWithAcknowledgementByArduino");
            //SendCommandAskUsIfReady();
            _acknowledgementByEmbeddedFinished = false;
            _cmdMessenger.SendCommand(new SendCommand((int)Command.AskUsIfReady));

            // We will exit here, but the test has just begun:
            // - Next the arduino will call us with AreYouReady command which will trigger OnAreYouReadyCommand() 
            // - After this the Command TestAckSendCommandArduinoFinish will be called by Arduino with results
            // - 
        }


        public void TestSendCommandWithAcknowledgementByArduinoFinished(ReceivedCommand command)
        {
            Console.WriteLine("*** TestSendCommandWithAcknowledgementByArduinoFinished");


            var result = command.ReadBoolArg();
            Console.WriteLine(result ? "OK" : "Not OK: unexpected response");
            _acknowledgementByEmbeddedFinished = true;
        }

        public void WaitForAcknowledgementByEmbeddedFinished()
        {
            for (int i = 0; i < 10; i++)
            {
                if (_acknowledgementByEmbeddedFinished) return;
                System.Threading.Thread.Sleep(1000);
            }
            Console.WriteLine("Not OK: No result on embedded acknowledgment");

        }



        public void TestSendFloatingPointData()
        {
            Console.WriteLine("*** Test sending 2 floats to embedded side, and receiving summation");

            var command = new SendCommand((int)Command.FloatAddition, (int)Command.FloatAdditionResult, 1000);
            command.AddArgument(3.14);
            command.AddArgument(2.71);
            var floatAdditionResultCommand = _cmdMessenger.SendCommand(command);

            if (floatAdditionResultCommand.Ok)
            {

                var result = floatAdditionResultCommand.ReadFloatArg();

                var difference = Math.Abs(result - 5.85);
                if (difference < 0.01)
                {
                    Console.WriteLine("OK");
                }
                else
                {
                    Console.WriteLine("Not OK: unexpected value received:  {0} instead of 5.85");
                }
            }
            else
            {
                Console.WriteLine("Not OK: No response on command FloatAddition");
            }
        }

        private void TestStringPingPong()
        {

            SendStringPingPong("abcdefghijklmnopqrstuvwxyz"); // No special characters, but escaped
            SendStringPingPong("abcde,fghijklmnopqrs,tuvwxyz"); //  escaped parameter separators
            SendStringPingPong("abcde,fghijklmnopqrs,tuvwxyz,"); //  escaped parameter separators at end
            SendStringPingPong("abc,defghij/klmnop//qr;stuvwxyz/"); // escaped escape char at end
            SendStringPingPong("abc,defghij/klmnop//qr;stuvwxyz//"); // double escaped escape  char at end
        }




        private void SendStringPingPong(string pingPongString)
        {


            var command = new SendCommand((int)Command.StringPing, (int)Command.StringPong, 1000);
            command.AddBinArgument(pingPongString); // Adding a string as binary command will escape it
            var stringPingCommand = _cmdMessenger.SendCommand(command);


            if (stringPingCommand != null)
            {

                var receiveString = stringPingCommand.ReadBinStringArg();
                if (pingPongString == receiveString)
                {
                    Console.WriteLine("OK: received string: {0} same as {1}", receiveString, pingPongString);
                }
                else
                {
                    Console.WriteLine("Not OK: received string: {0} instead of {1}", receiveString, pingPongString);
                }
            }
            else
            {
                Console.WriteLine("Not OK: No response on command TestStringPingPong");
            }
        }


        private char IntToSpecialChar(int i)
        {
            switch (i)
            {
                case 0:
                    return ';'; // End of line
                case 1:
                    return ','; // End of parameter
                case 3:
                    return '/'; // Escaping next char
                case 4:
                    return '\0'; // End of byte array
                default:
                    return 'a'; // Normal character

            }
        }

        private void TestBinaryValuePingPong()
        {
            int errorCount = 0;


            // Try some typical numbers
            errorCount += BinaryValuePingPong(0.0F) ? 0 : 1;
            errorCount += BinaryValuePingPong(1.0F) ? 0 : 1;
            errorCount += BinaryValuePingPong(15.0F) ? 0 : 1;
            errorCount += BinaryValuePingPong(65535.0F) ? 0 : 1;
            errorCount += BinaryValuePingPong(0.00390625F) ? 0 : 1;
            errorCount += BinaryValuePingPong(0.00000000023283064365386962890625F) ? 0 : 1;

            //Craft difficult floating point values, using all special characters.
            //These should all be handled correctly by escaping

            for (int a = 0; a < 5; a++)
            {
                for (int b = 0; b < 5; b++)
                {
                    for (int c = 0; c < 5; c++)
                    {
                        for (int d = 0; d < 5; d++)
                        {
                            var charA = IntToSpecialChar(a);
                            var charB = IntToSpecialChar(b);
                            var charC = IntToSpecialChar(c);
                            var charD = IntToSpecialChar(d);
                            errorCount += BinaryValuePingPong(CreateSingle(new[] { charA, charB, charC, charD })) ? 0 : 1;
                        }
                    }
                }
            }

            // Try a lot of random numbers
            for (int i = 0; i < 1000; i++)
            {
                errorCount += BinaryValuePingPong(Randomize()) ? 0 : 1;
            }

            Console.Write("number of failed floats: {0} \n", errorCount);
        }

        public Single Randomize()
        {
            const Single offset = (Single)(-100000.0);
            var value = (Single)((200000.0) * _randomNumber.NextDouble());
            return offset + value;
        }



        Single CreateSingle(char[] chars)
        {
            var bytes = BinaryConverter.CharsToBytes(chars); //_cmdMessenger.T _serialPortManager.StringEncoder.GetBytes(chars);      
            return BitConverter.ToSingle(bytes, 0);
        }

        private bool BinaryValuePingPong(Single inputValue)
        {
            var command = new SendCommand((int)Command.BinaryValuePing, (int)Command.BinaryValuePong, 1000);
            command.AddBinArgument(inputValue);
            var stringPingCommand = _cmdMessenger.SendCommand(command);


            if (stringPingCommand != null)
            {
                var returnedValue = stringPingCommand.ReadBinFloatArg();
                var result = CompareSingles(inputValue, (float)returnedValue);
                return result;
            }

            Console.WriteLine("Not OK: No response on command BinaryValuePingPong");
            return false;
        }


        public static bool CompareSingles(Single input, Single output)
        {
            byte[] byteArray = BitConverter.GetBytes(input);
            Console.Write("{0,16:E7}{1,20} {2,16:E7}", input,
                BitConverter.ToString(byteArray), output);

            float error = Math.Abs((input - output) / input);
            if (error > 1e-3)
            {
                Console.WriteLine("- Not OK");
                return false;
            }
            Console.WriteLine("- OK");
            return true;
        }
    }
}
