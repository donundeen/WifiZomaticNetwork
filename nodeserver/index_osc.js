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
              "10.0.0.226":{},
              "10.0.0.225":{},
              "10.0.0.227":{},
              "10.0.0.229":{},
              "10.0.0.228":{},
              "10.0.0.230":{},
              "10.0.0.130":{}
              };

var farts = [
	"fart1.wav",
	"fart2.wav",
	"fart3.wav",
	"fart4.wav",
	"fart5.wav",
	"fart6.wav",
	"fart7.wav",
	"fart8.wav"
]

var dangers = [
	"danger1.wav",
	"danger2.wav",
	"danger3.wav",
	"danger4.wav",
	"danger5.wav",
	"danger6.wav",
	"danger7.wav"
]


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
  var value1 = msg[1];
  var value2 = msg[2];
  console.log("values " + value1 + " : " +value2);
// process messaged based on the channel. No rebroadcast.
  switch(channel){
    case "/poop":
      doPoop();
      sendOSCtoAll(1, "/poop", sourceip);
      break;
    case "/danger":
      // rebroadcast danger message
      doDanger();
      sendOSCtoAll(1, "/danger", sourceip);
      break;
    case "/water":
      console.log("got water " + value1);
      sendOSCtoAll(value1, "/water", sourceip);
      break;
    default:
      console.log("don't understand command channel " + channel);
      // do nothing
  }
//  sendOSCtoAll([321,messageCount],channel, sourceip);
//  messageCount++;
});


function runTest(){
  sendOSCtoAll(1, "/danger", false);
  messageCount++;

}

// need send OSC command
function sendOSC(ip, message, channel){
	console.log("sending " + channel + " : " + message + " to " + ip +":"+PORT);
	const client = new Client(ip, PORT);
//	client.send('/plantmessage', message, () => {
	client.send(channel, message, () => {
		client.close();
  });
}

function recursiveSendOSC(ipindex, ips, message, channel, skipip){
	if(ipindex >= ips.length){
		return;
	}
	var ip = ips[ipindex];
        if(skipip == ip){
		ipindex++;
                recursiveSendOSC(ipindex, ips, message, channel, skipip);
		return;
	}
	console.log("Recursive sending " + channel + " : " + message + " to " + ip +":"+PORT);
        const client = new Client(ip, PORT);
//      client.send('/plantmessage', message, () => {
        client.send(channel, message, (err) => {
		if(err){
			console.log("UDP SEnd error " + err);
		}
                client.close(function(){
			console.log("close done");
			ipindex++;
			recursiveSendOSC(ipindex, ips, message, channel,skipip);
		});
	});
}

function sendOSCtoAll(message, channel, skipip){
  var ips = Object.keys(plants);
  var index = 0;
  recursiveSendOSC(index, ips, message, channel, skipip);
 
/*
  sendOSC(ips[i], message, channel);
  for(var i = 0; i<ips.length; i++){
    if(ips[i] != skipip){
    }
  }
*/
}


function doPoop(){
  console.log("farting");
  var path = "/home/pi/WifiZomaticNetwork/audio/"+farts[Math.floor(Math.random() * farts.length)];
  new Sound().play(path);
}

var allowDangerAlert = true;
function doDanger(){
  if(allowDangerAlert){
	allowDangerAlert = false;
  	var path = "/home/pi/WifiZomaticNetwork/audio/"+dangers[Math.floor(Math.random() * dangers.length)];
  	new Sound().play(path);
        setTimeout(()=>{allowDangerAlert = true}, 1000);
  }
}
//sendOSCtoAll(1, "/danger", false);


//setInterval(runTest, 5000);

