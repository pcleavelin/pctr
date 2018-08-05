function start(...args) {
    out("Runtime.js started!");

    if(args.length < 2) {
        out("Invalid number of arguments");
        return 1;
    }

    var fs = require("pctrlib/fs.ts");

    var program = compile(args[1], fs.fs.readFileSync(args[1]));

    return program.main(args.slice(1));
}

//#import pctrlib/module.js