import { fs } from './pctrlib/fs.ts';

function main() {
    var contents = fs.readFileSync("readme.md");
    out(contents);
}