/* Node Server for WifiZomatic Network.
It does:
- data interchange btw nodes // actualy I'm changing this. It only does stuff it needs to as  its own mother tree node.
- source/sink db // stores nutrient and shares them
- other things?
*/
import { createRequire } from "module";
const require = createRequire(import.meta.url);

import { Server } from 'node-osc';
import { Bundle, Client } from 'node-osc';

const config = require("./config.json");

var Sound = require("aplay");
new Sound().play("/home/pi/WifiZomaticNetwork/audio/fs1.wav");

var PORT = config.osc.port;
var HOST = config.osc.host;

var plant1ip = "10.0.0.224";

var plants = {"10.0.0.224":{},
              "10.0.0.225":{},
              "10.0.0.226":{},
              "10.0.0.227":{},
              "10.0.0.228":{},
              "10.0.0.229":{},
              "10.0.0.230":{}
              };

var messageCount = 0;

console.log("setting up to listen on " + HOST + " port " + PORT);

var oscServer = new Server(PORT, '0.0.0.0', () => {
  console.log('OSC Server is listening');
  runTest();
});

oscServer.on('message', function (msg, info) {
  console.log(`Got Message: ${msg}`);
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
  var channel = msg[0];
  var letter = msg[1];
  var number = msg[2];
  var command = letter+number;
  console.log("command " + channel);
// process messaged based on the channel. No rebroadcast.
  switch(channel){
    case "/poop":
      doPoop();
      break;
    default:
      console.log("don't understand command channel " + channel);
      // do nothing
  }
//  sendOSCtoAll([321,messageCount],channel, sourceip);
//  messageCount++;
});


function runTest(){
  sendOSCtoAll([201, messageCount], "/plantmessage", false);
  messageCount++;

}

// need send OSC command
function sendOSC(ip, message, channel){
	console.log("sending " + channel + " : " + message + " to " + ip);
	const client = new Client(ip, 9003);
//	client.send('/plantmessage', message, () => {
	client.send(channel, message, () => {
		client.close();
  });
}

function sendOSCtoAll(message, channel, skipip){
  var ips = Object.keys(plants);
  for(var i = 0; i<ips.length; i++){
    if(ips[i] != skipip){
        sendOSC(ips[i], message, channel);
    }
  }
}


function doPoop(){
  console.log("farting");
  new Sound().play("/home/pi/WifiZomaticNetwork/audio/fs1.wav");
}

//setInterval(runTest, 5000);

