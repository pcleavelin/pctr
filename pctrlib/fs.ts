//#import pctrlib/pctr.js

export class fs {
    public static readFileSync(file: string): string {
        var result = pctr.recv(ERecvType.FILE_READ, file);

        if(typeof result === 'string') {
            return result;
        } else {
            throw "Couldn't read file";
        }
    }

    public static fileExists(file: string): boolean {
        // TODO: call dedicated C++ function to check if a file exists
        out(`checking if ${file} exists`);
        return typeof pctr.recv(ERecvType.FILE_READ, file) === 'string';
    }

    public static directoryExists(file: string): boolean {
        // TODO: call dedicated C++ function to check if a directory exists
        //return typeof pctr.recv(ERecvType.FILE_READ, file) == 'string';
        return true;
    }

    public static createDirectory(file: string): void {
        // TODO: call dedicated C++ function to check if a directory exists
        //return typeof pctr.recv(ERecvType.FILE_READ, file) == 'string';
        throw "Not implemented";
    }
}