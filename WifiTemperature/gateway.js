// Map unique sensor ID to descriptive influx tag
var mapping =
    {
        "28ffcf2b7215010c": "Train",

        "280f41040000809c": "HW-Tank-Middle",
        "284c570400008050": "HW-Tank-Primary-Flow",
        "286b3a26000080e9": "Sitting-Room",
        "286e570400008088": "HW-Tank-Primary-Return",
        "2882b9270000805c": "Radiator-Flow",
        "28be37260000802a": "Boiler-Flow-at-Pump",
        "28bebb270000804f": "HW-Tank-Control",
        "28c76a040000802a": "HW-Tank-Top",

        "28089304000080c4": "Garage-Internal",
        "00933274": "Garage-External",

        "f001dc791": "Loft-BMP180",
        "001dc791": "Loft-DHT22"
    };

// IP:Port sensor network is sending on (influx db default)
var SRC_HOST = '192.168.0.14';
var SRC_PORT = 8089;

// IP:Port influx is listening on (default + 1)
var DST_HOST = '192.168.0.14';
var DST_PORT = 8090;

// ====================== THE CODE ======================

var dgram = require('dgram');
var server = dgram.createSocket('udp4');
var client = dgram.createSocket('udp4');

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" +
        address.port);
});

// Regular expression to extract id from sensor message format:
// temperature,id=28ffcf2b7215010c value=25.6250
var re = /(.*id=)([0123456789abcdef]+)( .*)/;
server.on('message', function (message, remote) {
    console.log("=== Message From " + remote.address + ':' + remote.port + " ===");
    // console.log(message.toString('ascii'));
    var lines = message.toString('ascii').split("\n");
    lines.forEach(function (line) {
        // Extract the sensor id
        var rc = re.exec(line);
        if (rc !== null) {
            // Map from ID to tag
            var tag = mapping[rc[2]];
            if (tag !== undefined) {
                // Substitute id for tag
                line = rc[1] + tag + rc[3];
            }
        }
        console.log(line);
        var buffer = new Buffer(line);
        client.send(buffer, 0, buffer.length, DST_PORT, DST_HOST);
    });
});

server.bind(SRC_PORT, SRC_HOST);
