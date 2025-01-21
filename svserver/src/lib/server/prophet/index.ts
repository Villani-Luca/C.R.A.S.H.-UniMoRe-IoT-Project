import env from '../env';
import fs from 'node:fs';
import path from 'node:path';
import child_process, { ChildProcess } from 'node:child_process'
import { stringify, parse } from "csv/sync";
import os from 'os';

export type ProphetPrediction = {
    ds: Date,
    trend: number,
    yhat: number,
    yhat_lower: number, 
    yhat_upper: number,
    trend_lower: number,
    trend_upper: number,
    additive_terms: number,
    additive_terms_lower: number,
    additive_terms_upper: number,
    weekly: number,
    weekly_lower: number,
    weekly_upper: number,
    multiplicative_terms: number,
    multiplicative_terms_lower: number,
    multiplicative_terms_upper: number,
}


export function prophet_predictions(userid: string, data: {ds: Date, y: number}[], frequency: 'D' | 'M'): Promise<ProphetPrediction[]> {
    const filename = path.join(os.tmpdir(), userid);
    const input_filename = filename + '.csv';
    const output_filename = filename + '.result.csv';
    const periods = frequency === 'M' ? 3 : 30; // predict the next MONTHS : DAYS
    
    const mapped = data.map(x => [x.ds.toISOString().split('T')[0], x.y]);
    const output = stringify(mapped, {
        header: true,
        columns: {
            ds: 'ds',
            y: 'y'
        }
    })
    fs.writeFileSync(input_filename, output);

    // D:/Projects/Uni/IoT-3DIntelligentSystem-Project/.venv/Scripts/python.exe 
    return new Promise<ProphetPrediction[]>((resolve, reject) => {
        const cp = child_process.spawn(
            env.PVENV_PATH, 
            [env.PROPHET_FILE, input_filename, output_filename, frequency, periods.toString()],
            {
                stdio: [process.stdin, process.stdout, process.stderr]
            }
        );
        
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