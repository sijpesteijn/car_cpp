import { Injectable } from '@angular/core';
import {HttpClient} from "@angular/common/http";
@Injectable()
export class Config {
    private config: any;

    constructor(private http: HttpClient) {

    }

    public load(jsonFile: string) {
        return new Promise((resolve, reject) => {
            this.http.get(jsonFile).subscribe(data => {
                this.config = data;
                resolve(true);
            });
        });
    }

    public get(key: string): string {
        const keys = key.split('.');
        let result = this.config;
        keys.forEach((k) => result = result[k]);
        return result;
    }
}