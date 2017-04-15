console.log('1')
var noble = require('noble');

// Search only for the Service UUID of the device (remove dashes)
var serviceUuids = ['19B10000E8F2537E4F6CD104768AAD4'];

// Search only for the X, Y, Z Axis charateristics
var characteristicUuids = [
  '19B10001E8F2537E4F6CD104768AAD4',
  '19B10002E8F2537E4F6CD104768AAD4',
  '19B10003E8F2537E4F6CD104768AAD4'
];

// start scanning when bluetooth is powered on
noble.on('stateChange', function(state) {
  console.log('2')
  if (state === 'poweredOn') {
    noble.startScanning(serviceUuids);
  } else {
    noble.stopScanning();
    console.log('stop')
  }
});

// Search for BLE peripherals
noble.on('discover', function(peripheral) {
  console.log('3')
  peripheral.connect(function(error) {
    console.log('connected to peripheral: ' + peripheral.uuid);
    // Only discover the service we specified above
    peripheral.discoverServices(serviceUuids, function(error, services) {
      var service = services[0];
      console.log('discovered service');

      service.discoverCharacteristics(characteristicUuids, function(error, characteristics) {
        console.log('discovered characteristics');
        // Assign Characteristics
        var characteristicX = characteristics[0];
        var characteristicY = characteristics[1];
        var characteristicZ = characteristics[2];

        // Subscribe to each characteristic
        characteristicX.subscribe(function(error) {
          if(error) console.log(error);
        });

        characteristicY.subscribe(function(error) {
          if(error) console.log(error);
        });

        characteristicZ.subscribe(function(error) {
          if(error) console.log(error);
        });

        // Handle data events for each characteristic
        characteristicX.on('data', function(data, isNotification) {
          console.log('X-Axis', data.readUInt8(0))
        });

        characteristicY.on('data', function(data, isNotification) {
          console.log('Y-Axis', data.readUInt8(0))
        });

        characteristicZ.on('data', function(data, isNotification) {
          console.log('Z-Axis', data.readUInt8(0))
        });

      });
    });
  });
});
