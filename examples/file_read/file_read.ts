import { fs } from './pctrlib/fs.ts';

export function main(...args) {
    var contents = fs.readFileSync("readme.md");
    out(contents);
}