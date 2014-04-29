using System;
using CommandMessenger;
using CommandMessenger.TransportLayer;

namespace CommandMessengerTests
{
    public class Acknowledge 
    {        
        private CmdMessenger _cmdMessenger;
        readonly Enumerator _command;
        private bool _acknowledgementByEmbeddedFinished;
        private readonly TestPlatform _testPlatform;


        public Acknowledge(TestPlatform testPlatform, Enumerator command)
        {
            _testPlatform = testPlatform;
            _command = command;
            DefineCommands();
        }

        // ------------------ Command Callbacks -------------------------
        private void DefineCommands()
        {
            _command.Add(new[]
                {
                    "AskUsIfReady", // Command asking other side to check if we acknowledge
                    "YouAreReady"   // Command to send to other side to tell them we received their acknowledgment                     
                });
        }

        private void AttachCommandCallBacks()
        {
            _cmdMessenger.Attach(_command["AreYouReady"], OnAreYouReadyCommand);
            _cmdMessenger.Attach(_command["YouAreReady"], OnYouAreReadyCommand);
        }


        // ------------------ Command Callbacks -------------------------

        private void OnAreYouReadyCommand(ReceivedCommand arguments)
        {
            // In response to AreYouReady ping. We send an ACK acknowledgment to say that we are ready
            _cmdMessenger.SendCommand(new SendCommand(_command["Ack"], "We are ready"));
        }

        private void OnYouAreReadyCommand(ReceivedCommand arguments)
        {
            // in response to YouAreReady message 
            TestSendCommandWithAcknowledgementByArduinoFinished(arguments);
        }

        // ------------------ Test functions -------------------------

        public void RunTests()
        {
            // Test opening and closing connection
            Common.StartTestSet("Waiting for acknowledgments");
            SetUpConnection();
            // Test acknowledgments
            TestSendCommandWithAcknowledgement();
            TestSendCommandWithAcknowledgementByArduino();
            WaitForAcknowledgementByEmbeddedFinished();

            TestSendCommandWithAcknowledgementAfterQueued();

            CloseConnection();
            Common.EndTestSet();
        }

        public void SetUpConnection()
        {
            //Common.StartTest("SetUpConnection");
            try
            {
                _cmdMessenger = Common.Connect(_testPlatform);
                AttachCommandCallBacks();
            }
            catch (Exception)
            {
                //Common.TestNotOk("Exception during opening connection");
            }
            if (!_testPlatform.Transport.IsConnected())
            {
                //Common.TestNotOk("Not open after trying to open connection");
            }
           // Common.EndTest();
        }

        public void CloseConnection()
        {
           // Common.StartTest("CloseConnection");
            try
            {
                Common.Disconnect();
            }
            catch (Exception)
            {
          //      Console.WriteLine("Not OK: Exception during closing Connection");
            }
           // Common.EndTest();
        }


        // Test: Send a test command with acknowledgment needed
        public void TestSendCommandWithAcknowledgement()
        {
            Common.StartTest("Test sending command and receiving acknowledgment");
            var receivedCommand = _cmdMessenger.SendCommand(new SendCommand(_command["AreYouReady"], _command["Ack"], 1000)) ;
            if (receivedCommand.Ok)
            {
                Common.TestOk("Acknowledgment for command AreYouReady");                
            }
            else
            {
                Common.TestNotOk("No acknowledgment for command AreYouReady");
            }
            Common.EndTest();
        }


        public void TestSendCommandWithAcknowledgementAfterQueued()
        {
            Common.StartTest("Test sending command and receiving acknowledgment after larger queue");
            
            // Quickly sent a bunch of commands, that will be combined in a command string
            for (var i = 0; i < 100; i++)
            {
                _cmdMessenger.QueueCommand(new SendCommand(_command["AreYouReady"]));
            }

            // Now wait for an acknowledge, terminating the command string
            var receivedCommand = _cmdMessenger.SendCommand(new SendCommand(_command["AreYouReady"], _command["Ack"], 1000)) ;
            if (receivedCommand.Ok)
            {
                Common.TestOk("Acknowledgment for command AreYouReady");                
            }
            else
            {
                Common.TestNotOk("No acknowledgment for command AreYouReady");
            }
            Common.EndTest();
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
            Common.StartTest("TestSendCommandWithAcknowledgementByArduino");
            //SendCommandAskUsIfReady();
            _acknowledgementByEmbeddedFinished = false;
            _cmdMessenger.SendCommand(new SendCommand(_command["AskUsIfReady"]));

            // We will exit here, but the test has just begun:
            // - Next the arduino will call us with AreYouReady command which will trigger OnAreYouReadyCommand() 
            // - After this the Command TestAckSendCommandArduinoFinish will be called by Arduino with results
            // - 
        }


        public void TestSendCommandWithAcknowledgementByArduinoFinished(ReceivedCommand command)
        {
            //Console.WriteLine("*** TestSendCommandWithAcknowledgementByArduinoFinished");

            var result = command.ReadBoolArg();
            if (!result)
            {
                Common.TestNotOk("Incorrect response");
            }
            //Console.WriteLine(result ? "OK" : "Not OK: unexpected response");
            _acknowledgementByEmbeddedFinished = true;
        }

        public void WaitForAcknowledgementByEmbeddedFinished()
        {
            for (var i = 0; i < 10; i++)
            {
                if (_acknowledgementByEmbeddedFinished)
                {
                    Common.TestOk("Received acknowledge from processor");
                    return;
                }
                System.Threading.Thread.Sleep(1000);
            }
            Common.TestNotOk("Received no acknowledge from  processor");

        }

    }
}
