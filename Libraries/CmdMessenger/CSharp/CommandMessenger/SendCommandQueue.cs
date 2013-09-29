using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace CommandMessenger
{
    class SendCommandQueue
    {
        private readonly Thread _queueThread;
        private readonly Queue<SendCommand> _queue = new Queue<SendCommand>();               // Buffer for Commands
        private readonly CmdMessenger _cmdMessenger;

        public SendCommandQueue(CmdMessenger cmdMessenger)
        {
            _cmdMessenger = cmdMessenger;
            
            // Create queue thread and wait for it to start
            _queueThread = new Thread(ProcessQueue) {Priority = ThreadPriority.BelowNormal};
            _queueThread.Start();
            while (!_queueThread.IsAlive);
        }

        private void ProcessQueue()
        {
            // Endless loop
            while (true)
            {
                SendCommandFromQueue();
            }
        }

        private void SendCommandFromQueue()
        {
            SendCommand sendCommand;
            lock (_queue)
            {
                sendCommand = _queue.Count != 0 ? _queue.Dequeue() : null;
            }
            if (sendCommand!=null) _cmdMessenger.SendCommand(sendCommand);             
        }

        public void QueueCommand(SendCommand sendCommand)
        {
            _queue.Enqueue(sendCommand);
        }
    }
}
