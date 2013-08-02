#!/usr/bin/env python
# -*- coding: cp860 -*-

import sys
import repa
from threading import Thread

terminated = False

class Flushfile(object):
    def __init__(self, fd):
        self.fd = fd

    def write(self, x):
        ret=self.fd.write(x)
        self.fd.flush()
        return ret

    def writelines(self, lines):
        ret=self.writelines(line)
        self.fd.flush()
        return ret

    def flush(self):
        return self.fd.flush

    def close(self):
        return self.fd.close()

    def fileno(self):
        return self.fd.fileno()

def handle_message():
    global terminated
    read_len = 0
    terminated = False
    while not terminated:
        try:
            message = repa.repaRecv(10)
            if message is not None:
                if message[2] > 0:
                    prefix = repa.prefixToString(message[3]) 
                    print 'Receive %s from %s I: "%s" D: "%s"' % (message[2],prefix,message[0],message[1])
        except:
            pass
    return

def main():
    global terminated
    
    interest = 'app-chat://message'
    repa.repaOpen()
    repa.registerInterest(interest)
     
    prefix = repa.prefixToString(repa.getRepaNodeAddress()) 
    print 'NodePrefix: %s' % prefix
    
    t = Thread(target=handle_message)
    t.setDaemon(True)
    t.start()
    
    data = raw_input('')
    data_len = len(data)
    while data != 'exit':
        if data_len > 0:
            if repa.repaSend(interest, data, data_len, 0) < 0:
                print 'CHATERROR: send message'
            else:
                print 'Message sent I: "%s" D: "%s"' % (interest,data)
    
        data = raw_input('')
        data_len = len(data)
    
    print 'Exiting...'
    terminated = True;
    repa.repaClose()
    
# Executable
if __name__ == '__main__':
    sys.stdout = Flushfile(sys.stdout)
    main()