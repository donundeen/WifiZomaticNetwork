/* Node Server for WifiZomatic Network.
It does:
- data interchange btw nodes
- source/sink db
- other things?
*/
import { createRequire } from "module";
const require = createRequire(import.meta.url);

import { Server } from 'node-osc';
import { Bundle, Client } from 'node-osc';

const config = require("./config.json");

var PORT = config.osc.port;
var HOST = config.osc.host;

var plant1ip = "10.0.0.225";


console.log("setting up to listen on " + HOST + " port " + PORT);

var oscServer = new Server(PORT, '0.0.0.0', () => {
  console.log('OSC Server is listening');
  runTest();
});

oscServer.on('message', function (msg) {
  console.log(`Message: ${msg}`);
  console.log(msg);
  var path = msg[0];
  var letter = msg[1];
  var number = msg[2];
  var command = letter+number;
  console.log("command " + command);
  sendOSC(9);
});


function runTest(){
	sendOSC(201);
}

// need send OSC command
function sendOSC(message){
	console.log("sending " + 9);
	const client = new Client(plant1ip, 9003);
	client.send('/plantmessage', 200, () => {
		client.close();
    });

}



