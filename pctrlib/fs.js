"use strict";
//#import pctrlib/pctr.js
exports.__esModule = true;
var fs = /** @class */ (function () {
    function fs() {
    }
    fs.readFileSync = function (file) {
        var result = pctr.recv(ERecvType.FILE_READ, file);
        if (typeof result === 'string') {
            return result;
        }
        else {
            throw "Couldn't read file";
        }
    };
    fs.fileExists = function (file) {
        // TODO: call dedicated C++ function to check if a file exists
        out("checking if " + file + " exists");
        return typeof pctr.recv(ERecvType.FILE_READ, file) == 'string';
    };
    fs.directoryExists = function (file) {
        // TODO: call dedicated C++ function to check if a directory exists
        //return typeof pctr.recv(ERecvType.FILE_READ, file) == 'string';
        return true;
    };
    fs.createDirectory = function (file) {
        // TODO: call dedicated C++ function to check if a directory exists
        //return typeof pctr.recv(ERecvType.FILE_READ, file) == 'string';
        throw "Not implemented";
    };
    return fs;
}());
exports.fs = fs;
