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

var plants = {"10.0.0.225":{}};


console.log("setting up to listen on " + HOST + " port " + PORT);

var oscServer = new Server(PORT, '0.0.0.0', () => {
  console.log('OSC Server is listening');
  runTest();
});

oscServer.on('message', function (msg, info) {
  console.log(`Message: ${msg}`);
  console.log(msg);
  console.log(JSON.stringify(info, null, '  '));
  /*
  info format:
  {
  "address": "10.0.0.225",
  "family": "IPv4",
  "port": 9003,
  "size": 32
  }
  */
  var sourceip = info.address;
  if(!plants[sourceip]){
    plants[sourceip] = {info: info};
  }
  var path = msg[0];
  var letter = msg[1];
  var number = msg[2];
  var command = letter+number;
  console.log("command " + command);

  sendOSCtoAll(9);
});


function runTest(){
	sendOSCtoAll([201, 101]);
}

// need send OSC command
function sendOSC(ip, message){
	console.log("sending " + message);
	const client = new Client(ip, 9003);
	client.send('/plantmessage', message, () => {
		client.close();
  });
}

function sendOSCtoAll(message){
  var ips = Object.keys(plants);
  for(var i = 0; i<ips.length; i++){
    sendOSC(ips[i], message);
  }
}


