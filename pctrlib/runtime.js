function start(...args) {
    out("Runtime.js started!");

    return pctr.execute("examples/hello_world/hello_world.js", "main");
}