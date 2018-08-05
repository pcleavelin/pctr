import { fs } from './pctrlib/fs.ts';

function main(...args) {
    if(fs.fileExists("readdme.md")) {
        var contents = fs.readFileSync("readme.md");
        out(contents);
    } else {
        out("File doesn't exist!");
    }
}