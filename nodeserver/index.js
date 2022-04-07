/* Node Server for WifiZomatic Network.
It does:
- data interchange btw nodes // actualy I'm changing this. It only does stuff it needs to as  its own mother tree node.
- source/sink db // stores nutrient and shares them
- other things?
*/
import { createRequire } from "module";
const require = createRequire(import.meta.url);

const mqtt = require("mqtt");

const config = require("./config.json");

var Sound = require("aplay");
new Sound().play("/home/pi/WifiZomaticNetwork/audio/fs1.wav");

//var PORT = config.osc.port;
var HOST = config.osc.host;
var PORT = 1883;

const clientId = "mothertree";

var plants = {"10.0.0.224":{},
              "10.0.0.226":{},
              "10.0.0.225":{},
              "10.0.0.227":{},
              "10.0.0.229":{},
              "10.0.0.228":{},
              "10.0.0.230":{}
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

const connectUrl = `mqtt://${HOST}:${PORT}`;

const client = mqtt.connect(connectUrl, {
	clientId,
	client: true,
	connectTimeout: 10000,
	reconnectPeriod: 1000
});


client.on('connect', () => {
	console.log("connected");
	client.subscribe(['Test'], ()=> {
		console.log(`Subscribe to topic 'Test'`);
	});
        client.subscribe(['/danger'], ()=> {
                console.log(`Subscribe to topic '/danger'`);
        });
        client.subscribe(['/poop'], ()=> {
                console.log(`Subscribe to topic '/poop'`);
        });
        client.subscribe(['/water'], ()=> {
                console.log(`Subscribe to topic '/water'`);
        });
});

client.on('message', (topic, payload) => {

	console.log("got message", topic, payload.toString());
	switch(topic){
		case "Test":
			console.log("got test message " + payload);
			break;
        	case "/poop":
      			doPoop();
      			break;
    		case "/danger":
		      // rebroadcast danger message
      			doDanger();
		        break;
    		case "/water":
			// nothing to do
			break;
		default:
			console.log("don't know what to do with " + topic);
	}
});

function runTest(){
  messageCount++;
	client.publish('/danger', 'nodejs mqtt test', {qos: 0, retain: false}, (error) => {
		if (error){
			console.log("got error" + error);
		}
	});

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

