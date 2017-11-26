
import {Injectable} from "@angular/core";
import {Config} from "./app.config";
import {Http} from "@angular/http";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {CarObserver} from "./observers/observer.service";

export interface Race {
    name: string;
    running: number;
    observers?: CarObserver[];
}

@Injectable()
export class RaceService {
    private raceWebsocket: WebSocket;
    private retry: any;
    private race: Subject<any> = new BehaviorSubject(null);

    constructor(private http: Http, private config: Config) {
        this.startReconnect();
    }

    getRaces(): Observable<Race[]> {
        return this.http.get(this.config.get('race.all')).map(response => {
            return response.json();
        });
    }

    loadRace(name: string): Observable<Race> {
        return this.http.get(this.config.get('race.get').replace(':name', name)).map(response => {
            const result = response.json();
            this.race.next(result);
            return result;
        });
    }

    getRace(): Observable<Race> {
        return this.race;
    }

    saveRace(race: Race): Observable<Race> {
        return this.http.post(this.config.get('race.save').replace(':name', race.name), JSON.stringify(race)).map(response => response.json());
    }

    private startReconnect() {
        this.retry = setInterval(() => {
            try {
                this.raceWebsocket = new WebSocket(this.config.get('race.status'));
                clearInterval(this.retry);
                this.raceWebsocket.onopen    = (evt) => {
                };
                this.raceWebsocket.onmessage = (msg) => {
                    if (msg.data) {
                        const race = JSON.parse(msg.data);
                        this.race.next(race);
                    }
                };
                this.raceWebsocket.onerror   = (error) => {
                    console.log(error);
                    // this.startReconnect();
                };
                this.raceWebsocket.onclose   = (event) => {
                    // this.startReconnect();
                };
            } catch (error) {
                console.error(error);
            }

        }, 1000);
    }
}