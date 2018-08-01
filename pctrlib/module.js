function require(file) {
    var contents = pctr.recv("0", file);

    if(typeof contents === 'string') {
        return pctr.compile(file);
    } else {
        throw `No module ${file} exists!`;
    }
}