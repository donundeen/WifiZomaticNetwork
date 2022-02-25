/* Node Server for WifiZomatic Network.
It does:
- data interchange btw nodes
- source/sink db
- other things?
*/
import { createRequire } from "module";
const require = createRequire(import.meta.url);

import { Server } from 'node-osc';
const config = require("./config.json");

var PORT = config.osc.port;
var HOST = config.osc.host;


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
  // if addone and letter is one of A, C, E,G,J
  if(addone && buggyLetters.indexOf(letter) > -1 ){
  	number++;
  }
  var command = letter+number;
  console.log("command " + command);
});


// need send OSC command



