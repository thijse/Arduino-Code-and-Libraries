using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CommandMessenger.Queue;

namespace CommandMessenger
{
    class QueueStrategy
    {
        public QueueStrategy(SendCommand sendCommand)
        {
            SendCommand = sendCommand;   
        }

        public ListQueue<SendCommand> Queue { get; set; }
        public SendCommand SendCommand { get; private set; }

        public void OnQueued()
        {
            // Remove all commands with the same CmdId
            foreach (var sendCommand in Queue)
            {
                if (sendCommand.CmdId == ThisCommand.CmdId)
                {
                    Queue.Remove(sendCommand.CmdId);
                }
            }
            //Add this command to the queue
            Queue.Enqueue(ThisCommand);
        }

        public void OnSend()
        {
           // Nothing to do 
        }
    }
}
