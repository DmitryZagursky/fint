from __future__ import print_function

import binascii
import pygatt

adapter = pygatt.GATTToolBackend()

try:
    adapter.start()
    device = adapter.connect('84:68:3E:00:34:83')
    
    #help(device)
    device.char_write("19b10002-e8f2-537e-4f6c-f3104768aad4",(2).to_bytes(2,'little'))
    value = binascii.hexlify(device.char_read("19b10002-e8f2-537e-4f6c-f3104768aad4"))
    print(value)
finally:
    adapter.stop()
