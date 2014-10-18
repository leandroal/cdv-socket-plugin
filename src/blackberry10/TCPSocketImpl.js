    /*disconnectNDK: function (const QVariantMap& args);
    disconnectAllNDK: function ();
    send: function (const QVariantMap& args);
    isConnected: function (const QVariantMap& args);
    receive: function (QString callbackId, const QVariantMap& args);

	// Asynchronous function calls into the plugin and returns
	tcpsocketTestAsync: function (success, fail, args, env) {
		var result = new PluginResult(args, env);
		resultObjs[result.callbackId] = result;
		args = JSON.parse(decodeURIComponent(args["input"]));
		tcpsocket.getInstance().tcpsocketTestAsync(result.callbackId, args);
		result.noResult(true);
	}
	*/

//var PluginResult = cordova.require('cordova/PluginResult');

var TCPSocketJNEXT = require('com.tlantic.plugins.socket.TCPSocketJNEXT');

var resultObjs = {};
var threadCallback = null;

exports.ping = function (success, fail, args, env) {
    		var result = new PluginResult(args, env);
    		params = {"message": args[0]}
            var response = bbtcpsocket.getInstance().InvokeMethod("ping", params);
            var data = JSON.parse(response);
            console.log("ping: " + JSON.stringify(response));
    
            if (data.error !== undefined) {
                result.error({
                    code: data.error.code,
                    message: data.error.message
                });
            } else {
                result.ok({
                    status: data.result
                });
            }
    };
    
exports.connect = function (success, fail, args, env) {
    /*	var result = new PluginResult(args, env);
    	// TODO: count the number of arguments == 2
    	params = {"host": args[0], "port": args[1]}
        var response = bbtcpsocket.getInstance().InvokeMethod("connect", params);
        var data = JSON.parse(response);
        console.log("connect: " + JSON.stringify(response));
    
        if (data.error !== undefined) {
    		result.error();
        } else {
    		result.ok();
        }
*/
		var result = new PluginResult(args, env);
		alert("Testando connect");
};

module.exports = {Socket: exports};
bbtcpsocket = new TCPSocketJNEXT.TCPSocketLib();
alert(bbtcpsocket.getId());
