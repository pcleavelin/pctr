//#import pctrlib/module.js

function start(...args) {
    out("Runtime.js started!");

    if(args.length < 2) {
        out("Invalid number of arguments");
        return 1;
    }

    var program = pctr.compile(args[1]);

    return program.main(args.slice(1));
}