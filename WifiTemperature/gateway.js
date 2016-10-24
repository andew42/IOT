// Map unique sensor ID to descriptive influx tag
var mapping =
{
    "28ffcf2b7215010c" : "Train",
    "284c570400008050" : "HW Tank Center"
};

// IP:Port sensor network is sending on
var SRC_HOST = '172.20.10.2';
var SRC_PORT = 8099;

// IP:Port influx is listening on
var DST_HOST = '172.20.10.2';
var DST_PORT = 8089;

// ====================== THE CODE ======================

var dgram = require('dgram');
var server = dgram.createSocket('udp4');
var client = dgram.createSocket('udp4');

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

// Regular expression to extract id from sensor message format:
// temperature,id=28ffcf2b7215010c value=25.6250
var re = /(.*id=)([0123456789abcdef]{16})(.*)/;
server.on('message', function (message, remote) {
    // Extract the sensor id
    var rc = re.exec(message);
    if (rc !== null)
    {
        // Map from ID to tag
        var tag = mapping[rc[2]];
        if (tag !== undefined)
        {
            // Substitute id for tag
            message = rc[1] + tag + rc[3];
        }
    }
    console.log(remote.address + ':' + remote.port +' - ' + message);
    var buffer = new Buffer(message);
    client.send(buffer, 0, buffer.length, DST_PORT, DST_HOST);
});

server.bind(SRC_PORT, SRC_HOST);
