var ws = require("nodejs-websocket")
var request = require("request")
console.log("Listening...")

//Array who mirror current active connections on server
var connections=[];


//initiate the server variable
var server = ws.createServer(function (conn) {

//Getting ip from a device connected
  var ip = conn.socket.remoteAddress.toString();
//formating on desire format
  ip  = ip.substring(ip.lastIndexOf(':')+1);
  console.log("New connection from:" + ip);

//Sends a Symbol who will make device reply with informations about him
  conn.sendText('$');

//Request ping
  conn.sendText('!');

//Function who will load when a device sends a text to server
  conn.on("text", function (str) {

    //Is a Ping Message?
    if (str=='ping') {
      //Reply with Pong
      conn.sendText('pong');
    }
    
    //Is a JSON message?
    else if(str.indexOf("{")>=0){
      //format text recived in JSON
      var jsonDevice = JSON.stringify(eval("(" + str + ")"));
      jsonDevice = JSON.parse(jsonDevice);

      //Put in jsonDevice a KEY who represents a KEY of current connection ( NEEDS TO HAVE SOMETHING UNIQUE ON CONNECTIONS PROVIDED BY SERVER)
      //jsonDevice.Device.key=conn.key;

      //Save in mirrored array the current connection with key as index
      connections[jsonDevice.Device.id]=conn;
      conn.deviceID=jsonDevice.Device.id;
      conn.sendText("Connected");


  //  console.log(connections[jsonDevice.Device.macAddress]);
      }
    
    else if (str.indexOf("%")>=0){
	request("http://apimm.rocketz.com.br/socialapi?youtube=canalinutilismo&instagram=lucasinutilismo&facebook=Inutilismo&twitter=lucasinutilismo",function(error, response, body){
	if(!error && response.statusCode==200){
	   conn.sendText(body + "L" + body.length);
	}
	});

    }	
    //Is a target Command?
    else if(str.indexOf("@")>=0){
        var targetMacAddress=str.substring(str.lastIndexOf('@')+1,str.lastIndexOf('&'));
        //Get message
        var message=str.substring(str.lastIndexOf('&')+1);
	sendToDevice(targetMacAddress,message);
      }

      //When recived Text dont match with any preconfigured option
      else{
        console.log("Received "+str);
        }
    })

    //When connection gets close
    conn.on("close", function (code, reason) {
      //getting connection key
      connectionKey=this.deviceID;
      console.log(connectionKey  + " Connection closed");
      //Remove connection from mirrored array
      connections.slice(connectionKey);

    })

    //When connection gets error
    conn.on("error",function(err) {
      console.log("Error");

    })

}).listen(82)

//Function who will send a BroadcastMessage for a group of Connections
function broadcast(connections, msg) {
  //For Each connection will send the message
    connections.forEach(function (conn) {
    var ip = conn.socket.remoteAddress.toString();
    ip  = ip.substring(ip.lastIndexOf(':')+1);
    conn.sendText(msg)
    })
}

//Send Message to a selected device (CurrentConnections device)
function sendToDevice(id,message) {
  //get in mirrored array a connection with selected key
  if(connections[id]!=null){
    connections[id].sendText(message);
    console.log(message);
  }
    
}
