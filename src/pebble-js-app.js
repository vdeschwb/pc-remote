function ServerConnection(IP) {
    this.pending_messages = [];
    this.IP = IP;
    if (IP!==null) {
        this._create_websocket();
    }
    
    this.close = function() {
        try {
            this.ws.close();
        } catch (e) {}
    };

    this.reconnect = function(IP) {
        console.log("Reconnecting with IP: " + IP);
        this.close();
        this.IP = IP;
        this._create_websocket();
    };
    
    this._retry_connect = function() {
        if (this.ws.readyState == 3) {
            console.log('retry_connect');
            this.reconnect(this.IP);
        }
    };

    this.register_callbacks = function() {
        this.ws.onopen = function () {
            console.log("Socket opened.");
        };


        this.ws.onclose = function (e) {
            console.log("Socket closed.");
            this.parent._retry_connect();
        };

        this.ws.onmessage = function (e) {
            if (e.data != 'ping') {
                console.log("Socket message:", e.data);
            }
            
            var reply = this.parent.pending_messages.shift();
            
            if (reply !== undefined) {
                console.log("Sending message: " + reply);
            } else {
                reply = 'pong';
            }
            this.send(reply);
        };

        this.ws.onerror = function (e) {
            console.log("Socket error:", e);
            this.parent._retry_connect();
        };
    };

    this._create_websocket = function() {
        this.ws = new WebSocket('ws://'+ this.IP + ':31415');
        this.ws.parent = this;
        this.register_callbacks();
//         return ws;
    };

    this.send = function(msg) {
        console.log("Queueing message: " + msg);
        this.pending_messages.push(msg);
    };
}


var connection = new ServerConnection(null);



Pebble.addEventListener('ready', function() {
    console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function(e) {
    var url = 'http://vdeschwb.github.io/Tick-Talk/';
    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration page returned: ' + JSON.stringify(configData));

//     var dict = {};
//     dict['DISPLAY_STYLE_KEY'] = configData['display_style'];
//     dict['FPS_KEY'] = configData['fps'];
    
      
    connection.reconnect(configData.IP);
       
    // Send to watchapp
//     Pebble.sendAppMessage(dict, function() {
//         console.log('Send successful: ' + JSON.stringify(dict));
//         }, function() {
//         console.log('Send failed!');
//     });
});


Pebble.addEventListener("appmessage", function(e) {
//     console.log(JSON.stringify(e));
    connection.send(e.payload.COMMAND_KEY.toString());
});