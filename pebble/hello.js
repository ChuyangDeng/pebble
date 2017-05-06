Pebble.addEventListener("appmessage", 
  function(e) {
    var message = "";
    if(e.payload.showF){
      showF();
    }
    else if (e.payload.standby) {
      standBy();
    } else if(e.payload.showAvg){
      showAvg();
    } else if (e.payload.showMin) {
      showMin();
    }else if (e.payload.showMax) {
      showMax();
    }
    else if(e.payload.click){
      showClick();
    }
     else if(e.payload.auto){
      showAuto();
     } else if (e.payload.ulong_msg){
       message = "ulong";
       sendToServer2(message);
     } else if (e.payload.dlong_msg){
       message = "dlong";
       sendToServer2(message);
     } else if (e.payload.predict){
       showPred();
     }
  });

var ipAddress = "128.91.168.186"; // Hard coded IP address

function sendToServer() {
  var req = new XMLHttpRequest();
  var port = "3001"; // Same port specified as argument to server 
  var url = "http://" + ipAddress + ":" + port + "/c";
  var method = "GET";
  var async = true;
  req.onload = function(e) {
      // see what came back
      var msg = "no response";
      var response = JSON.parse(req.responseText); 
      if (response) {
          if (response.name) {
          msg = response.name;
          }
      else msg = "noname"; 
      }
      // sends message back to pebble 
      Pebble.sendAppMessage({ "1": msg });
  };
  req.open(method, url, async); 
  req.send(null);
}

function sendToServer2(message) {
// function sendToServer(key) {
  var req = new XMLHttpRequest();
  var port = "3001"; // Same port specified as argument to server
  var url = "http://" + ipAddress + ":" + port + "/";
  url += message;
  var method = "GET";
  var async = true;
  req.onload = function(e) {
    // see what came back
    var msg = "no response";
    var response = JSON.parse(req.responseText);
    if (response) {
      if (response.name){
        msg = response.name;
      } else {
        msg = "no name";
      }

    }
    // sends message back to pebble
    if (msg.localeCompare("dlong")) Pebble.sendAppMessage({ "7": msg });
    else if (msg.localeCompare("ulong")) Pebble.sendAppMessage({ "6": msg });
  };
  req.open(method, url, async);
  req.send(null);
}

function showF() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/f";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;
    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "2": msg });
};
req.open(method, url, async); 
req.send(null);
}

function standBy() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/s";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;
    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "0": msg });
};
req.open(method, url, async); 
req.send(null);
}


function showAvg() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/avg";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;

    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "3": msg });
};
req.open(method, url, async); 
req.send(null);
}

function showMin() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/min";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;

    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "4": msg });
};
req.open(method, url, async); 
req.send(null);
}

function showMax() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/max";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;

    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "5": msg });
};
req.open(method, url, async); 
req.send(null);
}

function showClick() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/click";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;
    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "8": msg });
};
req.open(method, url, async); 
req.send(null);
}
function showAuto() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/auto";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;
    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "9": msg });
};
req.open(method, url, async); 
req.send(null);
}

function showPred() {
var req = new XMLHttpRequest();
var port = "3001"; // Same port specified as argument to server 
var url = "http://" + ipAddress + ":" + port + "/pd";
var method = "GET";
var async = true;
req.onload = function(e) {
// see what came back
  var msg = "no response";
  var response = JSON.parse(req.responseText); 
  if (response) {
    if (response.name) {
      msg = response.name;
    }
    else msg = "noname"; 
  }
// sends message back to pebble 
  Pebble.sendAppMessage({ "10": msg });
};
req.open(method, url, async); 
req.send(null);
}