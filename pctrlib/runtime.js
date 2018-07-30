function start(...args) {
    out("Runtime.js started!");

    if(args.length < 2) {
        out("Invalid number of arguments");
        return 1;
    }

    return pctr.execute(args[1], "main");
}