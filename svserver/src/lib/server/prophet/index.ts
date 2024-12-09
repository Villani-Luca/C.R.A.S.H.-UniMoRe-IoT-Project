import env from '../env';
import fs from 'node:fs';
import path from 'node:path';
import child_process from 'node:child_process'
import { stringify, parse } from "csv/sync";
import os from 'os';

export function prophet_predictions(userid: string, data: {ds: Date, y: number}[]) {
    const filename = path.join(os.tmpdir(), userid);
    const input_filename = filename + '.csv';
    const output_filename = filename + 'result.csv';
    
    const mapped = data.map(x => [x.ds.toISOString(), x.y]);
    const output = stringify(mapped, {
        header: true,
        columns: {
            ds: 'ds',
            y: 'y'
        }
    })
    fs.writeFileSync(output_filename, output);

    // D:/Projects/Uni/IoT-3DIntelligentSystem-Project/.venv/Scripts/python.exe 
    const cp = child_process.exec(`& ${env.PVENV_PATH} ${env.PROPHET_FILE} ${input_filename} ${output_filename}`);
    return new Promise<typeof data>((resolve, reject) => {
        cp.on('close', () => {
            fs.readFile(output_filename, (err, data) => {
                if(err){
                    reject(err);
                }
                
                const parsed = parse(data, {
                    columns: true,
                    cast: true,
                    cast_date: true,
                });
                
                resolve(parsed);
            })
        })
    });
}