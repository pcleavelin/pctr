import { ERecvType } from './pctrlib/pctr.ts';

export class fs {
    public static readFileSync(file: string): string {
        var result = pctr.recv(ERecvType.FILE_READ, file);

        if(typeof result === 'string') {
            return result;
        } else {
            throw "Couldn't read file";
        }
    }
}