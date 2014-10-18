'use strict';

module.exports = {

	lineArgsFromArray: function (args) {
	    var cmd = "";

	    if ("callbackId" in args) {
	        cmd += decodeURIComponent(args.callbackId);
		}
	    if ("0" in args) {
	        if (cmd.length > 0) {
	            cmd += " ";
			}
	        cmd += decodeURIComponent(args["0"]);
	    }

		return cmd;
	},

	cmdLineForMethod: function (method, args) {
	    var cmd = lineArgsFromArray(args);
	    if (cmd.length > 0) {
	        return method + " " + cmd;
		} else {
	        return method;
		}
	},

	TCPSocketLib: function () {
		var self = this,
			hasInstance = false;

		self.init = function () {
			alert("testando...");
			console.log("Initializing Native Socket Class.");
			if (!JNEXT.require("libTCPSocketLib")) {
				return false;
			}

			self.m_id = JNEXT.createObject("libTCPSocketLib.TCPSocketLib_JS");

			if (self.m_id === "") {
				return false;
			}

			JNEXT.registerEvents(self);
		};

		self.getId = function () {
			return self.m_id;
		};

	    self.InvokeMethod = function (method, args) {
	        //This is how Javascript calls into native
	        return JNEXT.invoke(self.m_id, cmdLineForMethod(method, args));
	    };

		// Fired by the Event framework (used by asynchronous callbacks)
		self.onEvent = function (strData) {
			var arData = strData.split(" "),
				callbackId = arData[0],
				result = resultObjs[callbackId],
				data = arData.slice(1, arData.length).join(" ");

			if (result) {
				if (callbackId != threadCallback) {
					result.callbackOk(data, false);
					delete resultObjs[callbackId];
				} else {
					result.callbackOk(data, true);
				}
			}
		};

		self.m_id = "";

		self.getInstance = function () {
			if (!hasInstance) {
				hasInstance = true;
				self.init();
			}
			return self;
		};

	}
};
