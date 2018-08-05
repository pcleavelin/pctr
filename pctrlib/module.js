function compile(file) {
    ts.executeCommandLine([file]);

    if(typeof ts.compiledFile === 'undefined') {
        throw `No module ${file} exists!`;
    } else {
        var contents = ts.compiledFile.contents;
        var name = ts.compiledFile.name;
        ts.compiledFile = 'undefined';
        return pctr.compile(name, contents);
    }
}

function require(file) {
    out("Attemping to import " + file);
    return compile(file).exports;
}

// This interfaces our filesystem functoins to the TypeScript compiler
// however, currently you need to change tsc.js so that it doesn't overwrite out ts.sys
var system = (function () {
    var byteOrderMarkIndicator = "\uFEFF";
    function getPCTRSystem() {

        var fs = (function() {
//#import pctrlib/fs.js

            return exports.fs;
        })();

        var realpath = function(path) { return "" + path; };
        return {
            newLine: "\n",
            args: "",
            useCaseSensitiveFileNames: true,
            write: function(s) {
                var str = "tsc: " + s;
                out(str);
                return str;
            },
            readFile: function (path, _encoding) {
                return fs.readFileSync(path);
            },
            writeFile: function (path, data, writeByteOrderMark) {
                ts.compiledFile = { name: path, contents: data };
            },
            resolvePath: function(path) { return path; },
            fileExists: fs.fileExists,
            directoryExists: fs.directoryExists,
            createDirectory: null, //fs.createDirectory,
            getExecutingFilePath: function () { return "third_party/node_modules/typescript/lib/tsc.js"; },
            getCurrentDirectory: function () { return ""; },
            getDirectories: function () { return [""] },
            getEnvironmentVariable: function () { return ""; },
            readDirectory: function (path, extensions, excludes, includes, _depth) {
                //var pattern = ts.getFileMatcherPatterns(path, excludes, includes, true, "/home/patrick/Documents/pctr");
                /*return ChakraHost.readDirectory(path, extensions, pattern.basePaths, pattern.excludePattern, pattern.includeFilePattern, pattern.includeDirectoryPattern);*/
                return ["pctrlib/runtime.js", "pctrlib/fs.ts"];
            },
            exit: function(a) {
                out("tsc tried to exit");
            },
            realpath: null
        };
    }
    function recursiveCreateDirectory(directoryPath, sys) {
        var basePath = ts.getDirectoryPath(directoryPath);
        var shouldCreateParent = basePath !== "" && directoryPath !== basePath && !sys.directoryExists(basePath);
        if (shouldCreateParent) {
            recursiveCreateDirectory(basePath, sys);
        }
        if (shouldCreateParent || !sys.directoryExists(directoryPath)) {
            sys.createDirectory(directoryPath);
        }
    }
    
    var sys = getPCTRSystem();
    
    return sys;
})();

var ts = {};
ts.sys = system;

//#import third_party/node_modules/typescript/lib/tsc.js

exports.ts = ts;