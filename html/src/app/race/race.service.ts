import {Injectable} from "@angular/core";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {HttpClient} from "@angular/common/http";
import {Config} from "../app.config";

export interface Roi {
    x: number;
    y: number;
    width: number;
    height: number;
    type?: string;
    color?: string;
}

export interface CarObserver {
    condition_achieved: number;
    type: string;
    active: number;
    roi: Roi;
}

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

    constructor(private http: HttpClient, private config: Config) {
        this.startReconnect();
    }

    getRaces(): Observable<Race[]> {
        return this.http.get(this.config.get('race.all')).map(response => {
            return (response as Race[]);
        });
    }

    loadRace(name: string): Observable<Race> {
        return this.http.get(this.config.get('race.get').replace(':name', name)).map((response: Race) => {
            const result = response;
            this.race.next(result);
            return result;
        });
    }

    getRace(): Observable<Race> {
        return this.race;
    }

    saveRace(race: Race): Observable<Race> {
        const org = this.cloneAndStrip(race);
        return this.http.post(this.config.get('race.save').replace(':name', org.name), JSON.stringify(org))
            .map((response: Race) => response);
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
                        const org = this.cloneAndStrip((this.race as any).value);
                        if (JSON.stringify(org) !== JSON.stringify(race)) {
                            console.log('Race');
                            this.race.next(race);
                        }
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

    private cloneAndStrip(race: Race) {
        const org: Race = JSON.parse(JSON.stringify(race));
        org.observers.forEach(observer => {
            delete observer.roi.type;
            delete observer.roi.color;
        });
        return org;
    }
}